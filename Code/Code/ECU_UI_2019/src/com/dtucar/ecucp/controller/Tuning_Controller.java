package com.dtucar.ecucp.controller;

import com.dtucar.ecucp.communication.TeensySync;
import com.dtucar.ecucp.model.ConfigValue;
import com.dtucar.ecucp.util.Constants.TuningState;
import com.dtucar.ecucp.model.TuningLogValue;
import com.dtucar.ecucp.model.TuningValue;
import com.dtucar.ecucp.util.*;
import com.dtucar.ecucp.communication.TeensySync.EcuEnum;
import com.dtucar.ecucp.view.ConfigTextField;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.control.*;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;

import java.io.*;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.concurrent.TimeUnit;

/**
 * The <code>TuningController</code> is the code behind the <code>TuningView</code>.<br>
 * This view allows the user to perform automated tests on the ECU. Through the table, minimum
 * and maximum speeds are defined. Rhe UI sends burn commands to the car until maximum is reached and coasts until minimum is reached.
 * The test is considered complete when either the user stops it, the distance is achieved, or the last line in the table is completed.<br/>
 * A log file is generated during the test and charts and values are shown showing the performance of the engine.
 */
public class Tuning_Controller extends BaseController {
    private static final String TAG = "TuningController";

    /**
     * The HBox containing the speedChart and the fuelChart (the top TitledPane)
     */
    @FXML
    private HBox speedAndFuelBox;
    /**
     * The TableView which contains the {@link #testData} to configure the test
     */
    @FXML
    private TextField durationText;
    /**
     * This displays the average/mean speed during the test
     */
    @FXML
    private TextField speedText;
    /**
     * This displays the fuel consumption (as sent by the ECU)
     */
    @FXML
    private TextField fuelConsumptionText;
    /**
     * This displays the current gear
     */
    @FXML
    private TextField gearText;
    /**
     * This displays the tuning state
     */
    @FXML
    private TextField stateText;
    /**
     * This indicates whether the test is running or not
     */
    @FXML
    private ToggleButton tuningButton;
    /**
     * The speed/distance chart
     */
    private LineChart<Number, Number> speedChart;
    /**
     * The fuel consumption/distance chart
     */
    private LineChart<Number, Number> fuelChart;
    /**
     * The path to the folder where the log file should be saved
     */
    private OutputStreamWriter writer;
    /**
     * A lock object to synchronize access to the {@link #writer} and prevent multiple threads writing to the log file at the same time
     */
    private final Object writerLock = new Object();
    /**
     * Whether the test is running
     */
    private boolean isTuningRunning = false;
    /**
     * Attempting to start tuning
     */
    private boolean attemptingToStartTuning = false;
    /**
     * Whether a response was received since the last communication request
     */
    private boolean lastRequestAnswered = true;
    /**
     * The current speed and distance (written to onDataReceived and read in the loop)
     */
    private float speed, distance;
    /**
     * The state of the test (stopped, burning or coasting)
     */
    private TuningState tuningState;
    /**
     * The accumulating numerator and denominator for the calculation of the mean/average speed
     */
    private long speedNum, speedDenom;

    /**
     * This list contains the values for the test (with default values)
     */
    private final ObservableList<TuningValue> tuningData = FXCollections.observableArrayList(
            new TuningValue(0, 5, 37),
            new TuningValue(1, 0, 0)
    );

    /**
     * The initialization method runs when the View is added to a Screen but before it is shown (on application start)
     */
    @FXML
    public void initialize() {
        // Initialize
        tuningState = Constants.TuningState.WAITING;
        // Add the speed and fuel charts
        speedChart = Charts.createSpeedDistanceChart();
        fuelChart = Charts.createFuelConsumptionDistanceChart();
        HBox.setHgrow(speedChart, Priority.ALWAYS);
        HBox.setHgrow(fuelChart, Priority.ALWAYS);
        speedAndFuelBox.getChildren().add(speedChart);
        speedAndFuelBox.getChildren().add(fuelChart);
        tuningButton.setText("Start tuning");
        tuningButton.setSelected(false);
    }


    /**
     * React on the test button being pressed
     */
    @FXML
    private void tuningButtonPressed() {
        if (!Locator.getCommunication().isConnected()) {
            // Do not start the test if there is no connection
            if (!isTuningRunning) AppLog.log("Tuning not started (no connection)");
            tuningButton.setSelected(false);
        }
        // Prepare the log file
        else if (!isTuningRunning && !attemptingToStartTuning) {
            attemptingToStartTuning = true;
            // Start the test
            AppLog.log("Tuning request sent");
            new ConfigValue(EcuEnum.TUNING_REQUEST_ON_OFF, 1);
        } else if (isTuningRunning) {
            //TODO: stop the tuning I guess
            AppLog.log("Tuning stopped by user");
            new ConfigValue(EcuEnum.TUNING_REQUEST_ON_OFF, 0);
        }
        // Update the test state (actually start and stop the test)
        //updateTuningRunning(tuningButton.isSelected());
    }

    /**
     * Run when affirmation that the teensy is in INIT_SWEEP and is waiting
     */
    private void confirmTunningIsRunning() {
        if (tuningButton.isSelected()) {
            if (attemptingToStartTuning) {
                updateTuningRunning(true);
                attemptingToStartTuning = false;
                TeensySync.sendConfig(new ConfigValue(EcuEnum.AUTOGEAR, 1),
                        new ConfigValue(EcuEnum.TUNING_IN_INIT_WAITING_FOR_LOG_SETUP, 1));
            } else {
                AppLog.log("In confirmTunningIsRunning(), button pressed but not attempting to start. How did you get here?");
            }
        } else {
            AppLog.log("In confirmTunningIsRunning(), button not pressed. How did you get here?");
        }
    }

    /**
     * Start or stop the test
     * Run this when it's been confirmed that the tuning has started.
     *
     * @param isTuningRunning true to start test, false to stop it
     */
    private void updateTuningRunning(boolean isTuningRunning) {
        // Set TEST_STOPPED (both when stopping and starting a test)
        //tuningState = TuningState.TEST_STOPPED;
        // If stopping the tuning, set the state immediately
        // Tuning is starting: Make sure autogear is set and reset data
        TeensySync.sendConfig(new ConfigValue(EcuEnum.AUTOGEAR, 1),
                new ConfigValue(EcuEnum.RESET_DISTANCE, 1),
                new ConfigValue(EcuEnum.RESET_FUEL_CONSUMPTION, 1));
        // Run on UI thread for UI updates
        Platform.runLater(() -> {
            if (isTuningRunning) {
                // Start test (reset data and charts and indicate tuning running)
                tuningButton.setText("Stop tuning");
                tuningButton.setSelected(true);
                speedDenom = 0;
                speedNum = 0;
                speed = 0;
                distance = 0;
                Charts.speedDistanceSeries.getData().clear();
                Charts.fuelSeries.getData().clear();
                durationText.setText("00:00");
                // If starting the tuning, set the state after initializations
                this.isTuningRunning = true;
            } else {
                tuningButton.setText("Start tuning");
                tuningButton.setSelected(false);
            }
        });
    }


    /**
     * This function is called from {@link MainController} when the associated tab or page is hidden from the user.
     * If the tuning is running, hide() is not called (which will continue the tuning in the background).
     */
    @Override
    public void hide() {
        if (!isTuningRunning) super.hide();
    }

    /**
     * The loop that runs every {@link Constants#BASE_LOOP_PERIOD}
     */
    @Override
    protected void loop() {
        // Loop!
        if (!Locator.getCommunication().isConnected()) {
            // If we're not connected (e.g. lost connection) stop any running tuning
            lastRequestAnswered = true;
            updateTuningRunning(false);
        } else if (lastRequestAnswered && isTuningRunning) {
            // We're connected and a tuning is running
            lastRequestAnswered = false;
            // Request data from Teensy
            TeensySync.requestConfig(
                    EcuEnum.STARTER,
                    EcuEnum.BATTERY_VOLTAGE,
                    EcuEnum.DISTANCE,
                    EcuEnum.SPEED,
                    EcuEnum.RPM,
                    EcuEnum.FUEL_CONSUMPTION,
                    EcuEnum.FUEL_CONSUMED,
                    EcuEnum.LAMBDA,
                    EcuEnum.OIL_TEMPERATURE,
                    EcuEnum.WATER_TEMPERATURE,
                    EcuEnum.EXHAUST_TEMPERATURE,
                    EcuEnum.GEAR,
                    EcuEnum.FUEL_CORRECTION,
                    EcuEnum.AUTOGEAR,
                    EcuEnum.BRAKE_FRONT,
                    EcuEnum.BRAKE_REAR
            );
        }
    }

    /**
     * Get the TAG string, identifying the controller
     *
     * @return TAG
     */
    @Override
    public String getTag() {
        return TAG;
    }

    /**
     * This function is called from {@link MainController} when the associated tab or page is shown to the user
     */
    @Override
    public void show() {
        super.show();
        lastRequestAnswered = true;
    }

    /**
     * Called when new data is received to display it on the UI
     *
     * @param data the hash map of the new data received
     */
    @Override
    public void onDataReceived(HashMap<String, Object> data) {
        Platform.runLater(() -> {
            TuningLogValue tuningLogValue = null;
            // Data received from Teensy
            // Only update some parts of the UI if the tuning is running
            /*if(attemptingToStartTuning){
                if (data.containsKey(EcuEnum.TUNING_IN_INIT_WAITING_FOR_LOG_SETUP.get())){
                    confirmTunningIsRunning();
                }
            }
            if(isTuningRunning) {
                if(data.containsKey(EcuEnum.FUEL_CORRECTION.get())) {
                    tuningLogValue.setFuelCorrection(((Number) data.get(EcuEnum.FUEL_CORRECTION.get())).floatValue());
                }
                if(data.containsKey(EcuEnum.SPEED.get())) {
                    speed = ((Number) data.get(EcuEnum.SPEED.get())).floatValue();
                    speedText.setText(StringUtil.doubleToShortString(speed));
                    }
                    tuningLogValue.setSpeed(speed);
                }
                if(data.containsKey(EcuEnum.GEAR.get())) {
                    int gear = ((Number) data.get(EcuEnum.GEAR.get())).intValue();
                    gearText.setText(Integer.toString(gear));
                }
                if(data.containsKey(EcuEnum.TUNING_REQUEST_ON_OFF.get())){
                    this.isTuningRunning = false;
                }
                if(data.containsKey(EcuEnum.TUNING_STATE.get())) {
                    int tState = ((Number) data.get(EcuEnum.TUNING_STATE.get())).intValue();
                    switch (tState){
                        case 1:
                            stateText.setText("NOT_TUNING");
                            break;
                        case 2:
                            stateText.setText("INIT_SWEEP");
                            break;
                        case 3:
                            stateText.setText("SWEEPING");
                            break;
                        case 4:
                            stateText.setText("PROCESS_DATA");
                            break;
                        case 5:
                            stateText.setText("SPEED_DOWN");
                            break;
                        case 6:
                            stateText.setText("EXIT_TUNING");
                            break;
                        case 7:
                            stateText.setText("WAITING");
                            break;
                        default:
                            stateText.setText("UNKNOWN");
                            break;
                    }
                    tuningLogValue.setEcuState(tState);
                }
                if(data.containsKey(EcuEnum.TUNING_STATE.get())){
                    TuningState tState = ((TuningState) data.get(EcuEnum.TUNING_STATE.get()));
                    tuningLogValue.setTuningState(tState);
                }

            // End of data received from Teensy
            // Data received from saved file
            //if(data.containsKey(EcuEnum.TEST_VALUES.get())) {
            //    Object tuningObject = data.get(EcuEnum.TEST_VALUES.get());
                //noinspection unchecked
            //    List<TuningValue> list = (List<TuningValue>) tuningObject;
            //    list.sort(Comparator.comparing(TuningValue::getIndex));
            //    tuningData.setAll(list);
            //    tuningTable.refresh();
            //}
            // End of data received from saved file
            // Write to log
            lastRequestAnswered = true;
             */
        });
    }
}
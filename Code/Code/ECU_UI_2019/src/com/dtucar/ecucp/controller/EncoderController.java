package com.dtucar.ecucp.controller;

import com.dtucar.ecucp.model.ConfigValue;
import com.dtucar.ecucp.util.Charts;
import com.dtucar.ecucp.util.Constants;
import com.dtucar.ecucp.util.Locator;
import com.dtucar.ecucp.util.StringUtil;
import com.dtucar.ecucp.communication.TeensySync;
import com.dtucar.ecucp.communication.TeensySync.EcuEnum;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.layout.GridPane;

import java.util.HashMap;

/**
 * The <code>EncoderController</code> is the code behind the <code>EncoderView</code>.<br>
 * This view allows calibration of the engine encoder to ensure TDC positions are aligned correctly.
 * It also shows information about the wheel encoder data to ease debugging and calibration.
 */
public class EncoderController extends BaseController {
	private static final String TAG = "EncoderController";

	@FXML
	private TextField aPulseCount;
	@FXML
	private TextField bPulseCount;
	@FXML
	private TextField zPulseCount;
	@FXML
	private TextField tdc1Text;
	@FXML
	private TextField tdc2Text;
	@FXML
	private TextField position1Text;
	@FXML
	private TextField position2Text;
	@FXML
	private TextField speedText;
	@FXML
	private TextField distanceText;
	@FXML
	private TextField encoderNumProblemsText;
	@FXML
	private TextField encoderDistance0Text;
	@FXML
	private TextField encoderDistance1Text;
	@FXML
	private TitledPane engineEncoderPane;
	@FXML
	private TitledPane wheelEncoderPane;
	@FXML
	private GridPane wheelEncoderGridPane;

	/**
	 * Whether a response was received since the last communication request
	 */
	private boolean lastRequestAnswered = true;

	/**
	 * The initialization method runs when the View is added to a Screen but before it is shown (on application start)
	 */
	@FXML
	public void initialize() {
		// Add speed/time and distance/time charts (for wheel encoder)
		wheelEncoderGridPane.add(Charts.createSpeedTimeChart(), 0, 1);
		wheelEncoderGridPane.add(Charts.createDistanceTimeChart(), 0, 2);
		// Add context menu to reset distance
		ContextMenu distanceContext = new ContextMenu();
		MenuItem distanceReset = new MenuItem("Reset");
		distanceReset.setOnAction(event -> TeensySync.sendConfig(new ConfigValue(EcuEnum.RESET_DISTANCE, 1)));
		distanceContext.getItems().addAll(distanceReset);
		distanceText.setContextMenu(distanceContext);
	}

	/**
	 * The loop that runs every {@link Constants#BASE_LOOP_PERIOD}
	 */
	@Override
	protected void loop() {
		if(!Locator.getCommunication().isConnected()) {
			// Do nothing if communication is not connected
			lastRequestAnswered = true;
			Charts.setTimeChartsStartTime(System.currentTimeMillis());
			Charts.speedTimeSeries.getData().clear();
			Charts.distanceSeries.getData().clear();
		} else if(lastRequestAnswered) {
			// We're connected
			lastRequestAnswered = false;
			// If the wheel encoder pane is expanded, request the relevant data
			if(wheelEncoderPane.isExpanded()) {
				TeensySync.requestConfig(
						EcuEnum.ENCODER,
						EcuEnum.DISTANCE,
						EcuEnum.SPEED
				);
			}
			// If the engine encoder pane is expanded, request the relevant data
			if(engineEncoderPane.isExpanded()) {
				TeensySync.requestConfig(
						EcuEnum.ENGINE_ENCODER_A_PULSE_COUNT,
						EcuEnum.ENGINE_ENCODER_B_PULSE_COUNT,
						EcuEnum.ENGINE_ENCODER_Z_PULSE_COUNT
				);
			}
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
	 * Called when new data is received to update the UI
	 *
	 * @param data the hash map of the new data received
	 */
	@Override
	public void onDataReceived(HashMap<String, Object> data) {
		Platform.runLater(() -> {
			if(data.containsKey(EcuEnum.ENC_NUM_PROBLEMS.get())) encoderNumProblemsText.setText(String.valueOf(data.get(EcuEnum.ENC_NUM_PROBLEMS.get())));
			if(data.containsKey(EcuEnum.ENC_DISTANCE_0.get())) encoderDistance0Text.setText(String.valueOf(data.get(EcuEnum.ENC_DISTANCE_0.get())));
			if(data.containsKey(EcuEnum.ENC_DISTANCE_1.get())) encoderDistance1Text.setText(String.valueOf(data.get(EcuEnum.ENC_DISTANCE_1.get())));
			if(data.containsKey(EcuEnum.DISTANCE.get())) distanceText.setText(StringUtil.doubleToShortString(((Number) data.get(EcuEnum.DISTANCE.get())).floatValue()) + " m");
			if(data.containsKey(EcuEnum.SPEED.get())) speedText.setText(StringUtil.doubleToShortString(((Number) data.get(EcuEnum.SPEED.get())).floatValue()) + " km/h");
			if(data.containsKey(EcuEnum.ENGINE_ENCODER_A_PULSE_COUNT.get())) aPulseCount.setText(String.valueOf(data.get(EcuEnum.ENGINE_ENCODER_A_PULSE_COUNT.get())));
			if(data.containsKey(EcuEnum.ENGINE_ENCODER_B_PULSE_COUNT.get())) bPulseCount.setText(String.valueOf(data.get(EcuEnum.ENGINE_ENCODER_B_PULSE_COUNT.get())));
			if(data.containsKey(EcuEnum.ENGINE_ENCODER_Z_PULSE_COUNT.get())) zPulseCount.setText(String.valueOf(data.get(EcuEnum.ENGINE_ENCODER_Z_PULSE_COUNT.get())));
			lastRequestAnswered = true;
		});
	}

	/**
	 * Calculate the TDC (Top Dead Center) based on the positions saved
	 * (one needs some knowledge about the engine to feel when a TDC has been reached and take the positions when this happens)
	 */
	private void calculateTdc() {
		int position1, position2;
		try {
			position1 = Integer.parseInt(position1Text.getText());
		} catch(NumberFormatException nfe) {
			return;
		}
		try {
			position2 = Integer.parseInt(position2Text.getText());
		} catch(NumberFormatException nfe) {
			return;
		}
		int tdc1 = 360 - ((position1 + position2) / 2);
		int tdc2 = (tdc1 + 360) % 720;
		tdc1Text.setText(String.valueOf(tdc1));
		tdc2Text.setText(String.valueOf(tdc2));
	}

	/**
	 * Save position 1 when the position 1 button is pressed and calculate TDC
	 */
	@FXML
	private void onTakePosition1() {
		position1Text.setText(aPulseCount.getText());
		calculateTdc();
	}

	/**
	 * Save position 2 when the position 2 button is pressed and calculate TDC
	 */
	@FXML
	private void onTakePosition2() {
		position2Text.setText(aPulseCount.getText());
		calculateTdc();
	}

	/**
	 * Reset the wheel encoder data when the reset button in the wheel encoder pane is pressed
	 */
	@FXML
	private void onResetPressed() {
		TeensySync.sendConfig(new ConfigValue(EcuEnum.RESET_DISTANCE, 1));
		Charts.setTimeChartsStartTime(System.currentTimeMillis());
		Charts.clearAllCharts();
	}
}

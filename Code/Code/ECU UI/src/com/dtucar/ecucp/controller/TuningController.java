      ackage com.dtucar.ecucp.controller;

      import com.dtucar.ecucp.communication.TeensySync;
      import com.dtucar.ecucp.model.ConfigValue;
      import com.dtucar.ecucp.model.TestLogValue;
      import com.dtucar.ecucp.util.Constants.TestState;
      import com.dtucar.ecucp.model.TestValue;
      import com.dtucar.ecucp.util.*;
      import com.dtucar.ecucp.communication.TeensySync.EcuEnum;
      import com.dtucar.ecucp.view.ConfigTextField;
      import com.google.gson.JsonArray;
      import com.google.gson.JsonObject;
      import javafx.application.Platform;
      import javafx.collections.FXCollections;
      import javafx.collections.ObservableList;
      import javafx.event.EventHandler;
      import javafx.fxml.FXML;
      import javafx.scene.chart.LineChart;
      import javafx.scene.chart.NumberAxis;
      import javafx.scene.control.*;
      import javafx.scene.control.cell.PropertyValueFactory;
      import javafx.scene.layout.HBox;
      import javafx.scene.layout.Priority;
      import javafx.util.Callback;

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
       */
      public class TuningController extends BaseController {
      	private static final String TAG = "TuninController";

      	/**
      	 * The HBox containing the speedChart and the fuelChart (the top TitledPane)
      	 */
      	@FXML
      	private HBox speedAndFuelBox;
      	/**
      	 * The second TitledPane which contains the Lambda/RPM chart
      	 */
      	@FXML
      	private TitledPane lambdaPane;
      	/**
      	 * The TableView which contains the {@link #testData} to configure the test
      	 */
      	@FXML
      	private TableView testTable;
      	/**
      	 * A ConfigTextField to set the distance value of the test
      	 */
      	@FXML
      	private ConfigTextField testDistanceText;
      	/**
      	 * This displays the current duration of the test
      	 */
      	@FXML
      	private TextField durationText;
      	/**
      	 * This displays the average/mean speed during the test
      	 */
      	@FXML
      	private TextField meanSpeedText;
      	/**
      	 * This displays the distance travelled during the test
      	 */
      	@FXML
      	private TextField distanceText;
      	/**
      	 * This displays the fuel consumed during the test
      	 */
      	@FXML
      	private TextField fuelConsumedText;
      	/**
      	 * The displays the current speed
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
      	 * This indicates whether the test is running or not
      	 */
      	@FXML
      	private ProgressIndicator testProgressIndicator;
      	/**
      	 * This indicates whether the test is running or not (could be used to indicate actual progress)
      	 */
      	@FXML
      	private ProgressBar testProgress;
      	/**
      	 * This button starts and stops the test
      	 */
      	@FXML
      	private ToggleButton testButton;

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
      	private File logFolder;
      	/**
      	 * A stream writer to write to the log file
      	 */
      	private OutputStreamWriter writer;
      	/**
      	 * A lock object to synchronize access to the {@link #writer} and prevent multiple threads writing to the log file at the same time
      	 */
      	private final Object writerLock = new Object();
      	/**
      	 * The time in millis when the test was started
      	 */
      	private long millisAtTestStart = 0;
      	/**
      	 * The time in millis when the last speed reading was received
      	 */
      	private long millisAtLastSpeed = 0;
      	/**
      	 * Whether the test is running
      	 */
      	private boolean isTestRunning = false;
      	/**
      	 * Whether a response was received since the last communication request
      	 */
      	private boolean lastRequestAnswered = true;
      	/**
      	 * The index for the current row of the test table that is being run
      	 */
      	private int tableDataRowRunning;
      	/**
      	 * The current speed and distance (written to onDataReceived and read in the loop)
      	 */
      	private float speed, distance;
      	/**
      	 * The state of the test (stopped, burning or coasting)
      	 */
      	private TestState testState;
      	/**
      	 * The accumulating numerator and denominator for the calculation of the mean/average speed
      	 */
      	private long speedNum, speedDenom;

      	/**
      	 * This list contains the values for the test (with default values)
      	 */
      	private final ObservableList<TestValue> testData = FXCollections.observableArrayList(
      			new TestValue(0, 5, 37),
      			new TestValue(1, 0, 0)
      	);

      	/**
      	 * The initialization method runs when the View is added to a Screen but before it is shown (on application start)
      	 */
      	@FXML
      	public void initialize() {
      		// Initialize
      		testState = TestState.TEST_STOPPED;
      		// Set up test table
      		setupTable();
      		// Add the speed and fuel charts
      		speedChart = Charts.createSpeedDistanceChart();
      		fuelChart = Charts.createFuelConsumptionDistanceChart();
      		HBox.setHgrow(speedChart, Priority.ALWAYS);
      		HBox.setHgrow(fuelChart, Priority.ALWAYS);
      		speedAndFuelBox.getChildren().add(speedChart);
      		speedAndFuelBox.getChildren().add(fuelChart);
      		// Add Lambda/RPM chart
      		lambdaPane.setContent(Charts.createLambdaRpmChartTest());
      		// Prepare test distance text with default value
      		testDistanceText.setTextConfirmed("2000", false);
      		// Prepare context menu for changing the log folder (right click on button)
      		ContextMenu contextMenu = new ContextMenu();
      		MenuItem chooseFolder = new MenuItem("Set log folder...");
      		chooseFolder.setOnAction(event -> chooseLogDirectory());
      		contextMenu.getItems().addAll(chooseFolder);
      		testButton.setContextMenu(contextMenu);
      	}

      	/**
      	 * Open a dialog to choose the directory where the log file should be saved
      	 */
      	private void chooseLogDirectory() {
      		File logDirectory = SaveAndLoad.chooseDirectory();
      		if(logDirectory != null) {
      			this.logFolder = logDirectory;
      			// The test button starts with "Choose folder" text
      			// Once a folder has been chosen, it changes to "Start test"
      			testButton.setText("Start test");
      			testButton.setSelected(false);
      		}
      	}

      	/**
      	 * Set up the test table
      	 */
      	@SuppressWarnings("unchecked")
      	private void setupTable() {
      		// Set the minSpeed column properties
      		Callback<TableColumn, TableCell> cellFactory = p -> new TableUtil.EditingCell<TestValue>();
      		TableColumn minSpeedColumn = new TableColumn("Min [km/h]");
      		minSpeedColumn.setSortable(false);
      		minSpeedColumn.setCellValueFactory(new PropertyValueFactory<TestValue, Integer>("minSpeed"));
      		minSpeedColumn.setCellFactory(cellFactory);
      		minSpeedColumn.setOnEditCommit(
      				(EventHandler<TableColumn.CellEditEvent<TestValue, Integer>>) t -> {
      					TestValue editedValue = t.getTableView().getItems().get(t.getTablePosition().getRow());
      					editedValue.setMinSpeed(t.getNewValue());
      				}
      		);
      		// Set the maxSpeed column properties
      		TableColumn maxSpeedColumn = new TableColumn("Max [km/h]");
      		maxSpeedColumn.setSortable(false);
      		maxSpeedColumn.setCellValueFactory(new PropertyValueFactory<TestValue, Integer>("maxSpeed"));
      		maxSpeedColumn.setCellFactory(cellFactory);
      		maxSpeedColumn.setOnEditCommit(
      				(EventHandler<TableColumn.CellEditEvent<TestValue, Integer>>) t -> {
      					TestValue editedValue = t.getTableView().getItems().get(t.getTablePosition().getRow());
      					editedValue.setMaxSpeed(t.getNewValue());
      				}
      		);
      		// Set up general things
      		TableUtil.setupTable(testTable);
      		testTable.setItems(testData);
      		testTable.getColumns().addAll(minSpeedColumn, maxSpeedColumn);
      		// Add context menu to table (for add/delete)
      		ContextMenu contextMenu = TableUtil.getImportExportContextMenu(testData);
      		MenuItem addRow = new MenuItem("Add row");
      		addRow.setOnAction(event -> addRow());
      		MenuItem deleteRow = new MenuItem("Delete row");
      		deleteRow.setOnAction(event -> deleteRow());
      		contextMenu.getItems().addAll(0, Arrays.asList(addRow, deleteRow, new SeparatorMenuItem()));
      		testTable.setContextMenu(contextMenu);
      	}

      	/**
      	 * Add a row to the end of the table
      	 */
      	@SuppressWarnings("unchecked")
      	private void addRow() {
      		TablePosition pos = testTable.getFocusModel().getFocusedCell();
      		testTable.getSelectionModel().clearSelection();
      		TestValue data = new TestValue(testTable.getItems().size(), 0, 0);
      		testTable.getItems().add(data);
      		int row = testTable.getItems().size() - 1;
      		testTable.getSelectionModel().select(row, pos.getTableColumn());
      	}

      	/**
      	 * Delete the selected row from the table
      	 */
      	private void deleteRow() {
      		Object selectedItem = testTable.getSelectionModel().getSelectedItem();
      		testTable.getItems().remove(selectedItem);
      	}

      	/**
      	 * React on the test button being pressed
      	 */
      	@FXML
      	private void testButtonPressed() {
      		if(!Locator.getCommunication().isConnected()) {
      			// Do not start the test if there is not connection
      			if(!isTestRunning) AppLog.log("Test not started (no connection)");
      			testButton.setSelected(false);
      			return;
      		}
      		if(!isTestRunning) {
      			// Start the test
      			// If no directory for the logs has been chosen, open a dialog for that now
      			if(logFolder == null) {
      				chooseLogDirectory();
      				return;
      			}
      			// Prepare the log file
      			DateTimeFormatter dtf = DateTimeFormatter.ofPattern("yyyy-MM-dd HH-mm-ss");
      			LocalDateTime now = LocalDateTime.now();
      			String time = dtf.format(now);
      			File logFile = new File(logFolder + File.separator + time + " " + Locator.getResources().getString("suggested_test_file_name") + ".csv");
      			File configLogFile = new File(logFolder + File.separator + time + " " + Locator.getResources().getString("suggested_test_config_file_name") + ".txt");
      			// Save config alongside the log file
      			Locator.getMainController().save(configLogFile);
      			try {
      				synchronized(writerLock) {
      					writer = new OutputStreamWriter(new FileOutputStream(logFile, true), "UTF-8");
      					writer.write(TestLogValue.getHeader());
      				}
      			} catch(IOException e) {
      				e.printStackTrace();
      				AppLog.error("Could not write to log: " + e.getMessage());
      				DialogUtil.showExceptionDialog(e);
      			}
      			AppLog.log("Test started");
      		} else {
      			AppLog.log("Test stopped by user");
      		}
      		// Update the test state (actually start and stop the test)
      		updateTestRunning(testButton.isSelected());
      	}

      	/**
      	 * Start or stop the test
      	 *
      	 * @param isTestRunning true to start test, false to stop it
      	 */
      	private void updateTestRunning(boolean isTestRunning) {
      		// Set TEST_STOPPED (both when stopping and starting a test)
      		testState = TestState.TEST_STOPPED;
      		// If stopping the test, set the state immediately
      		if(!isTestRunning) {
      			// Test is stopping
      			this.isTestRunning = false;
      			// Make sure burn is stopped
      			TeensySync.sendConfig(new ConfigValue(EcuEnum.BURN, 0));
      		} else {
      			// Test is starting: Make sure autogear is set and reset data
      			TeensySync.sendConfig(new ConfigValue(EcuEnum.AUTOGEAR, 1),
      					new ConfigValue(EcuEnum.RESET_DISTANCE, 1),
      					new ConfigValue(EcuEnum.RESET_FUEL_CONSUMPTION, 1),
      					new ConfigValue(EcuEnum.BURN, 0));
      		}
      		// Run on UI thread for UI updates
      		Platform.runLater(() -> {
      			if(isTestRunning) {
      				// Start test (reset data and charts and indicate test running)
      				testButton.setText("Stop test");
      				testButton.setSelected(true);
      				tableDataRowRunning = 0;
      				speedDenom = 0;
      				speedNum = 0;
      				speed = 0;
      				distance = 0;
      				millisAtTestStart = System.currentTimeMillis();
      				millisAtLastSpeed = 0;
      				Charts.speedDistanceSeries.getData().clear();
      				Charts.fuelSeries.getData().clear();
      				Charts.lambdaG1SeriesTest.getData().clear();
      				Charts.lambdaG2SeriesTest.getData().clear();
      				durationText.setText("00:00");
      				testProgress.setProgress(ProgressBar.INDETERMINATE_PROGRESS);
      				testProgressIndicator.setProgress(ProgressBar.INDETERMINATE_PROGRESS);
      				if(!testDistanceText.getText().isEmpty()) {
      					if(testDistanceText.getValue() != null) {
      						((NumberAxis) speedChart.getXAxis()).setUpperBound(testDistanceText.getValue());
      						((NumberAxis) fuelChart.getXAxis()).setUpperBound(testDistanceText.getValue());
      					}
      				}
      				// If starting the test, set the state after initializations
      				this.isTestRunning = true;
      			} else {
      				// Stop test (indicate test stopped and close log)
      				if(logFolder != null) {
      					testButton.setText("Start test");
      					testButton.setSelected(false);
      				}
      				testProgress.setProgress(100);
      				testProgressIndicator.setProgress(100);
      				if(writer != null) {
      					try {
      						synchronized(writerLock) {
      							writer.flush();
      							writer.close();
      							writer = null;
      						}
      					} catch(IOException e) {
      						e.printStackTrace();
      						AppLog.error("Could not write to log: " + e.getMessage());
      						DialogUtil.showExceptionDialog(e);
      					}
      				}
      			}
      		});
      	}

      	/**
      	 * This function is called from {@link MainController} when the associated tab or page is hidden from the user.
      	 * If the test is running, hide() is not called (which will continue the test in the background).
      	 */
      	@Override
      	public void hide() {
      		if(!isTestRunning) super.hide();
      	}

      	/**
      	 * The loop that runs every {@link Constants#BASE_LOOP_PERIOD}
      	 */
      	@Override
      	protected void loop() {
      		// Loop!
      		if(!Locator.getCommunication().isConnected()) {
      			// If we're not connected (e.g. lost connection) stop any running test
      			lastRequestAnswered = true;
      			updateTestRunning(false);
      			Platform.runLater(() -> {
      				testProgress.setProgress(0);
      				testProgressIndicator.setProgress(0);
      			});
      		} else if(lastRequestAnswered && isTestRunning) {
      			// We're connected and a test is running
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
      			// Update the ECU Burn state
      			TeensySync.sendConfig(new ConfigValue(EcuEnum.BURN, testState == TestState.TEST_BURNING ? 1 : 0));
      		}
      		if(isTestRunning) {
      			// Manage the running test
      			// Update time running
      			long millisRunning = System.currentTimeMillis() - millisAtTestStart;
      			String duration = String.format("%02d:%02d", TimeUnit.MILLISECONDS.toMinutes(millisRunning), TimeUnit.MILLISECONDS.toSeconds(millisRunning) - TimeUnit.MINUTES.toSeconds(TimeUnit.MILLISECONDS.toMinutes(millisRunning)));
      			Platform.runLater(() -> durationText.setText(duration));
      			// Stop if distance reached
      			if(!testDistanceText.getText().isEmpty() && !distanceText.getText().isEmpty()) {
      				if(testDistanceText.getValue() > 0 && distance >= testDistanceText.getValue()) {
      					AppLog.log("Test stopped by distance");
      					updateTestRunning(false);
      				}
      			}
      			// Change commands according to table
      			if(testData.size() > tableDataRowRunning) {
      				// Get the current test value
      				TestValue testValue = testData.get(tableDataRowRunning);
      				if(speed >= testValue.getMaxSpeed() && testState == TestState.TEST_BURNING) {
      					// If we're above or at max speed and burning, stop the burn (coast)
      					testState = TestState.TEST_COASTING;
      					TeensySync.sendConfig(new ConfigValue(EcuEnum.BURN, 0));
      					tableDataRowRunning++;
      					AppLog.log("Test: Max speed reached: coasting");
      				} else if(speed <= testValue.getMinSpeed() && (testState == TestState.TEST_STOPPED || testState == TestState.TEST_COASTING)) {
      					// If we're below or at min speed, and stopped or coasting
      					if(testValue.getMaxSpeed() > 0) {
      						// And the max speed is higher than 0, then start burn
      						testState = TestState.TEST_BURNING;
      						TeensySync.sendConfig(new ConfigValue(EcuEnum.BURN, 1));
      						AppLog.log("Test: Min speed reached: burning");
      					} else {
      						tableDataRowRunning++;
      						AppLog.log("Test: Min speed reached but next speed 0: continuing");
      					}
      				}
      			} else {
      				// The are no more test values left
      				AppLog.log("Test stopped by end of table");
      				TeensySync.sendConfig(new ConfigValue(EcuEnum.BURN, 0));
      				updateTestRunning(false);
      			}
      		} else {
      			// Test not running
      			testState = TestState.TEST_STOPPED;
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
      			TestLogValue testLogValue = null;
      			// Data received from Teensy
      			// Only update some parts of the UI if the test is running
      			if(isTestRunning) {
      				testLogValue = new TestLogValue();
      				testLogValue.setOnTimeMillis(System.currentTimeMillis() - millisAtTestStart);
      				testLogValue.setTimeUtcMillis(System.currentTimeMillis());
      				testLogValue.setTestState(testState);
      				if(data.containsKey(EcuEnum.STARTER.get())) {
      					boolean isStarterRunning = ((Number) data.get(EcuEnum.STARTER.get())).intValue() == 1;
      					testLogValue.setStarter(isStarterRunning);
      				}
      				if(data.containsKey(EcuEnum.GEAR.get())) {
      					testLogValue.setGear(((Number) data.get(EcuEnum.GEAR.get())).intValue());
      				}
      				if(data.containsKey(EcuEnum.AUTOGEAR.get())) {
      					testLogValue.setAutoGear(((Number) data.get(EcuEnum.AUTOGEAR.get())).intValue() == 1);
      				}
      				if(data.containsKey(EcuEnum.BATTERY_VOLTAGE.get())) {
      					float batteryVoltage = ((Number) data.get(EcuEnum.BATTERY_VOLTAGE.get())).floatValue();
      					testLogValue.setBatteryVoltage(batteryVoltage);
      				}
      				if(data.containsKey(EcuEnum.DISTANCE.get())) {
      					distance = ((Number) data.get(EcuEnum.DISTANCE.get())).floatValue();
      					distanceText.setText(StringUtil.doubleToShortString(distance));
      					testLogValue.setDistance(distance);
      				}
      				if(data.containsKey(EcuEnum.FUEL_CORRECTION.get())) {
      					testLogValue.setFuelCorrection(((Number) data.get(EcuEnum.FUEL_CORRECTION.get())).floatValue());
      				}
      				if(data.containsKey(EcuEnum.FUEL_CONSUMPTION.get())) {
      					float fuelConsumption = ((Number) data.get(EcuEnum.FUEL_CONSUMPTION.get())).floatValue();
      					fuelConsumedText.getTooltip().setText(StringUtil.doubleToShortString(fuelConsumption) + " km/l");
      					fuelConsumptionText.setText(StringUtil.doubleToShortString(fuelConsumption));
      					testLogValue.setFuelConsumption(((Number) data.get(EcuEnum.FUEL_CONSUMPTION.get())).floatValue());
      				}
      				if(data.containsKey(EcuEnum.FUEL_CONSUMED.get())) {
      					fuelConsumedText.setText(String.valueOf(data.get(EcuEnum.FUEL_CONSUMED.get())));
      					testLogValue.setFuelConsumed(((Number) data.get(EcuEnum.FUEL_CONSUMED.get())).floatValue());
      				}
      				if(data.containsKey(EcuEnum.SPEED.get())) {
      					speed = ((Number) data.get(EcuEnum.SPEED.get())).floatValue();
      					speedText.setText(StringUtil.doubleToShortString(speed));
      					if(isTestRunning && speed > 0) {
      						if(millisAtLastSpeed > 0) {
      							int millisSinceLastSpeed = (int) (System.currentTimeMillis() - millisAtLastSpeed);
      							speedDenom += millisSinceLastSpeed;
      							speedNum += speed * millisSinceLastSpeed;
      							float averageSpeed = speedNum / (float) speedDenom;
      							meanSpeedText.setText(StringUtil.doubleToShortString(averageSpeed));
      						}
      						millisAtLastSpeed = System.currentTimeMillis();
      					}
      					testLogValue.setSpeed(speed);
      				}
      				if(data.containsKey(EcuEnum.RPM.get())) testLogValue.setRpm(((Number) data.get(EcuEnum.RPM.get())).floatValue());
      				if(data.containsKey(EcuEnum.LAMBDA.get())) testLogValue.setLambda(((Number) data.get(EcuEnum.LAMBDA.get())).floatValue());
      				if(data.containsKey(EcuEnum.WATER_TEMPERATURE.get()))
      					testLogValue.setWaterTemperature(((Number) data.get(EcuEnum.WATER_TEMPERATURE.get())).floatValue());
      				if(data.containsKey(EcuEnum.OIL_TEMPERATURE.get()))
      					testLogValue.setOilTemperature(((Number) data.get(EcuEnum.OIL_TEMPERATURE.get())).floatValue());
      				if(data.containsKey(EcuEnum.EXHAUST_TEMPERATURE.get()))
      					testLogValue.setExhaustTemperature(((Number) data.get(EcuEnum.EXHAUST_TEMPERATURE.get())).floatValue());
      				if(data.containsKey(EcuEnum.BRAKE_FRONT.get())) {
      					float brakeFrontPressure = ((Number) data.get(EcuEnum.BRAKE_FRONT.get())).floatValue();
      					testLogValue.setBrakeFront(brakeFrontPressure);
      				}
      				if(data.containsKey(EcuEnum.BRAKE_REAR.get())) {
      					float brakeRearPressure = ((Number) data.get(EcuEnum.BRAKE_REAR.get())).floatValue();
      					testLogValue.setBrakeRear(brakeRearPressure);
      				}
      				if(data.containsKey(EcuEnum.GEAR.get())) {
      					int gear = ((Number) data.get(EcuEnum.GEAR.get())).intValue();
      					gearText.setText(Integer.toString(gear));
      				}
      				if(data.containsKey(EcuEnum.STATE.get())) {
      					int state = ((Number) data.get(EcuEnum.STATE.get())).intValue();
      					testLogValue.setEcuState(state);
      				}
      			}
      			// End of data received from Teensy
      			// Data received from saved file
      			if(data.containsKey(EcuEnum.TEST_VALUES.get())) {
      				Object testObject = data.get(EcuEnum.TEST_VALUES.get());
      				//noinspection unchecked
      				List<TestValue> list = (List<TestValue>) testObject;
      				list.sort(Comparator.comparing(TestValue::getIndex));
      				testData.setAll(list);
      				testTable.refresh();
      			}
      			if(data.containsKey(EcuEnum.TEST_DISTANCE.get())) testDistanceText.setTextConfirmed(String.valueOf(data.get(EcuEnum.TEST_DISTANCE.get())), false);
      			if(data.containsKey(Constants.TEST_LOG_FOLDER_KEY)) {
      				logFolder = new File((String) data.get(Constants.TEST_LOG_FOLDER_KEY));
      				if(!logFolder.exists()) logFolder = null;
      				if(logFolder != null) testButton.setText("Start test");
      			}
      			// End of data received from saved file
      			// Write to log
      			if(testLogValue != null && writer != null
      					&& data.containsKey(EcuEnum.LAMBDA.get())
      					&& data.containsKey(EcuEnum.AUTOGEAR.get())
      					&& data.containsKey(EcuEnum.FUEL_CORRECTION.get())) {
      				try {
      					String testLogString = testLogValue.toString();
      					if(Constants.DEBUG) System.out.println("Writing to log:\n" + testLogString);
      					synchronized(writerLock) {
      						writer.write(testLogString);
      					}
      				} catch(IOException e) {
      					e.printStackTrace();
      					AppLog.error("Could not write to log: " + e.getMessage());
      					DialogUtil.showExceptionDialog(e);
      				}
      			}
      			lastRequestAnswered = true;
      		});
      	}

      	/**
      	 * Produce a JsonArray containing all settings that need to be saved when using the "Save Configuration" function
      	 *
      	 * @return jsonArray with the settings to save
      	 */
      	public JsonArray getAll() {
      		// Save the TestData table
      		JsonArray array = TableUtil.testValuesToJsonArray(testData.toArray(new TestValue[0]));
      		JsonObject object = new JsonObject();
      		// Save the test distance
      		object.addProperty(EcuEnum.TEST_DISTANCE.get(), testDistanceText.getValue());
      		array.add(object);
      		// Save the log folder location
      		if(logFolder != null) {
      			object = new JsonObject();
      			object.addProperty(Constants.TEST_LOG_FOLDER_KEY, logFolder.getAbsolutePath());
      			array.add(object);
      		}
      		return array;
      	}
      }


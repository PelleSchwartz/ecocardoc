package com.dtucar.ecucp.controller;

import com.dtucar.ecucp.communication.TeensySync;
import com.dtucar.ecucp.util.*;
import com.dtucar.ecucp.communication.TeensySync.EcuEnum;
import com.fazecast.jSerialComm.SerialPort;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.fxml.FXML;
import javafx.scene.Cursor;
import javafx.scene.control.*;
import javafx.scene.input.Clipboard;
import javafx.scene.input.ClipboardContent;
import javafx.scene.input.MouseButton;
import javafx.scene.layout.GridPane;
import javafx.scene.text.Text;
import org.controlsfx.control.ToggleSwitch;

import java.text.SimpleDateFormat;
import java.util.*;

/**
 * The <code>LeftController</code> is the code behind the <code>LeftView</code>.<br>
 * This view is the left column that is always visible, showing connection controls and status, and the app event log<br>
 */
public class LeftController extends BaseController {
	private static final String TAG = "LeftController";
	private static final int SERIAL_REFRESH_PERIOD = 1000;    // [ms]

	/**
	 * The ChoiceBox with a dropdown menu showing all available serial (COM) ports
	 */
	@FXML
	private ChoiceBox usbSerialChoice;
	/**
	 * The ToggleSwitch to establish or close a connection to the selected serial (COM) port
	 */
	@FXML
	private ToggleSwitch usbSerialToggle;
	/**
	 * The TextArea containing the app event log
	 */
	@FXML
	private TextArea appLogText;
	/**
	 * The pane containing the {@link #usbSerialChoice} and the {@link #usbSerialToggle}
	 */
	@FXML
	private GridPane serialPane;
	/**
	 * The Text that shows the last time an update from the ECU was received (or "Disconnected")
	 */
	@FXML
	private Text lastUpdateTimeText;

	/**
	 * The last time (in System Millis) an update from the ECU was received
	 */
	private long lastSerialRefreshTime = 0;
	/**
	 * The maximum time delay (in ms) between responses from the ECU (used to debug latency issues)
	 */
	private long maxPingPeriod = 0;
	/**
	 * The last time (in System Millis) a response from the ECU was received
	 */
	private long lastPingTime = 0;
	/**
	 * The tooltip above the {@link #lastUpdateTimeText} which shows the response delay
	 */
	private Tooltip lastUpdateTimeTooltip;
	/**
	 * Whether or not the app event log is paused
	 */
	private boolean isAppLogPaused = false;
	/**
	 * The timer that is responsible for calling {@link TeensySync#loop()} every {@link Constants#TEENSY_SYNC_PERIOD}
	 */
	private Timer teensySyncTimer;

	/**
	 * The initialization method runs when the View is added to a Screen but before it is shown (on application start)
	 */
	@FXML
	public void initialize() {
		// Show available serial (COM) ports
		populateUsbSerialChoice();
		// Add a listener to the ToggleSwitch
		usbSerialToggle.selectedProperty().addListener((observable, oldValue, newValue) -> {
			if(oldValue == newValue) return;
			// Set the toggle as disabled (while we are connecting/disconnecting)
			usbSerialToggle.setDisable(true);
			// Set the cursor to the waiting ring (while we are connecting/disconnecting)
			Locator.getMainApp().getStage().getScene().setCursor(Cursor.WAIT);
			// React on the command
			if(newValue) {
				// Connect: We want to establish the connection
				// Get the serial (COM) port we want to connect to
				final String port = (String) usbSerialChoice.getValue();
				AppLog.log("Connecting to " + port + "...");
				// Clear the buffers
				TeensySync.clearBuffers();
				maxPingPeriod = 0;
				// Connect the serial
				Locator.getCommunication().connectSerial(port, result -> Platform.runLater(() -> {
					// After the connection attempt, update the UI
					// Set the cursor back to default
					Locator.getMainApp().getStage().getScene().setCursor(Cursor.DEFAULT);
					// Re-enable the toggle switch
					usbSerialToggle.setDisable(false);
					// Check whether the connection was a success
					boolean wasConnectionAttemptSuccessful = Boolean.parseBoolean(result);
					if(wasConnectionAttemptSuccessful) {
						// Connection success
						// Set the serial pane background to green
						serialPane.setStyle("-fx-background-color: #00c853;");
						// Log the event and reset the update time style
						AppLog.log("Connected to " + port);
						lastUpdateTimeText.setStyle("");
						// Clear all charts
						Charts.clearAllCharts();
						// Timer for TeensySync loop
						if(teensySyncTimer != null) teensySyncTimer.cancel();
						teensySyncTimer = new Timer(true);
						TimerTask timerTask = new TimerTask() {
							@Override
							public void run() {
								// If the main app is alive (we haven't closed the program), run the TeensySync loop
								if(Locator.getMainApp() != null && Locator.getMainApp().isAlive()) TeensySync.loop();
							}
						};
						teensySyncTimer.scheduleAtFixedRate(timerTask, Constants.TEENSY_SYNC_PERIOD, Constants.TEENSY_SYNC_PERIOD);
						// Notify MainController
						Locator.getMainController().onConnect();
					} else {
						// Connection failure
						// Log the event and indicate that no connection is established
						AppLog.log("Failed to open port " + port);
						usbSerialToggle.setSelected(false);
					}
				}));
				usbSerialChoice.setDisable(true);
			} else {
				// Disconnect: We want to close the connection
				Locator.getCommunication().disconnect();
				// Re-enable the dropdown menu and the toggle switch
				usbSerialChoice.setDisable(false);
				usbSerialToggle.setDisable(false);
				// Set the cursor back to default
				Locator.getMainApp().getStage().getScene().setCursor(Cursor.DEFAULT);
				// Set the serial pane color back to (almost) white
				serialPane.setStyle("-fx-background-color: #f4f4f4;");
				// Show the disconnected state in the last update text and the app log
				lastUpdateTimeText.setText("Disconnected");
				AppLog.log("Disconnected");
			}
		});
		// Add a context menu to the app log
		ContextMenu contextMenu = new ContextMenu();
		// Provide menu to pause and resume the log
		MenuItem pauseItem = new MenuItem("Pause");
		pauseItem.setOnAction(event -> {
			isAppLogPaused = !isAppLogPaused;
			if(isAppLogPaused) pauseItem.setText("Resume");
			else pauseItem.setText("Pause");
		});
		// Provide menu to clear the log
		MenuItem clearItem = new MenuItem("Clear");
		clearItem.setOnAction(event -> appLogText.clear());
		// Provide menu to copy the log content
		MenuItem copyItem = new MenuItem("Copy all");
		//noinspection Duplicates
		copyItem.setOnAction(event -> {
			final Clipboard clipboard = Clipboard.getSystemClipboard();
			final ClipboardContent content = new ClipboardContent();
			content.putString(appLogText.getText());
			clipboard.setContent(content);
		});
		contextMenu.getItems().addAll(pauseItem, clearItem, new SeparatorMenuItem(), copyItem);
		appLogText.setContextMenu(contextMenu);
		// Tooltip on update text to show update delay time (latency)
		lastUpdateTimeTooltip = new Tooltip();
		lastUpdateTimeTooltip.setAutoHide(true);
		Tooltip.install(lastUpdateTimeText, lastUpdateTimeTooltip);
		// Toggle the tooltip on click and reset the maximum ping period on double-click
		lastUpdateTimeText.setOnMouseClicked(event -> {
			if(!lastUpdateTimeTooltip.isShowing() && event.getButton() == MouseButton.SECONDARY) lastUpdateTimeTooltip.show(Locator.getMainApp().getStage(), event.getScreenX(), event.getScreenY());
			else lastUpdateTimeTooltip.hide();
			if(event.getClickCount() == 2) maxPingPeriod = 0;
		});
		// Register an app log listener
		AppLog.registerListener(message -> {
			// Ignore when paused
			if(isAppLogPaused) {
				return;
			}
			Platform.runLater(() -> {
				// Trim the app log if it is too long (which will cause the entire UI to slow down)
				if(appLogText.getText().length() > Constants.MAX_TEXT_IN_TEXT_AREA) {
					appLogText.deleteText(0, message.length());
				}
				// Append the new log line
				appLogText.appendText(message + "\n");
			});
		});
	}

	/**
	 * Populate the dropdown choice box and show the available serial (COM) ports
	 */
	@SuppressWarnings("unchecked")
	@FXML
	private void populateUsbSerialChoice() {
		// Get the available serial ports
		SerialPort[] commPorts = SerialPort.getCommPorts();
		if(commPorts.length == 0) {
			// No ports were found
			usbSerialChoice.setItems(FXCollections.observableArrayList(Locator.getResources().getString("no_ports")));
			usbSerialChoice.setValue(Locator.getResources().getString("no_ports"));
			usbSerialToggle.setDisable(true);
		} else {
			// A (or several) serial port was found
			ArrayList usbSerialChoiceList = new ArrayList(commPorts.length);
			for(SerialPort serialPort : commPorts) {
				String portName = serialPort.getSystemPortName();
				usbSerialChoiceList.add(portName);
			}
			// Show the available ports
			usbSerialChoice.setItems(FXCollections.observableArrayList(usbSerialChoiceList));
			usbSerialChoice.setValue(usbSerialChoiceList.get(0));
			usbSerialToggle.setDisable(false);
		}
	}

	/**
	 * This method is called when the stop button is pressed
	 */
	public void onStopButtonPressed() {
		// Stop all communication ("emergency stop" which also halts any burn or idle) and disconnect from the ECU
		TeensySync.stop();
		// Set the cursor to the waiting (loading) ring
		Platform.runLater(() -> Locator.getMainApp().getStage().getScene().setCursor(Cursor.WAIT));
		// Log the event
		AppLog.log("STOP pressed");
		// Schedule a delayed task (which runs once)
		new Timer(true).schedule(
				new TimerTask() {
					@Override
					public void run() {
						Platform.runLater(() -> {
							// Indicate that the connection is now closed
							if(usbSerialToggle != null) usbSerialToggle.selectedProperty().setValue(false);
							// Set the cursor back to the default one
							Locator.getMainApp().getStage().getScene().setCursor(Cursor.DEFAULT);
						});
						this.cancel();
					}
				},
				500
		);
	}

	/**
	 * The loop that runs every {@link Constants#BASE_LOOP_PERIOD}
	 */
	@Override
	protected void loop() {
		// If we're not connected, refresh the dropdown menu with the available serial (COM) ports every SERIAL_REFRESH_PERIOD
		if(!Locator.getCommunication().isConnected() && System.currentTimeMillis() - lastSerialRefreshTime > SERIAL_REFRESH_PERIOD) {
			lastSerialRefreshTime = System.currentTimeMillis();
			// Only refresh the dropdown while it is not showing
			if(!usbSerialChoice.isShowing()) {
				Platform.runLater(() -> {
					// Populate the dropdown menu
					populateUsbSerialChoice();
					// If the cursor is not waiting (we're not in the process of connecting)
					if(Locator.getMainApp().getStage().getScene().getCursor() != Cursor.WAIT) {
						// Show the disconnected state
						usbSerialChoice.setDisable(false);
						usbSerialToggle.setDisable(false);
						usbSerialToggle.setSelected(false);
						serialPane.setStyle("-fx-background-color: #f4f4f4;");
						lastUpdateTimeText.setText("Disconnected");
					}
				});
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
	 * Called when new data is received to update the last update time and the ping time
	 *
	 * @param data the hash map of the new data received
	 */
	@Override
	public void onDataReceived(HashMap<String, Object> data) {
		Platform.runLater(() -> {
			// The source key is always sent
			if(data.containsKey(Constants.SOURCE_KEY)) {
				// If we're not receiving from a save file (we're receiving from the ECU)
				if(!data.get(Constants.SOURCE_KEY).equals(Constants.SOURCE_SAVE_AND_LOAD)) {
					// Show the last updated time
					String timeStamp = new SimpleDateFormat("HH:mm:ss.SSS").format(Calendar.getInstance().getTime());
					lastUpdateTimeText.setText("Received: " + timeStamp);
					// Calculate the ping (response latency) and the maximum (worst) ping since last reset
					long currentPingPeriod = System.currentTimeMillis() - lastPingTime;
					if(lastPingTime != 0 && currentPingPeriod > maxPingPeriod) maxPingPeriod = currentPingPeriod;
					lastPingTime = System.currentTimeMillis();
					lastUpdateTimeTooltip.setText("Current refresh delay: " + currentPingPeriod + " ms\nMax refresh delay: " + maxPingPeriod + " ms");
				}
			}
			// The state key is always sent
			if(data.containsKey(EcuEnum.STATE.get())) {
				// Get the state
				int ecuState = ((Number) data.get(EcuEnum.STATE.get())).intValue();
				// If the ECU is not sending the alive state, the emergency stop must have been pressed
				if((ecuState & Constants.STATE_ECU_ALIVE_MASK) != 1) {
					// Show the emergency stop state
					lastUpdateTimeText.setStyle("-fx-fill: RED");
					lastUpdateTimeText.setText("Emergency stop pressed!");
				} else {
					// Reset to default (non-emergency stop) state
					lastUpdateTimeText.setStyle("");
				}
			}
		});
	}
}

package com.dtucar.ecucp.controller;

import com.dtucar.ecucp.communication.MessageReceivedListener;
import com.dtucar.ecucp.communication.TeensySync;
import com.dtucar.ecucp.model.ConfigValue;
import com.dtucar.ecucp.util.*;
import com.dtucar.ecucp.communication.TeensySync.EcuEnum;
import com.dtucar.ecucp.view.ConfigTextField;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.geometry.Insets;
import javafx.scene.Cursor;
import javafx.scene.control.*;
import javafx.scene.input.Clipboard;
import javafx.scene.input.ClipboardContent;
import javafx.scene.input.KeyCode;
import javafx.scene.layout.GridPane;
import javafx.stage.FileChooser;
import javafx.stage.Modality;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.*;
import java.util.stream.Collectors;

/**
 * The <code>LogController</code> is the code behind the <code>LogView</code>.<br>
 * This view loads the log file from the Teensy and displays it.
 */
public class LogController extends BaseController implements MessageReceivedListener {
	private static final String TAG = "LogController";

	/**
	 * The ListView that shows the ObservableList of the log file content (a ListView is more efficient for a really long text that a simple Text view)
	 */
	@FXML
	private ListView<String> logText;
	/**
	 * The ObservableList that contains the strings of the log file content
	 */
	private final ObservableList<String> logTextContent = FXCollections.observableArrayList();
	/**
	 * The Dialog that allows configuration of the logging period and setting the time and date
	 */
	private Dialog<HashMap<String, Object>> configureDialog;
	/**
	 * The ComboBox inside the {@link #configureDialog} that is simply used as a stylized view (the ComboBox function is not actually used)
	 */
	private ComboBox datePicker;
	/**
	 * The ConfigTextField where the log period can be entered (the period in ms by which the ECU logs to the SD card)
	 */
	private ConfigTextField loggingPeriod;
	/**
	 * The content of the {@link #loggingPeriod} text field (used when fetching the log period setting from the ECU)
	 */
	private String logPeriod;
	/**
	 * The time as received from the Teensy (used when fetching the time setting from the ECU)
	 */
	private long teensyTime = 0;
	/**
	 * Whether a response was received since the last communication request
	 */
	private boolean lastRequestAnswered = true;
	/**
	 * Whether a transfer of the log file is currently in progress
	 */
	private boolean gettingLogFileInProgress = false;
	/**
	 * The time in System Millis when the log transfer was started
	 */
	private long logFileTransferStartedMillis;
	/**
	 * A temporary buffer for incomplete log lines received from the ECU (since the log is transferred from the ECU in packets that can be cut off)
	 */
	private String savedLine = "";

	/**
	 * The initialization method runs when the View is added to a Screen but before it is shown (on application start)
	 */
	@FXML
	public void initialize() {
		// Set up the logText ListView, so it acts similar to a TextView (allows selection and copying)
		// Allow multiple selection
		logText.getSelectionModel().setSelectionMode(SelectionMode.MULTIPLE);
		// Add context menu for copy
		ContextMenu contextMenu = new ContextMenu();
		MenuItem copyItem = new MenuItem("Copy");
		copyItem.setOnAction(event -> {
			// Add the selected items to the clipboard separated by a new line character
			final Clipboard clipboard = Clipboard.getSystemClipboard();
			final ClipboardContent content = new ClipboardContent();
			ObservableList<String> selectedItems = logText.getSelectionModel().getSelectedItems();
			content.putString(selectedItems.stream().map(Object::toString).collect(Collectors.joining("\n")));
			clipboard.setContent(content);
		});
		// Add context menu for copy all
		MenuItem copyAllItem = new MenuItem("Copy all");
		copyAllItem.setOnAction(event -> {
			// Add all items to the clipboard separated by a new line character
			final Clipboard clipboard = Clipboard.getSystemClipboard();
			final ClipboardContent content = new ClipboardContent();
			content.putString(logTextContent.stream().map(Object::toString).collect(Collectors.joining("\n")));
			clipboard.setContent(content);
		});
		contextMenu.getItems().addAll(copyItem, copyAllItem);
		logText.setContextMenu(contextMenu);
		// Handle Ctrl+C shortcut for copying
		logText.setOnKeyPressed(event -> {
			if(event.isControlDown() && event.getCode() == KeyCode.C) copyItem.getOnAction().handle(null);
		});
		// Bind to the data object
		logText.setItems(logTextContent);
	}

	/**
	 * This function is called from {@link MainController} when the associated tab or page is shown to the user
	 */
	@Override
	public void show() {
		super.show();
		createConfigureDialog();
		TeensySync.requestConfig(EcuEnum.TIME, EcuEnum.LOG_PERIOD);
	}

	/**
	 * This function is called from {@link MainController} when the associated tab or page is hidden from the user
	 */
	@Override
	public void hide() {
		super.hide();
		Locator.getMainApp().getStage().getScene().setCursor(Cursor.DEFAULT);
	}

	/**
	 * The loop that runs every {@link Constants#BASE_LOOP_PERIOD}
	 */
	@Override
	protected void loop() {
		if(!Locator.getCommunication().isConnected()) {
			lastRequestAnswered = true;
		} else if(configureDialog != null && configureDialog.isShowing() && !datePicker.isDisabled() && lastRequestAnswered) {
			lastRequestAnswered = false;
			TeensySync.requestConfig(EcuEnum.TIME);
		}
	}

	/**
	 * Create and style the configure dialog that allows configuration of the logging period and setting the time and date
	 */
	private void createConfigureDialog() {
		// Create the custom configureDialog
		if(configureDialog == null) {
			// Style the Dialog
			configureDialog = new Dialog<>();
			configureDialog.setTitle("Configure logging");
			configureDialog.initOwner(Locator.getMainApp().getStage());
			configureDialog.initModality(Modality.NONE);
			configureDialog.setHeaderText(null);
			configureDialog.setGraphic(null);
			configureDialog.getDialogPane().getStylesheets().add(Locator.getMainApp().getClass().getResource("view/RefreshComboBox.css").toString());
			configureDialog.getDialogPane().getButtonTypes().addAll(ButtonType.OK, ButtonType.CANCEL);
			// Add the ConfigTextField
			GridPane grid = new GridPane();
			grid.setHgap(10);
			grid.setVgap(10);
			grid.setPadding(new Insets(20, 10, 10, 10));
			loggingPeriod = new ConfigTextField();
			loggingPeriod.setPromptText("Auto");
			if(loggingPeriod.getValue() != null) loggingPeriod.setOnUpdateListener(event -> TeensySync.sendConfig(new ConfigValue(EcuEnum.LOG_PERIOD, loggingPeriod.getValue())));
			loggingPeriod.setTooltip(new Tooltip(Locator.getResources().getString("log_period_tooltip")));
			if(logPeriod != null && !logPeriod.isEmpty()) loggingPeriod.setTextConfirmed(logPeriod, true);
			// Add the "fake" DatePicker (that only allows for a sync button, but no actual text entry)
			datePicker = new ComboBox();
			datePicker.setPromptText("Not set");
			datePicker.setEditable(true);
			datePicker.getEditor().setEditable(false);
			datePicker.setTooltip(new Tooltip(Locator.getResources().getString("time_sync_tooltip")));
			if(teensyTime > 0) {
				LocalDateTime localDateTime = LocalDateTime.ofInstant(Instant.ofEpochMilli(teensyTime), ZoneId.systemDefault());
				DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
				String timeAndDateString = localDateTime.format(formatter);
				datePicker.getEditor().setText(timeAndDateString);
			}
			datePicker.showingProperty().addListener((obs, t, t1) -> {
				if(t1) {
					// The field is hacked from a date picker which we don't use, so hide it
					datePicker.hide();
					// If not connected, return
					if(!Locator.getCommunication().isConnected()) {
						datePicker.getEditor().setText("Can't send time (not connected)");
						return;
					}
					// A float cannot store the precise value of unix seconds, so some trickery is needed, because all our data transmission uses float
					// This is the correct value in int
					int unixSeconds = (int) (System.currentTimeMillis() / 1000);
					// This is the truncated version in float
					float unixSecondsFloat = (float) unixSeconds;
					// Round up the truncated version
					for(int i = 0; (int) unixSecondsFloat < unixSeconds; i++) {
						unixSecondsFloat = (float) (unixSeconds + i);
					}
					// Calculate the time until the float value is correct
					final int secondsUntilSyncF = ((int) unixSecondsFloat - unixSeconds);
					final float unixSecondsFloatF = unixSecondsFloat;
					datePicker.setDisable(true);
					if(Constants.DEBUG) System.out.println("Syncing " + unixSeconds + "s to (float) " + (int) unixSecondsFloat + "s in " + secondsUntilSyncF + "s");
					AppLog.log("Initiated RTC time sync...");
					// Schedule a timer to send the update when the float is correct (matching the int value)
					new Timer(true).schedule(
							new TimerTask() {
								int secondsUntilSync = secondsUntilSyncF;

								@Override
								public void run() {
									secondsUntilSync--;
									if(secondsUntilSync == 0) {
										TeensySync.sendConfig(new ConfigValue(EcuEnum.TIME, (int) unixSecondsFloatF));
										datePicker.setDisable(false);
										AppLog.log("RTC time sync complete");
										this.cancel();
									}
									Platform.runLater(() -> datePicker.getEditor().setText("Time sync in " + secondsUntilSync + "s"));
								}
							},
							0, 1000
					);
				}
			});
			// Add the elements to the grid and the dialog
			grid.add(new Label("Logging period"), 0, 1);
			grid.add(loggingPeriod, 1, 1);
			grid.add(new Label("Set time"), 0, 2);
			grid.add(datePicker, 1, 2);
			configureDialog.getDialogPane().setContent(grid);
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
	 * Called when new data is received to display it on the UI
	 *
	 * @param data the hash map of the new data received
	 */
	@Override
	public void onDataReceived(HashMap<String, Object> data) {
		Platform.runLater(() -> {
			// A log key was received
			if(data.containsKey(EcuEnum.LOG.get())) {
				Object logObject = data.get(EcuEnum.LOG.get());
				//noinspection unchecked
				List<String> list = (List<String>) logObject;
				// Scroll to the end
				logText.scrollTo(logTextContent.size());
				if(list.stream().anyMatch(s -> s.startsWith("##### LOG START #####"))) {
					// A log transfer is starting
					// Set the waiting cursor, add a message listener (for the raw, un-parsed messages) and disable parsing (to improve performance)
					Locator.getMainApp().getStage().getScene().setCursor(Cursor.WAIT);
					Locator.registerMessageListener(this);
					Locator.getCommunication().setDisableParsing(true);
					logTextContent.add("##### LOG START #####");
					logFileTransferStartedMillis = System.currentTimeMillis();
					AppLog.log("Fetching log...");
				} else if(list.stream().anyMatch(s -> s.startsWith("#####"))) {
					// This one handles when ECU responds with "##### NO SD CARD #####" or other message prefixed with "#####" indicating to further log transmission
					// Set the default cursor, remove the message listener, and re-enable parsing
					Locator.getMainApp().getStage().getScene().setCursor(Cursor.DEFAULT);
					Locator.unregisterMessageListener(this);
					Locator.getCommunication().setDisableParsing(false);
					gettingLogFileInProgress = false;
				}
			}
			// The log period setting was received (parse it and display it)
			if(data.containsKey(EcuEnum.LOG_PERIOD.get())) {
				logPeriod = String.valueOf(data.get(EcuEnum.LOG_PERIOD.get()));
				if(configureDialog != null) loggingPeriod.setTextConfirmed(logPeriod, true);
			}
			// The Teensy time setting was received (parse it and display it)
			if(data.containsKey(EcuEnum.TIME.get())) {
				long teensyTime = ((Number) data.get(EcuEnum.TIME.get())).longValue();
				if(teensyTime > 0) {
					this.teensyTime = teensyTime;
					// If the configure dialog has been initialized, is showing and the date picker is not disabled (that means we're not in the process of setting the time)
					if(configureDialog != null && configureDialog.isShowing() && datePicker != null && !datePicker.isDisabled()) {
						// Parse the time and date and display it
						LocalDateTime localDateTime = LocalDateTime.ofInstant(Instant.ofEpochMilli(teensyTime * 1000), ZoneId.systemDefault());
						DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
						String timeAndDateString = localDateTime.format(formatter);
						datePicker.getEditor().setText(timeAndDateString);
					}
				}
			}
			lastRequestAnswered = true;
		});
	}

	/**
	 * Will be called to notify of a new (not parsed) message.
	 *
	 * @param message the string received
	 */
	@Override
	public void onMessageReceived(String message) {
		// If a log file transfer is in progress, parse the message
		if(gettingLogFileInProgress) {
			Platform.runLater(() -> {
				// When the entire file has been received, put stuff back in place
				if(message.contains("##### LOG END #####")) {
					// Add the log end message, scroll to the end, set the default cursor, unregister the message listener and re-enable parsing: The transfer is finished
					logTextContent.addAll(message);
					logText.scrollTo(logTextContent.size());
					Locator.getMainApp().getStage().getScene().setCursor(Cursor.DEFAULT);
					Locator.unregisterMessageListener(this);
					Locator.getCommunication().setDisableParsing(false);
					gettingLogFileInProgress = false;
					AppLog.log("Log file (" + logTextContent.stream().mapToInt(String::length).sum() / 1000 + " KB)\nfetched in " + (System.currentTimeMillis() - logFileTransferStartedMillis) + " ms");
					return;
				}
				// Prepend any saved (incomplete) line from the previous received message
				String messageWithSaved = savedLine + message;
				// Split the message on each new line character
				String[] lines = messageWithSaved.split("(?=\n)");
				if(lines.length > 1) {
					// Add all the the lines except the last one
					List<String> lineList = Arrays.asList(lines).subList(0, lines.length - 1);
					// Only add the line if it is not empty
					lineList = lineList.stream().filter(s -> !s.isEmpty()).collect(Collectors.toList());
					// Replace all the new lines (since we're adding it to a ListView where each line is displayed on its own line anyway)
					lineList.replaceAll(s -> s.replace("\n", ""));
					logTextContent.addAll(lineList);
				}
				// Add the last line if it's complete or save it for next package
				String lastLine = lines[lines.length - 1];
				if(lastLine.endsWith("\n") && lastLine.length() > 5) {
					logTextContent.add(lastLine.replace("\n", ""));
					savedLine = "";
				} else {
					savedLine = lastLine;
				}
				// Scroll to the end
				logText.scrollTo(logTextContent.size());
			});
		}
	}

	/**
	 * Called when the Configure button is pressed (opens the dialog)
	 */
	@FXML
	private void onConfigure() {
		if(configureDialog != null) configureDialog.showAndWait();
	}

	/**
	 * Called when the Save button is pressed (to save the log content to a text file)
	 */
	@FXML
	private void onSave() {
		try {
			// Open a file chooser to pick the save location
			FileChooser fileChooser = new FileChooser();
			fileChooser.setTitle("Save File");
			DateTimeFormatter dtf = DateTimeFormatter.ofPattern("yyyy-MM-dd HH-mm");
			LocalDateTime now = LocalDateTime.now();
			String time = dtf.format(now);
			fileChooser.setInitialFileName(time + " " + Locator.getResources().getString("suggested_log_file_name") + (gettingLogFileInProgress ? " - INCOMPLETE" : ""));
			fileChooser.setInitialDirectory(new File("."));
			fileChooser.getExtensionFilters().addAll(new FileChooser.ExtensionFilter("Comma Separated file (CSV)", "*.csv"),
					new FileChooser.ExtensionFilter("Text file (TXT)", "*.txt"));
			File file = fileChooser.showSaveDialog(Locator.getMainApp().getStage());
			// If a file has been chosen, save the log content
			if(file != null) {
				// Set the waiting cursor
				Platform.runLater(() -> Locator.getMainApp().getStage().getScene().setCursor(Cursor.WAIT));
				// Save through a stream writer
				OutputStreamWriter writer = new OutputStreamWriter(new FileOutputStream(file), "UTF-8");
				String absolutePath = file.getAbsolutePath();
				int fileNumber = 1;
				// Write the content of logTextContent to a file
				for(int i = 1; i < logTextContent.size() - 1; i++) {
					String logString = logTextContent.get(i) + '\n';
					// Don't save lines starting with "#####" (such as log start and log end lines)
					if(logString.startsWith("#####")) continue;
					// If the Teensy has restarted, it writes the Header again
					if(logString.startsWith("OnTime") && i > 1) {
						// In this case, close this file
						writer.flush();
						writer.close();
						AppLog.log("Log saved to: " + file.getName());
						// And create a new one with " - 2" or " - 3" etc. appended
						String filePath = StringUtil.replaceLast(absolutePath, ".", " - " + fileNumber++ + ".");
						file = new File(filePath);
						writer = new OutputStreamWriter(new FileOutputStream(file), "UTF-8");
					}
					// Write line and replace all newline characters with the system default (for Windows, this is \r\n)
					writer.write(logString.replace("\n", System.lineSeparator()));
				}
				// Close the file
				writer.flush();
				writer.close();
				AppLog.log("Log saved to: " + file.getName());
				Platform.runLater(() -> Locator.getMainApp().getStage().getScene().setCursor(Cursor.DEFAULT));
			}
		} catch(IOException e) {
			e.printStackTrace();
			AppLog.error("An error occurred during save log file: " + e.getMessage());
			DialogUtil.showExceptionDialog(e);
		}
	}

	/**
	 * Called when the Reset button is pressed (delete the log)
	 */
	@FXML
	private void onReset() {
		// Open a confirmation alert
		Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
		alert.initOwner(Locator.getMainApp().getStage());
		alert.setTitle("Confirm reset");
		alert.setHeaderText(null);
		alert.setContentText("Are you sure you want to reset and delete the log?\nThis will delete the log file on the SD card.");
		Optional<ButtonType> result = alert.showAndWait();
		// Reset the log both on-screen and on the Teensy when OK is pressed
		if(result.isPresent() && result.get() == ButtonType.OK) {
			TeensySync.sendConfig(new ConfigValue(EcuEnum.RESET_LOG, 1));
			logTextContent.clear();
		}
	}

	/**
	 * Called when the Get button is pressed (request a log file transfer)
	 */
	@FXML
	private void onGet() {
		if(lastRequestAnswered) {
			lastRequestAnswered = false;
			logTextContent.clear();
			gettingLogFileInProgress = true;
			TeensySync.requestConfig(EcuEnum.TIME, EcuEnum.LOG_PERIOD, EcuEnum.LOG);
		}
	}

	/**
	 * Called from the {@link MainController#onConnect()} when a connection has been successfully established
	 */
	public void onConnect() {
		TeensySync.requestConfig(EcuEnum.TIME, EcuEnum.LOG_PERIOD);
	}
}

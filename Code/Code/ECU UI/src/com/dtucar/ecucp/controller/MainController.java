package com.dtucar.ecucp.controller;

import com.dtucar.ecucp.Main;
import com.dtucar.ecucp.communication.TeensySync;
import com.dtucar.ecucp.model.ConfigValue;
import com.dtucar.ecucp.util.*;
import javafx.application.Platform;
import javafx.event.Event;
import javafx.fxml.FXML;
import javafx.scene.Cursor;
import javafx.scene.control.*;
import javafx.scene.image.ImageView;
import javafx.scene.input.KeyCode;
import javafx.stage.Stage;

import java.io.File;
import java.util.HashMap;
import java.util.Timer;
import java.util.TimerTask;

/**
 * The <code>MainController</code> is the code behind the <code>MainView</code>.<br>
 * This view is the first one that gets loaded by {@link Main}.
 * It contains the references to the Menu and all the other views (Tabs and LeftView) of the application.
 */
public class MainController {
	private static final String TAG = "MainController";

	/**
	 * The TabPane contains all the tabs of the application (each tab has its own Controller)
	 */
	@FXML
	private TabPane tabPane;
	/**
	 * The LeftController controls the LeftView: the left column that is always visible
	 */
	@FXML
	private LeftController leftController;

	@FXML
	private CockpitController cockpitController;
	@FXML
	private ConfigurationController configurationController;
	@FXML
	private ChartsController chartsController;
	@FXML
	private DebugController debugController;
	@FXML
	private LogController logController;
	@FXML
	private CanController canController;
	@FXML
	private Rs232Controller rs232Controller;
	@FXML
	private TestController testController;
	@FXML
	private EncoderController encoderController;

	/**
	 * The tab of the ChartsView (used to attach the right-click pause menu)
	 */
	@FXML
	private Tab chartsTab;

	/**
	 * The MenuItem of the right-click pause menu for the {@link #setPausedCharts(boolean)} function
	 */
	private MenuItem pauseItem;
	/**
	 * Whether or not a party is in progress
	 */
	private boolean party = false;

	/**
	 * The initialization method runs when the View is added to a Screen but before it is shown (on application start)
	 */
	@FXML
	public void initialize() {
		// Set the Locator MainController reference to this instance
		Locator.setMainController(this);
		// Show (start) the LeftController
		leftController.show();
		// Show (start) the ChartsController (let it run in the background)
		chartsController.show();
		// Show (start) the ConfigurationController (let it fetch the settings once)
		configurationController.show();
		// Call show (start) on the first tab (normally the CockpitController)
		callShow(tabPane.getSelectionModel().getSelectedItem().getContent().getId());
		// Set a listener to act on tab changes
		tabPane.getSelectionModel().selectedItemProperty().addListener((ov, previousTab, newTab) -> {
			if(Constants.DEBUG) System.out.println(TAG + ": " + "Switched from " + previousTab.getContent().getId() + " to " + newTab.getContent().getId());
			// Call hide (pause) on the tab being hidden
			if(previousTab != null && previousTab.getContent() != null) {
				callHide(previousTab.getContent().getId());
			}
			// Call show (start/resume) on the tab being shown
			callShow(newTab.getContent().getId());
		});
		// Add the pause/resume charts menu to the charts tab
		ContextMenu contextMenu = new ContextMenu();
		pauseItem = new MenuItem("Pause");
		pauseItem.setOnAction(event -> setPausedCharts(!chartsController.getPaused()));
		contextMenu.getItems().add(pauseItem);
		chartsTab.setContextMenu(contextMenu);
		chartsTab.setOnSelectionChanged(event -> chartsController.setPaused(false));
		// Bind the ESC key to the emergency stop button
		Locator.getMainApp().getStage().setOnShown(windowEvent -> Locator.getMainApp().getStage().getScene().setOnKeyPressed(keyEvent -> {
			if(keyEvent.getCode() == KeyCode.ESCAPE) leftController.onStopButtonPressed();
		}));
	}

	/**
	 * Call the hide (pause) method when the associated tab or page is hidden from the user
	 *
	 * @param id the identification of the controller
	 */
	private void callHide(String id) {
		switch(id) {
			case "cockpit":
				cockpitController.hide();
				break;
			case "configuration":
				configurationController.hide();
				break;
			case "test":
				testController.hide();
				break;
			case "debug":
				debugController.hide();
				break;
			case "log":
				logController.hide();
				break;
			case "can":
				canController.hide();
				break;
			case "rs232":
				rs232Controller.hide();
				break;
			case "encoder":
				encoderController.hide();
				break;
			default:
				// Note: LeftController is always shown and ChartsController runs in the background
				break;
		}
	}

	/**
	 * Call the show (start/resume) method when the associated tab or page is shown to the user
	 *
	 * @param id the identification of the controller
	 */
	private void callShow(String id) {
		switch(id) {
			case "cockpit":
				cockpitController.show();
				break;
			case "configuration":
				configurationController.show();
				break;
			case "test":
				testController.show();
				break;
			case "debug":
				debugController.show();
				break;
			case "log":
				logController.show();
				break;
			case "can":
				canController.show();
				break;
			case "rs232":
				rs232Controller.show();
				break;
			case "encoder":
				encoderController.show();
				break;
			default:
				// Note: LeftController is always shown and ChartsController runs in the background
				break;
		}
	}

	/**
	 * Opens a dialog to display information about the application.
	 */
	@FXML
	private void handleAbout() {
		Alert alert = new Alert(Alert.AlertType.INFORMATION);
		alert.initOwner(Locator.getMainApp().getStage());
		alert.setTitle(Locator.getResources().getString("window_title"));
		alert.setHeaderText(Locator.getResources().getString("about_header"));
		alert.setContentText(Locator.getResources().getString("version_prefix") + " " + Locator.getResources().getString("version") + " " + Locator.getResources().getString("version_suffix") + "\n" + Locator.getResources().getString("about_text"));
		ImageView imageView = new ImageView(Locator.getMainApp().getClass().getResource("assets/logo64.png").toString());
		imageView.setCursor(Cursor.HAND);
		Tooltip partyTooltip = new Tooltip("Activate PARTY mode");
		partyTooltip.setAutoHide(true);
		Tooltip.install(imageView, partyTooltip);
		imageView.setOnMouseClicked(event -> {
			party = !party;
			if(party) {
				AppLog.log("PARTY started");
				TunesUtil.playSong(TunesUtil.Song.PARTY_SAX);
				partyTooltip.setText("Deactivate PARTY mode");
			} else {
				AppLog.log("PARTY stopped");
				TunesUtil.playSong(TunesUtil.Song.STOP);
				partyTooltip.setText("Activate PARTY mode");
			}
			if(Locator.getCommunication().isConnected()) TeensySync.sendConfig(new ConfigValue(TeensySync.EcuEnum.PARTY, party ? 1 : 0));
		});
		alert.setGraphic(imageView);
		alert.showAndWait();
	}

	/**
	 * Called from the File->Restart button causing the application to close and reopen
	 */
	@FXML
	private void onRestart() {
		AppLog.log("Restarting...");
		leftController.onStopButtonPressed();
		// To allow for the emergency stop to fully close down everything, the restart is scheduled with some delay
		new Timer(true).schedule(
				new TimerTask() {
					@Override
					public void run() {
						Platform.runLater(() -> {
							Locator.getMainApp().getStage().close();
							try {
								new Main().start(new Stage());
							} catch(Exception e) {
								e.printStackTrace();
							}
						});
					}
				},
				600
		);
	}

	/**
	 * Called from the File->Exit button causing the application to terminate immediately
	 */
	@FXML
	private void onExit() {
		Platform.exit();
	}

	/**
	 * Called from the File->Save button to save the configuration to an existing file (if save or load has not been called yet, this will prompt for source)
	 */
	@FXML
	private void save() {
		try {
			SaveAndLoad.saveAll(true, cockpitController, configurationController, testController);
		} catch(Exception e) {
			e.printStackTrace();
			AppLog.error("An error occurred during save configuration: " + e.getMessage());
			DialogUtil.showExceptionDialog(e);
		}
	}

	/**
	 * Called from the {@link TestController} when saving a configuration alongside a TestLog
	 *
	 * @param overrideFile the file to which the configuration will be saved
	 */
	public void save(File overrideFile) {
		try {
			SaveAndLoad.saveAll(overrideFile, false, cockpitController, configurationController, testController);
		} catch(Exception e) {
			e.printStackTrace();
			AppLog.error("An error occurred during save configuration: " + e.getMessage());
			DialogUtil.showExceptionDialog(e);
		}
	}

	/**
	 * Called from the File->Save configuration... button to save the configuration to a new file (prompts for destination)
	 */
	@FXML
	private void saveConfiguration() {
		try {
			SaveAndLoad.saveAll(false, cockpitController, configurationController, testController);
		} catch(Exception e) {
			e.printStackTrace();
			AppLog.error("An error occurred during save configuration: " + e.getMessage());
			DialogUtil.showExceptionDialog(e);
		}
	}

	/**
	 * Called from the File->Load configuration... button to load the configuration from a file (prompts for source)
	 */
	@FXML
	public void loadConfiguration() {
		try {
			HashMap<String, Object> hashMap = SaveAndLoad.loadAll();
			// The loaded value are sent to the controllers in their onDataReceived functions with the Source set to SOURCE_SAVE_AND_LOAD
			if(hashMap != null) {
				cockpitController.onDataReceived(hashMap);
				configurationController.onDataReceived(hashMap);
				testController.onDataReceived(hashMap);
				logController.onDataReceived(hashMap);
			}
		} catch(Exception e) {
			e.printStackTrace();
			AppLog.error("An error occurred during load configuration: " + e.getMessage());
			DialogUtil.showExceptionDialog(e);
		}
	}

	/**
	 * Called from {@link LeftController} when a connection was successfully established
	 */
	public void onConnect() {
		configurationController.onConnect();
		logController.onConnect();
	}

	/**
	 * Called when the Pause button in the right-click menu on the Charts tab is pressed (to pause background charts drawing)
	 *
	 * @param paused true if paused
	 */
	private void setPausedCharts(boolean paused) {
		chartsController.setPaused(paused);
		if(pauseItem != null) {
			if(chartsController.getPaused()) pauseItem.setText("Resume");
			else pauseItem.setText("Pause");
		}
	}

	/**
	 * Called when the DTU logo (in the top right corner) is pressed
	 *
	 * @param actionEvent event
	 */
	@FXML
	private void dtuLogoPressed(Event actionEvent) {
		// Close the menu that would otherwise pop-up
		if(actionEvent.getSource() instanceof Menu) ((Menu) actionEvent.getSource()).hide();
		// Toggle the party mode
		party = !party;
		if(party) TunesUtil.playSong(TunesUtil.Song.RICK);
		else TunesUtil.playSong(TunesUtil.Song.STOP);
	}
}

package com.dtucar.ecucp.controller;

import com.dtucar.ecucp.communication.TeensySync;
import com.dtucar.ecucp.util.*;
import com.dtucar.ecucp.communication.TeensySync.EcuEnum;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.TextArea;

import java.io.IOException;
import java.util.HashMap;

/**
 * The <code>CanController</code> is the code behind the <code>CanView</code>.<br>
 * This view shows debug information related to the CAN system in a plain TextArea, displaying a single formatted string sent from the ECU.
 */
public class CanController extends BaseController {
	private static final String TAG = "CanController";

	/**
	 * The TextArea in which the CAN debug information is shown as received from the ECU
	 */
	@FXML
	private TextArea canText;
	/**
	 * A CheckBox to toggle the automatic update of the debug information
	 */
	@FXML
	private CheckBox autoCheckbox;

	/**
	 * When the {@link #autoCheckbox} is checked, the ECU is polled for every AUTO_UPDATE_PERIOD for new CAN status
	 */
	private static final int AUTO_UPDATE_PERIOD = 500;    // [ms]
	/**
	 * Whether a response was received since the last communication request
	 */
	private boolean lastRequestAnswered = true;
	/**
	 * The time in millis at the last auto update
	 */
	private long lastAutoUpdateTime = 0;

	/**
	 * The loop that runs every {@link Constants#BASE_LOOP_PERIOD}
	 */
	@SuppressWarnings("Duplicates")
	@Override
	protected void loop() {
		if(!Locator.getCommunication().isConnected()) {
			// Do nothing if communication is not connected
			lastRequestAnswered = true;
		} else if(autoCheckbox.isSelected() && (System.currentTimeMillis() - lastAutoUpdateTime) > AUTO_UPDATE_PERIOD) {
			// If the auto update is checked, and it is time for an auto update, request a new status message
			lastAutoUpdateTime = System.currentTimeMillis();
			onGet();
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
	 * Called by Communication when new received data has been successfully parsed
	 *
	 * @param data the hash map of the new data received
	 */
	@Override
	public void onDataReceived(HashMap<String, Object> data) {
		Platform.runLater(() -> {
			// If the data contains a CAN status message
			if(data.containsKey(EcuEnum.CAN.get())) {
				// Save the current scroll position
				double scrollTop = canText.getScrollTop();
				// Replace the text area content with the new message
				canText.setText((String) data.get(EcuEnum.CAN.get()));
				// Restore the scroll position to the saved position
				canText.setScrollTop(scrollTop);
			}
			lastRequestAnswered = true;
		});
	}

	/**
	 * Save the content of the text area to a text file using {@link SaveAndLoad#saveStringToFile(String, String, String)}
	 */
	@FXML
	private void onSave() {
		try {
			SaveAndLoad.saveStringToFile(canText.getText(), Locator.getResources().getString("suggested_can_file_name"), "txt");
		} catch(IOException e) {
			e.printStackTrace();
			AppLog.error("An error occurred during save file: " + e.getMessage());
			DialogUtil.showExceptionDialog(e);
		}
	}

	/**
	 * Clear the TextArea
	 */
	@FXML
	private void onReset() {
		canText.clear();
	}

	/**
	 * Request a new CAN status message from the ECU
	 */
	@FXML
	private void onGet() {
		if(lastRequestAnswered) {
			lastRequestAnswered = false;
			TeensySync.requestConfig(EcuEnum.CAN);
		}
	}
}

package com.dtucar.ecucp.controller;

import com.dtucar.ecucp.util.Constants;
import com.dtucar.ecucp.util.Locator;
import com.dtucar.ecucp.communication.MessageReceivedListener;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.input.Clipboard;
import javafx.scene.input.ClipboardContent;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;

import java.util.*;

/**
 * The <code>DebugController</code> is the code behind the <code>DebugView</code>.<br>
 * This view allows it to send raw messages to the ECU and shows the raw messages as received from the ECU.
 */
public class DebugController extends BaseController implements MessageReceivedListener {
	private static final String TAG = "DebugController";

	/**
	 * The text area with the messages received and sent to the ECU
	 */
	@FXML
	private TextArea debugText;
	/**
	 * The text field from which the content is sent to t1he ECU
	 */
	@FXML
	private TextField debugTextField;
	/**
	 * The list/history of commands sent to the ECU (to ease sending of previously sent messages)
	 */
	private final List<String> commandList = new ArrayList<>();
	/**
	 * A pointer to the currently displayed index of the {@link #commandList} in the {@link #debugTextField}
	 */
	private int commandListIndex = 0;
	/**
	 * Whether or not to continue saving raw received messages in the {@link #debugText} when switched to another tab
	 */
	private boolean stayActiveInBackground = false;

	/**
	 * The initialization method runs when the View is added to a Screen but before it is shown (on application start)
	 */
	@FXML
	public void initialize() {
		ContextMenu contextMenu = new ContextMenu();
		// Add a context menu option to clear all content
		MenuItem clearItem = new MenuItem("Clear");
		clearItem.setOnAction(event -> onClear());
		// Add a context menu option to copy all content
		MenuItem copyItem = new MenuItem("Copy all");
		//noinspection Duplicates
		copyItem.setOnAction(event -> {
			final Clipboard clipboard = Clipboard.getSystemClipboard();
			final ClipboardContent content = new ClipboardContent();
			content.putString(debugText.getText());
			clipboard.setContent(content);
		});
		// Add a context menu option to toggle staying active in the background (when switching to another tab)
		MenuItem stayActiveItem = new MenuItem("Stay active");
		stayActiveItem.setOnAction(event -> {
			stayActiveInBackground = !stayActiveInBackground;
			if(stayActiveInBackground) stayActiveItem.setText("Do not stay active");
			else stayActiveItem.setText("Stay active");
		});
		contextMenu.getItems().addAll(stayActiveItem, new SeparatorMenuItem(), clearItem, new SeparatorMenuItem(), copyItem);
		debugText.setContextMenu(contextMenu);
		// When a keyboard event occurs in the text field
		debugTextField.addEventFilter(KeyEvent.ANY, event -> {
			if(event.getEventType() == KeyEvent.KEY_PRESSED) {
				// If pressing up, go backwards in the command history (up until the oldest command saved in the command history list)
				if(event.getCode() == KeyCode.UP) {
					if(commandListIndex >= 0 && commandListIndex < commandList.size()) {
						if(commandListIndex == commandList.size() - 1) commandList.add(debugTextField.getText());
						debugTextField.setText(commandList.get(commandListIndex));
						if(commandListIndex > 0) commandListIndex--;
					}
					// If pressing down, go forward in the command history (until the end, then clear it)
				} else if(event.getCode() == KeyCode.DOWN) {
					if(commandListIndex >= 0 && commandListIndex + 1 < commandList.size()) {
						commandListIndex++;
						debugTextField.setText(commandList.get(commandListIndex));
					} else if(commandListIndex + 1 != commandList.size() && !commandList.isEmpty()) {
						debugTextField.clear();
					}
				}
			}
		});
	}

	/**
	 * Called by Communication when new data has been received
	 *
	 * @param data the raw string received
	 */
	@Override
	public void onMessageReceived(String data) {
		Platform.runLater(() -> {
			// Delete older debug messages if the text area is over its limit (too much text leads to lag in the UI)
			while(debugText.getText().length() > Constants.MAX_TEXT_IN_TEXT_AREA) {
				debugText.deleteText(0, 1000);
			}
			// Append the new data with a new line and scroll to the bottom
			debugText.appendText(data);
			debugText.appendText("\n");
			debugText.setScrollTop(0);
		});
	}

	/**
	 * This function is called from {@link MainController} when the associated tab or page is shown to the user
	 */
	@Override
	public void show() {
		// Run the usual show code from BaseController
		super.show();
		// Register the listener for raw un-parsed messages from the Communication
		Locator.registerMessageListener(this);
	}

	/**
	 * This function is called from {@link MainController} when the associated tab or page is hidden from the user
	 */
	@Override
	public void hide() {
		// If the DebugController should not continue logging raw messages in the background
		if(!stayActiveInBackground) {
			// Run the usual hide code from BaseController
			super.hide();
			// Unregister the listener
			Locator.unregisterMessageListener(this);
		}
	}

	/**
	 * Loop unused
	 */
	@Override
	protected void loop() {
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
	 * Called by Communication when new received data has been successfully parsed (unused, since DebugController only receives raw un-parsed messages)
	 */
	@Override
	public void onDataReceived(HashMap<String, Object> data) {
	}

	/**
	 * Send the message in the text field
	 */
	private void sendMessage() {
		// If connected
		if(Locator.getCommunication().isConnected()) {
			// Get the text and send it
			String message = debugTextField.getText();
			Locator.getCommunication().send(message);
			// Add a new line if needed
			if(!debugText.getText().endsWith("\n")) {
				debugText.appendText("\n");
			}
			// Add it to the command history and show it on the text area
			commandList.add(message);
			commandListIndex = commandList.size() - 1;
			debugText.appendText("> Sent: " + message + "\n");
			debugTextField.clear();
		}
	}

	/**
	 * OnAction in the text field (enter key pressed)
	 */
	@FXML
	private void onTextField() {
		sendMessage();
	}

	/**
	 * The send button is pressed
	 */
	@FXML
	private void onSend() {
		sendMessage();
	}

	/**
	 * The clear button is pressed
	 */
	@FXML
	private void onClear() {
		debugText.clear();
	}

}

package com.dtucar.ecucp.view;

import com.dtucar.ecucp.util.OnUpdateListener;
import javafx.scene.control.TextField;

/**
 * The ConfigTextField is a custom control based on the default TextField but extended with the possibility to add change listeners based on value updates,
 * the possibility to change the background color (to distinguish confirmed and unconfirmed changes) and with prettier float formatting.
 */
public class ConfigTextField extends TextField {
	private OnUpdateListener onUpdateListener;
	private Float value;

	public ConfigTextField() {
		// OnAction (pressing enter in the text field) update the value
		setOnAction(event -> updateValue(true));
		// When the text is changed, change the background to yellow
		textProperty().addListener((obs, oldText, newText) -> setStyle("-fx-control-inner-background: #ffca28"));
		// When the text field loses focus, update the value
		focusedProperty().addListener((observable, oldValue, newValue) -> {
			if(oldValue) updateValue(true);
		});
	}

	/**
	 * Set the value of the text field programmatically (if confirmed is true, the listeners will not be notified of the change and the background will be changed to green)
	 *
	 * @param text      the text to put in the text field
	 * @param confirmed whether or not the value is confirmed
	 */
	public void setTextConfirmed(String text, boolean confirmed) {
		setText(text);
		updateValue(!confirmed);
		if(confirmed) setStyle("-fx-control-inner-background: #4caf50");
	}

	/**
	 * Update the value and optionally notify the update listeners
	 *
	 * @param notify - true when listeners should be notified
	 */
	private void updateValue(boolean notify) {
		// Parse the input
		Float newValue = parseInput(getText());
		// If it is invalid, clear the text field
		if(newValue == null) {
			value = null;
			setText("");
			return;
		}
		// If the value has changed
		if(!newValue.equals(value)) {
			value = newValue;
			// Set the text to the value on a white background
			setText(prettyPrintFloat(value));
			setStyle("-fx-control-inner-background: #ffffff");
			// Optionally notify the listeners
			if(onUpdateListener != null && notify) onUpdateListener.onUpdate(value);
		}
	}

	/**
	 * Utility function to prevent printing integers stored as float with decimals (e.g. 42.00)
	 *
	 * @param input the float value
	 * @return the string representation
	 */
	private String prettyPrintFloat(float input) {
		int i = (int) input;
		return input == i ? String.valueOf(i) : String.valueOf(input);
	}

	/**
	 * Parse the string input to a float (invalid values will be ignored)<br>.
	 * Both comma (,) and period (.) are interpreted as decimal separators.
	 *
	 * @param input the string input
	 * @return the float value
	 */
	private Float parseInput(String input) {
		input = input.trim();
		input = input.replaceAll(",", ".");
		Float value = null;
		try {
			value = Float.parseFloat(input);
		} catch(NumberFormatException ignored) {
		}
		return value;
	}

	/**
	 * Get the float value
	 *
	 * @return the float value
	 */
	public Float getValue() {
		return value;
	}

	/**
	 * The the listener to be notified when the value is updated
	 *
	 * @param onUpdateListener the listener
	 */
	public void setOnUpdateListener(OnUpdateListener onUpdateListener) {
		this.onUpdateListener = onUpdateListener;
	}
}


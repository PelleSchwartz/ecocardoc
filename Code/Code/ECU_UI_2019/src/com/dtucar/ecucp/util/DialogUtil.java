package com.dtucar.ecucp.util;

import javafx.application.Platform;
import javafx.scene.control.Alert;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Priority;
import javafx.stage.Modality;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.StringWriter;

/**
 * Class for various functions related to dialogs
 */
public class DialogUtil {
	/**
	 * Whether or not the exception dialog is showing
	 */
	private static boolean isExceptionDialogShowing = false;

	/**
	 * Show the exception dialog
	 *
	 * @param e the exception to show
	 */
	public static void showExceptionDialog(Throwable e) {
		// Convert the exception stack trace to a print stream
		ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
		PrintStream printStream = new PrintStream(outputStream);
		e.printStackTrace(printStream);
		printStream.close();
		// Log the exception to the app log file
		AppLog.logToFile(outputStream.toString());
		// Run dialog related stuff on the UI thread
		Platform.runLater(() -> {
			if(Constants.SHOW_EXCEPTIONS && !isExceptionDialogShowing) {
				isExceptionDialogShowing = true;
				// Construct an alert
				Alert alert = new Alert(Alert.AlertType.ERROR);
				alert.initOwner(Locator.getMainApp().getStage());
				alert.setTitle("An error occurred");
				alert.setHeaderText("An unexpected exception was thrown");
				alert.setContentText("If you think this is an error in the program, copy the details below and send them to the developer. You should restart the application before you continue working.");
				alert.initModality(Modality.NONE);

				// Convert stack trace to string
				StringWriter sw = new StringWriter();
				PrintWriter pw = new PrintWriter(sw);
				e.printStackTrace(pw);
				String exceptionText = sw.toString();

				// Create controls to show the exception
				Label label = new Label("Exception stacktrace:");
				TextArea textArea = new TextArea(exceptionText);
				textArea.setEditable(false);
				textArea.setWrapText(true);
				textArea.setMaxWidth(Double.MAX_VALUE);
				textArea.setMaxHeight(Double.MAX_VALUE);
				GridPane.setVgrow(textArea, Priority.ALWAYS);
				GridPane.setHgrow(textArea, Priority.ALWAYS);
				GridPane gridPane = new GridPane();
				gridPane.setMaxWidth(Double.MAX_VALUE);
				gridPane.add(label, 0, 0);
				gridPane.add(textArea, 0, 1);

				// Set the content of the alert and show it
				alert.getDialogPane().setExpandableContent(gridPane);
				try {
					alert.show();
				} catch(Exception ignored) {
					System.err.println("Exception while showing the exception for: " + e.getMessage());
				}
				isExceptionDialogShowing = false;
			} else {
				System.err.println("Did not show exception dialog for: " + e.getMessage());
			}
		});
	}
}

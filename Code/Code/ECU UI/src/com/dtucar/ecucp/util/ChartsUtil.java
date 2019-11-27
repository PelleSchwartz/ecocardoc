package com.dtucar.ecucp.util;

import javafx.embed.swing.SwingFXUtils;
import javafx.geometry.Insets;
import javafx.scene.SnapshotParameters;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.*;
import javafx.scene.image.WritableImage;
import javafx.scene.layout.GridPane;
import javafx.stage.FileChooser;
import javafx.stage.Modality;
import javafx.stage.Stage;

import javax.imageio.ImageIO;
import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;

/**
 * The ChartsUtil class contains utility methods to configure and add functions to charts
 */
public class ChartsUtil {
	/**
	 * Configure the charts (generically for all charts): This adds the format, reset and export context menus
	 *
	 * @param chart
	 */
	public static void configChart(XYChart chart) {
		// Prepare the context menu
		final ContextMenu contextMenu = new ContextMenu();
		MenuItem format = new MenuItem("Format...");
		MenuItem reset = new MenuItem("Reset");
		SeparatorMenuItem separatorMenuItem = new SeparatorMenuItem();
		MenuItem export = new MenuItem("Export");
		contextMenu.getItems().addAll(format, reset, separatorMenuItem, export);
		// Format calls showFormatAxisDialog
		format.setOnAction(event -> showFormatAxisDialog(chart));
		// Reset clears the series data
		reset.setOnAction(event -> {
			for(Object series : chart.getData()) {
				//noinspection unchecked
				((XYChart.Series<Number, Number>) series).getData().clear();
			}
		});
		// Export saves the charts as a PNG snapshot
		export.setOnAction(event -> {
			WritableImage image = chart.snapshot(new SnapshotParameters(), null);
			FileChooser fileChooser = new FileChooser();
			fileChooser.setTitle("Save File");
			fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("Portable Network Graphics file (PNG)", "*.png"));
			fileChooser.setInitialDirectory(new File("."));
			File file = fileChooser.showSaveDialog(Locator.getMainApp().getStage());
			try {
				if(file != null) ImageIO.write(SwingFXUtils.fromFXImage(image, null), "png", file);
			} catch(IOException ignored) {
			}
		});
		// Hide the context menu when the mouse clicks elsewhere and show it on right click
		chart.setOnMousePressed(event -> {
			if(contextMenu.isShowing()) contextMenu.hide();
			if(event.isSecondaryButtonDown()) {
				contextMenu.show(chart, event.getScreenX(), event.getScreenY());
			}
		});
	}

	/**
	 * Show the dialog to format the x-, and y-axis of the chart
	 *
	 * @param chart the chart
	 */
	private static void showFormatAxisDialog(XYChart chart) {
		// Create the custom dialog with minimum decorations
		Dialog<List<String>> dialog = new Dialog<>();
		dialog.setTitle("Format axis");
		dialog.initOwner(Locator.getMainApp().getStage());
		dialog.initModality(Modality.NONE);
		dialog.setHeaderText(null);
		dialog.setGraphic(null);
		Stage stage = (Stage) dialog.getDialogPane().getScene().getWindow();
		// Close the dialog when it loses focus (the user clicks behind the dialog)
		stage.focusedProperty().addListener((ov, t, t1) -> {
			if(!t1) dialog.close();
		});
		// Add OK and Cancel buttons
		dialog.getDialogPane().getButtonTypes().addAll(ButtonType.OK, ButtonType.CANCEL);
		// Create the text fields for entering minimum and maximum values for the axes
		GridPane grid = new GridPane();
		grid.setHgap(10);
		grid.setVgap(10);
		grid.setPadding(new Insets(20, 10, 10, 10));
		TextField xMinimumText = new TextField();
		xMinimumText.setPromptText("Auto");
		TextField xMaximumText = new TextField();
		xMaximumText.setPromptText("Auto");
		TextField yMinimumText = new TextField();
		yMinimumText.setPromptText("Auto");
		TextField yMaximumText = new TextField();
		yMaximumText.setPromptText("Auto");
		NumberAxis xAxis = (NumberAxis) chart.getXAxis();
		NumberAxis yAxis = (NumberAxis) chart.getYAxis();
		xMinimumText.setText(Double.toString(xAxis.getLowerBound()));
		xMaximumText.setText(Double.toString(xAxis.getUpperBound()));
		yMinimumText.setText(Double.toString(yAxis.getLowerBound()));
		yMaximumText.setText(Double.toString(yAxis.getUpperBound()));
		grid.add(new Label("Minimum"), 1, 0);
		grid.add(new Label("Maximum"), 2, 0);
		grid.add(new Label("X-axis:"), 0, 1);
		grid.add(xMinimumText, 1, 1);
		grid.add(xMaximumText, 2, 1);
		grid.add(new Label("Y-axis:"), 0, 2);
		grid.add(yMinimumText, 1, 2);
		grid.add(yMaximumText, 2, 2);
		dialog.getDialogPane().setContent(grid);
		// When OK is pressed, return the values as string in a list
		dialog.setResultConverter(dialogButton -> {
			if(dialogButton == ButtonType.OK) return Arrays.asList(xMinimumText.getText(), xMaximumText.getText(), yMinimumText.getText(), yMaximumText.getText());
			return null;
		});

		// Show the dialog
		Optional<List<String>> result = dialog.showAndWait();

		// When the dialog is closed
		result.ifPresent(r -> {
			if(Constants.DEBUG) System.out.println(Arrays.toString(r.toArray()));
			// Get the four strings, parse the values and adapt the chart based on them
			if(r.size() != 4) return;
			if(r.get(0).isEmpty() || r.get(1).isEmpty()) {
				xAxis.setLowerBound(0);
				xAxis.setAutoRanging(true);
			} else {
				try {
					double lower = Double.parseDouble(r.get(0));
					double upper = Double.parseDouble(r.get(1));
					xAxis.setAutoRanging(false);
					xAxis.setLowerBound(lower);
					xAxis.setUpperBound(upper);
					xAxis.setTickUnit(Math.abs(xAxis.getUpperBound() - xAxis.getLowerBound()) / 20);
				} catch(NumberFormatException ignored) {
					xAxis.setLowerBound(0);
					xAxis.setAutoRanging(true);
				}
			}
			if(r.get(2).isEmpty() || r.get(3).isEmpty()) {
				yAxis.setLowerBound(0);
				yAxis.setAutoRanging(true);
			} else {
				try {
					double lower = Double.parseDouble(r.get(2));
					double upper = Double.parseDouble(r.get(3));
					yAxis.setAutoRanging(false);
					yAxis.setLowerBound(lower);
					yAxis.setUpperBound(upper);
					yAxis.setTickUnit(Math.abs(yAxis.getUpperBound() - yAxis.getLowerBound()) / 5);
				} catch(NumberFormatException ignored) {
					yAxis.setLowerBound(0);
					yAxis.setAutoRanging(true);
				}
			}
		});
	}
}

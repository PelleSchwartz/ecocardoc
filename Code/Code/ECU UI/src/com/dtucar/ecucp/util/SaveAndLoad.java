package com.dtucar.ecucp.util;

import com.dtucar.ecucp.communication.TeensySync;
import com.dtucar.ecucp.controller.CockpitController;
import com.dtucar.ecucp.controller.ConfigurationController;
import com.dtucar.ecucp.controller.TestController;
import com.dtucar.ecucp.model.LutValue;
import com.dtucar.ecucp.model.TestValue;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonArray;
import javafx.collections.ObservableList;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.HashMap;
import java.util.List;

/**
 * The SaveAndLoad class contains functions related to saving and loading configuration files
 */
public class SaveAndLoad {
	private static final Gson gson = new GsonBuilder().setPrettyPrinting().create();
	private static File file = null;

	/**
	 * Save configuration to file
	 *
	 * @param saveToExisting          whether or not to try and save to the existing file (false when using "Save as" (show file picker), true when using "Save")
	 * @param cockpitController       cockpitController
	 * @param configurationController configurationController
	 * @param testController          testController
	 * @throws IOException if something goes wrong
	 */
	public static void saveAll(boolean saveToExisting, CockpitController cockpitController, ConfigurationController configurationController, TestController testController) throws IOException {
		saveAll(null, saveToExisting, cockpitController, configurationController, testController);
	}

	/**
	 * Save configuration to file
	 *
	 * @param overrideFile            override the previous save location with this save location (used when saving config from TestController)
	 * @param saveToExisting          whether or not to try and save to the existing file (false when using "Save as" (show file picker), true when using "Save")
	 * @param cockpitController       cockpitController
	 * @param configurationController configurationController
	 * @param testController          testController
	 * @throws IOException if something goes wrong
	 */
	public static void saveAll(File overrideFile, boolean saveToExisting, CockpitController cockpitController, ConfigurationController configurationController, TestController testController) throws IOException {
		// If we don't have a path already, show the file picker dialog
		if((file == null || !saveToExisting) && overrideFile == null) {
			FileChooser fileChooser = new FileChooser();
			fileChooser.setTitle("Save File");
			fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("Text file (TXT)", "*.txt"));
			fileChooser.setInitialDirectory(new File("."));
			file = fileChooser.showSaveDialog(Locator.getMainApp().getStage());
		}
		// Save to file
		if(file != null || overrideFile != null) {
			File file = overrideFile != null ? overrideFile : SaveAndLoad.file;
			OutputStreamWriter writer = new OutputStreamWriter(new FileOutputStream(file), "UTF-8");
			JsonArray array = new JsonArray();
			array.addAll(configurationController.getAll());
			array.addAll(cockpitController.getAll());
			array.addAll(testController.getAll());
			String string = gson.toJson(array);
			writer.write(string.replaceAll("\n", System.lineSeparator()));
			writer.flush();
			writer.close();
			AppLog.log("Configuration saved to: " + file.getName());
		}
	}

	/**
	 * Load all configurations from a save file
	 *
	 * @return a HashMap with the values from the save file
	 * @throws IOException if something goes wrong
	 */
	public static HashMap<String, Object> loadAll() throws IOException {
		// Open a FileChoose dialog
		FileChooser fileChooser = new FileChooser();
		fileChooser.setTitle("Open File");
		fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("Text file (TXT)", "*.txt"));
		fileChooser.setInitialDirectory(new File("."));
		file = fileChooser.showOpenDialog(Locator.getMainApp().getStage());
		if(file != null) {
			// Read the file content to a string
			byte[] encoded = Files.readAllBytes(file.toPath());
			String fileContent = new String(encoded, StandardCharsets.UTF_8);
			// Parse the save file content using TeensySync (since it uses the same syntax)
			List<HashMap<String, Object>> hashMapList = TeensySync.parseData(fileContent);
			if(hashMapList != null) {
				if(hashMapList.size() != 1) AppLog.error("Loaded more than 1 config!");
				if(hashMapList.get(0) != null) {
					hashMapList.get(0).put(Constants.SOURCE_KEY, Constants.SOURCE_SAVE_AND_LOAD);
					AppLog.log("Configuration loaded from: " + file.getName());
					return hashMapList.get(0);
				}
			}
		}
		return null;
	}

	/**
	 * Show a DirectoryChooser dialog to choose a directory
	 *
	 * @return a file pointing to a directory
	 */
	public static File chooseDirectory() {
		DirectoryChooser fileChooser = new DirectoryChooser();
		fileChooser.setTitle("Choose folder");
		fileChooser.setInitialDirectory(new File("."));
		return fileChooser.showDialog(Locator.getMainApp().getStage());
	}

	/**
	 * Export the TestValues from the test table to a file
	 *
	 * @param data the test values
	 * @throws IOException if something goes wrong
	 */
	public static void exportTestTable(ObservableList<TestValue> data) throws IOException {
		FileChooser fileChooser = new FileChooser();
		fileChooser.setTitle("Save File");
		fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("Text file (TXT)", "*.txt"));
		fileChooser.setInitialDirectory(new File("."));
		File file = fileChooser.showSaveDialog(Locator.getMainApp().getStage());
		if(file != null) {
			OutputStreamWriter writer = new OutputStreamWriter(new FileOutputStream(file), "UTF-8");
			JsonArray testArray = TableUtil.testValuesToJsonArray(data.toArray(new TestValue[0]));
			String string = gson.toJson(testArray);
			writer.write(string.replaceAll("\n", System.lineSeparator()));
			writer.flush();
			writer.close();
		}
	}

	/**
	 * Export the LutValues from the LUT table to a file
	 *
	 * @param data the LUT values
	 * @throws IOException if something goes wrong
	 */
	public static void exportLutTable(ObservableList<LutValue> data) throws IOException {
		FileChooser fileChooser = new FileChooser();
		fileChooser.setTitle("Save File");
		fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("Text file (TXT)", "*.txt"));
		fileChooser.setInitialDirectory(new File("."));
		File file = fileChooser.showSaveDialog(Locator.getMainApp().getStage());
		if(file != null) {
			OutputStreamWriter writer = new OutputStreamWriter(new FileOutputStream(file), "UTF-8");
			JsonArray lutArray = TableUtil.lutValuesToJsonArray(data.toArray(new LutValue[0]));
			String string = gson.toJson(lutArray);
			writer.write(string.replaceAll("\n", System.lineSeparator()));
			writer.flush();
			writer.close();
		}
	}

	/**
	 * Save a string to a file (for example the CAN or RS232 debug text)
	 *
	 * @param text                string to save
	 * @param suggestedFileName   file name to suggest in the file chooser dialog
	 * @param suggestedExtensions file extension to suggested in the file chooser dialog (e.g. .csv or .txt)
	 * @throws IOException if something goes wrong
	 */
	public static void saveStringToFile(String text, String suggestedFileName, String suggestedExtensions) throws IOException {
		// Open a FileChoose dialog
		FileChooser fileChooser = new FileChooser();
		fileChooser.setTitle("Save File");
		// Get the date and time
		DateTimeFormatter dtf = DateTimeFormatter.ofPattern("yyyy-MM-dd HH-mm");
		LocalDateTime now = LocalDateTime.now();
		String time = dtf.format(now);
		// Set the suggested file name as the current date and time and the suggested file name
		fileChooser.setInitialFileName(time + " " + suggestedFileName);
		// Set the suggested file extension
		if(suggestedExtensions.contains("csv")) fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("Comma Separated file (CSV)", "*.csv"));
		fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("Text file (TXT)", "*.txt"));
		// Set the initial directory as the one where the application is be executed from
		fileChooser.setInitialDirectory(new File("."));
		// Show the dialog
		File file = fileChooser.showSaveDialog(Locator.getMainApp().getStage());
		// Save to file
		if(file != null) {
			OutputStreamWriter writer = new OutputStreamWriter(new FileOutputStream(file), "UTF-8");
			writer.write(text.replaceAll("\n", System.lineSeparator()));
			writer.flush();
			writer.close();
		}
	}
}

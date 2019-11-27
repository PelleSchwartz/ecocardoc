package com.dtucar.ecucp.controller;

import com.dtucar.ecucp.communication.TeensySync;
import com.dtucar.ecucp.communication.TeensySync.EcuEnum;
import com.dtucar.ecucp.model.ConfigValue;
import com.dtucar.ecucp.model.LutValue;
import com.dtucar.ecucp.util.AppLog;
import com.dtucar.ecucp.util.Constants;
import com.dtucar.ecucp.util.TableUtil;
import com.dtucar.ecucp.view.ConfigTextField;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.util.Callback;

import java.util.HashMap;
import java.util.List;

/**
 * The <code>ConfigurationController</code> is the code behind the <code>ConfigurationView</code>.<br>
 * This view shows several configurable text fields below a table for the RPM to injection and ignition LUT.
 */
public class ConfigurationController extends BaseController {
	private static final String TAG = "ConfigurationController";

	@FXML
	private ConfigTextField engineCalibrationText;
	@FXML
	private ConfigTextField maxRpmText;
	@FXML
	private ConfigTextField starterLimitText;
	@FXML
	private ConfigTextField idleLimitText;
	@FXML
	private ConfigTextField fuelTankVolumeText;
	@FXML
	private ConfigTextField ignitionDwellTimeText;
	@FXML
	private ConfigTextField injectionTimingText;
	@FXML
	private ConfigTextField idleInjectionText;
	@FXML
	private ConfigTextField gearChangeWaitText;
	@FXML
	private ConfigTextField gearChangeLimitText;
	@FXML
	private ConfigTextField gearUpSpeedText;
	@FXML
	private ConfigTextField gearDownSpeedText;
	@FXML
	private ConfigTextField fuelConversionSlopeText;
	@FXML
	private ConfigTextField fuelConversionInterceptText;
	@FXML
	private ConfigTextField lambdaConversionSlopeText;
	@FXML
	private ConfigTextField lambdaConversionInterceptText;
	@FXML
	private ConfigTextField fuelDensityText;
	@FXML
	private ConfigTextField fuelLowerHeatingValueText;
	@FXML
	private ConfigTextField equivalentLowerHeatingValueText;
	@FXML
	private ConfigTextField wheelDiameterText;
	@FXML
	private ConfigTextField pulsesPerRevolutionText;
	@FXML
	private ConfigTextField gearNeutralPwmText;
	@FXML
	private ConfigTextField gear1PwmText;
	@FXML
	private ConfigTextField gear2PwmText;
	/**
	 * The RPM to injection and ignition LUT table
	 */
	@FXML
	private TableView<LutValue> lutTable;

	/**
	 * The maximum (last) RPM value of the LUT table
	 */
	private static final int MAX_LUT_TABLE_RPM = 6000;
	/**
	 * The increments between each row of RPM value in the LUT table
	 */
	private static final int LUT_TABLE_INCREMENTS = 250;
	/**
	 * TODO NO idea what this does
	 */
	private static final int CONFIGURATION_UPDATE_PERIOD = 1000;    // [ms]
	/**
	 * The data of the LUT table
	 */
	private final ObservableList<LutValue> lutData = FXCollections.observableArrayList();
	/**
	 * Whether or not we have loaded the initial values from the ECU
	 */
	private boolean hasLoadedInitialValuesFromTeensy = false;
	/**
	 * The time (in System Millis) of the last configuration request time
	 */
	private long lastConfigurationRequestTime = 0;
	/**
	 * Fuel correction value (which is multiplied onto the table when the reset from fuel correction function is used)
	 */
	private float fuelCorrection = 1;

	@FXML
	public void initialize() {
		// Set up the LUT table
		setupLutTable(lutTable, lutData);

		// OnUpdate listeners for all configuration fields
		// When a value is updated, send the new configuration to the Teensy
		engineCalibrationText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.ENGINE_CALIBRATION, value)));
		maxRpmText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.MAX_RPM, value)));
		starterLimitText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.STARTER_LIMIT, value)));
		idleLimitText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.IDLE_LIMIT, value)));
		fuelTankVolumeText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.FUEL_TANK_VOLUME, value)));
		ignitionDwellTimeText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.IGNITION_DWELL_TIME, value)));
		injectionTimingText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.INJECTION_TIMING, value)));
		idleInjectionText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.IDLE_INJECTION, value)));
		gearChangeWaitText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.GEAR_CHANGE_WAIT, value)));
		gearChangeLimitText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.GEAR_CHANGE_LIMIT, value)));
		gearUpSpeedText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.GEAR_UP_SPEED, value)));
		gearDownSpeedText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.GEAR_DOWN_SPEED, value)));
		gearNeutralPwmText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.GEAR_NEUTRAL_PWM, value)));
		gear1PwmText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.GEAR_1_PWM, value)));
		gear2PwmText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.GEAR_2_PWM, value)));
		fuelConversionSlopeText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.FUEL_CONVERSION_SLOPE, value)));
		fuelConversionInterceptText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.FUEL_CONVERSION_INTERCEPT, value)));
		lambdaConversionSlopeText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.LAMBDA_CONVERSION_SLOPE, value)));
		lambdaConversionInterceptText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.LAMBDA_CONVERSION_INTERCEPT, value)));
		fuelDensityText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.FUEL_DENSITY, value)));
		fuelLowerHeatingValueText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.FUEL_LOWER_HEATING_VALUE, value)));
		equivalentLowerHeatingValueText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.EQUIVALENT_FUEL_LOWER_HEATING_VALUE, value)));
		wheelDiameterText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.WHEEL_DIAMETER, value)));
		pulsesPerRevolutionText.setOnUpdateListener(value -> TeensySync.sendConfig(new ConfigValue(EcuEnum.PULSES_PER_REVOLUTION, value)));
	}

	/**
	 * Set up the LUT table
	 *
	 * @param lutTable the LUT table
	 * @param lutData  the LUT data
	 */
	@SuppressWarnings("unchecked")
	private void setupLutTable(TableView lutTable, ObservableList<LutValue> lutData) {
		// Table Initialization (set values to 0)
		for(int i = 0; i <= MAX_LUT_TABLE_RPM; i += LUT_TABLE_INCREMENTS) {
			lutData.add(new LutValue(i, 0, 0, 0));
		}
		// Add the RPM column
		Callback<TableColumn, TableCell> cellFactory = p -> new TableUtil.EditingCell<LutValue>();
		TableColumn<LutValue, Integer> rpmColumn = new TableColumn<>("RPM");
		rpmColumn.setCellValueFactory(new PropertyValueFactory<>("rpm"));
		// Add the injection column for gear 1
		TableColumn injection1Column = new TableColumn("Injection 1 Length [µs]");
		injection1Column.setId("injection1_column");
		injection1Column.setCellValueFactory(new PropertyValueFactory<LutValue, Integer>("injection1"));
		injection1Column.setCellFactory(cellFactory);
		injection1Column.setOnEditCommit(
				// When the value is changed, send a LUT update to the Teensy
				(EventHandler<TableColumn.CellEditEvent<LutValue, Integer>>) t -> {
					LutValue editedValue = t.getTableView().getItems().get(t.getTablePosition().getRow());
					editedValue.setInjection1(t.getNewValue());
					TeensySync.sendLutUpdate(editedValue);
				}
		);
		// Add the injection column for gear 2
		TableColumn injection2Column = new TableColumn("Injection 2 Length [µs]");
		injection2Column.setId("injection2_column");
		injection2Column.setCellValueFactory(new PropertyValueFactory<LutValue, Integer>("injection2"));
		injection2Column.setCellFactory(cellFactory);
		injection2Column.setOnEditCommit(
				// When the value is changed, send a LUT update to the Teensy
				(EventHandler<TableColumn.CellEditEvent<LutValue, Integer>>) t -> {
					LutValue editedValue = t.getTableView().getItems().get(t.getTablePosition().getRow());
					editedValue.setInjection2(t.getNewValue());
					TeensySync.sendLutUpdate(editedValue);
				}
		);
		// Add the ignition column
		TableColumn ignitionColumn = new TableColumn("Ignition Timing [CAD BTDC]");
		ignitionColumn.setCellValueFactory(new PropertyValueFactory<LutValue, Integer>("ignition"));
		ignitionColumn.setCellFactory(cellFactory);
		ignitionColumn.setOnEditCommit(
				// When the value is changed, send a LUT update to the Teensy
				(EventHandler<TableColumn.CellEditEvent<LutValue, Integer>>) t -> {
					LutValue editedValue = t.getTableView().getItems().get(t.getTablePosition().getRow());
					editedValue.setIgnition(t.getNewValue());
					TeensySync.sendLutUpdate(editedValue);
				}
		);
		// Setup table
		TableUtil.setupTable(lutTable);
		// Set items
		lutTable.setItems(lutData);
		// Set columns
		lutTable.getColumns().addAll(rpmColumn, injection1Column, injection2Column, ignitionColumn);
		// Add context menu
		ContextMenu contextMenu = TableUtil.getImportExportContextMenu(lutData);
		MenuItem resetFromFuelCorrectionItem = new MenuItem("Reset from Fuel Correction");
		resetFromFuelCorrectionItem.setOnAction(event -> resetLutFromFuelCorrection());
		contextMenu.getItems().addAll(new SeparatorMenuItem(), resetFromFuelCorrectionItem);
		lutTable.setContextMenu(contextMenu);
		// Add tooltips on column headers
		Platform.runLater(() -> {
			TableUtil.setTooltipOnColumn(lutTable, injection1Column.getId(), "Injection times for the 1st gear");
			TableUtil.setTooltipOnColumn(lutTable, injection2Column.getId(), "Injection times for the 2nd gear");
		});
	}

	/**
	 * Reset from LUT correction (through right click menu on the table): This multiplies all injection times with the fuel correction value (as configured in the CockpitView)
	 */
	private void resetLutFromFuelCorrection() {
		for(LutValue lutValue : lutData) {
			lutValue.setInjection1(Math.round(lutValue.getInjection1() * fuelCorrection));
			lutValue.setInjection2(Math.round(lutValue.getInjection2() * fuelCorrection));
		}
		lutTable.refresh();
		AppLog.log("LUT adjusted by " + fuelCorrection);
	}

	/**
	 * The loop that runs every {@link Constants#BASE_LOOP_PERIOD} (not used here)
	 */
	@Override
	protected void loop() {
	}

	/**
	 * This function is called from {@link MainController} when the associated tab or page is hidden from the user.
	 */
	@Override
	public void hide() {
		// Hide, except when it hasn't loaded initial values yet (then it should keep the data listener registered)
		if(hasLoadedInitialValuesFromTeensy) super.hide();
	}

	/**
	 * Called from the {@link MainController#onConnect()} when a connection has been successfully established
	 */
	public void onConnect() {
		// TODO NO idea what this does
		if(System.currentTimeMillis() - lastConfigurationRequestTime > CONFIGURATION_UPDATE_PERIOD)
			lastConfigurationRequestTime = System.currentTimeMillis();
		// Request data from Teensy
		TeensySync.requestConfig(
				EcuEnum.LUT,
				EcuEnum.FUEL_CORRECTION,
				EcuEnum.ENGINE_CALIBRATION,
				EcuEnum.MAX_RPM,
				EcuEnum.STARTER_LIMIT,
				EcuEnum.IDLE_LIMIT,
				EcuEnum.FUEL_TANK_VOLUME,
				EcuEnum.IGNITION_DWELL_TIME,
				EcuEnum.INJECTION_TIMING,
				EcuEnum.IDLE_INJECTION,
				EcuEnum.GEAR_CHANGE_WAIT,
				EcuEnum.GEAR_CHANGE_LIMIT,
				EcuEnum.GEAR_UP_SPEED,
				EcuEnum.GEAR_DOWN_SPEED,
				EcuEnum.GEAR_NEUTRAL_PWM,
				EcuEnum.GEAR_1_PWM,
				EcuEnum.GEAR_2_PWM,
				EcuEnum.FUEL_CONVERSION_SLOPE,
				EcuEnum.FUEL_CONVERSION_INTERCEPT,
				EcuEnum.LAMBDA_CONVERSION_SLOPE,
				EcuEnum.LAMBDA_CONVERSION_INTERCEPT,
				EcuEnum.FUEL_DENSITY,
				EcuEnum.FUEL_LOWER_HEATING_VALUE,
				EcuEnum.EQUIVALENT_FUEL_LOWER_HEATING_VALUE,
				EcuEnum.WHEEL_DIAMETER,
				EcuEnum.PULSES_PER_REVOLUTION);
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
	 * Called when new data is received to update the values
	 *
	 * @param data the hash map of the new data received
	 */
	@Override
	public void onDataReceived(HashMap<String, Object> data) {
		Platform.runLater(() -> {
			// Mark the data as unconfirmed when loading from a save file
			boolean confirmed = (data.containsKey(Constants.SOURCE_KEY) && !data.get(Constants.SOURCE_KEY).equals(Constants.SOURCE_SAVE_AND_LOAD));
			// LUT data fields
			if(data.containsKey(EcuEnum.LUT.get())) {
				// Get the LUT object
				Object lutObject = data.get(EcuEnum.LUT.get());
				//noinspection unchecked
				List<LutValue> list = (List<LutValue>) lutObject;
				// For each lut value
				for(LutValue lutValue : list) {
					// Check if an existing value with the same RPM is already in the table
					LutValue existingValue = lutData.stream().filter(l -> l.getRpm() == lutValue.getRpm()).findFirst().orElse(null);
					if(existingValue != null) {
						// If a value already exists, update it
						existingValue.setIgnition(lutValue.getIgnition());
						existingValue.setInjection1(lutValue.getInjection1());
						existingValue.setInjection2(lutValue.getInjection2());
					} else {
						// If the value does not already exist, add it
						lutData.add(lutValue);
					}
				}
				// If the value is not confirmed, send it to Teensy
				if(!confirmed) {
					LutValue[] lutArray = new LutValue[list.size()];
					lutArray = list.toArray(lutArray);
					TeensySync.sendLutUpdate(lutArray);
				}
				// Update the LUT table
				lutTable.refresh();
				// If a LUT has been received, that must mean we have loaded configurations
				hasLoadedInitialValuesFromTeensy = true;
			}
			// Other configuration fields
			// If the received data contains a certain configuration, parse it and set the configuration text field with the value confirmed (green)
			if(data.containsKey(EcuEnum.FUEL_CORRECTION.get()))
				fuelCorrection = ((Number) data.get(EcuEnum.FUEL_CORRECTION.get())).floatValue();
			if(data.containsKey(EcuEnum.ENGINE_CALIBRATION.get()))
				engineCalibrationText.setTextConfirmed(String.valueOf(data.get(EcuEnum.ENGINE_CALIBRATION.get())), confirmed);
			if(data.containsKey(EcuEnum.MAX_RPM.get())) maxRpmText.setTextConfirmed(String.valueOf(data.get(EcuEnum.MAX_RPM.get())), confirmed);
			if(data.containsKey(EcuEnum.STARTER_LIMIT.get()))
				starterLimitText.setTextConfirmed(String.valueOf(data.get(EcuEnum.STARTER_LIMIT.get())), confirmed);
			if(data.containsKey(EcuEnum.IDLE_LIMIT.get())) idleLimitText.setTextConfirmed(String.valueOf(data.get(EcuEnum.IDLE_LIMIT.get())), confirmed);
			if(data.containsKey(EcuEnum.FUEL_TANK_VOLUME.get()))
				fuelTankVolumeText.setTextConfirmed(String.valueOf(data.get(EcuEnum.FUEL_TANK_VOLUME.get())), confirmed);
			if(data.containsKey(EcuEnum.IGNITION_DWELL_TIME.get()))
				ignitionDwellTimeText.setTextConfirmed(String.valueOf(data.get(EcuEnum.IGNITION_DWELL_TIME.get())), confirmed);
			if(data.containsKey(EcuEnum.INJECTION_TIMING.get()))
				injectionTimingText.setTextConfirmed(String.valueOf(data.get(EcuEnum.INJECTION_TIMING.get())), confirmed);
			if(data.containsKey(EcuEnum.IDLE_INJECTION.get()))
				idleInjectionText.setTextConfirmed(String.valueOf(data.get(EcuEnum.IDLE_INJECTION.get())), confirmed);
			if(data.containsKey(EcuEnum.GEAR_CHANGE_WAIT.get()))
				gearChangeWaitText.setTextConfirmed(String.valueOf(data.get(EcuEnum.GEAR_CHANGE_WAIT.get())), confirmed);
			if(data.containsKey(EcuEnum.GEAR_CHANGE_LIMIT.get()))
				gearChangeLimitText.setTextConfirmed(String.valueOf(data.get(EcuEnum.GEAR_CHANGE_LIMIT.get())), confirmed);
			if(data.containsKey(EcuEnum.GEAR_UP_SPEED.get()))
				gearUpSpeedText.setTextConfirmed(String.valueOf(data.get(EcuEnum.GEAR_UP_SPEED.get())), confirmed);
			if(data.containsKey(EcuEnum.GEAR_DOWN_SPEED.get()))
				gearDownSpeedText.setTextConfirmed(String.valueOf(data.get(EcuEnum.GEAR_DOWN_SPEED.get())), confirmed);
			if(data.containsKey(EcuEnum.GEAR_NEUTRAL_PWM.get()))
				gearNeutralPwmText.setTextConfirmed(String.valueOf(data.get(EcuEnum.GEAR_NEUTRAL_PWM.get())), confirmed);
			if(data.containsKey(EcuEnum.GEAR_1_PWM.get())) gear1PwmText.setTextConfirmed(String.valueOf(data.get(EcuEnum.GEAR_1_PWM.get())), confirmed);
			if(data.containsKey(EcuEnum.GEAR_2_PWM.get())) gear2PwmText.setTextConfirmed(String.valueOf(data.get(EcuEnum.GEAR_2_PWM.get())), confirmed);
			if(data.containsKey(EcuEnum.FUEL_CONVERSION_SLOPE.get()))
				fuelConversionSlopeText.setTextConfirmed(String.valueOf(data.get(EcuEnum.FUEL_CONVERSION_SLOPE.get())), confirmed);
			if(data.containsKey(EcuEnum.FUEL_CONVERSION_INTERCEPT.get()))
				fuelConversionInterceptText.setTextConfirmed(String.valueOf(data.get(EcuEnum.FUEL_CONVERSION_INTERCEPT.get())), confirmed);
			if(data.containsKey(EcuEnum.LAMBDA_CONVERSION_SLOPE.get()))
				lambdaConversionSlopeText.setTextConfirmed(String.valueOf(data.get(EcuEnum.LAMBDA_CONVERSION_SLOPE.get())), confirmed);
			if(data.containsKey(EcuEnum.LAMBDA_CONVERSION_INTERCEPT.get()))
				lambdaConversionInterceptText.setTextConfirmed(String.valueOf(data.get(EcuEnum.LAMBDA_CONVERSION_INTERCEPT.get())), confirmed);
			if(data.containsKey(EcuEnum.FUEL_DENSITY.get())) fuelDensityText.setTextConfirmed(String.valueOf(data.get(EcuEnum.FUEL_DENSITY.get())), confirmed);
			if(data.containsKey(EcuEnum.FUEL_LOWER_HEATING_VALUE.get()))
				fuelLowerHeatingValueText.setTextConfirmed(String.valueOf(data.get(EcuEnum.FUEL_LOWER_HEATING_VALUE.get())), confirmed);
			if(data.containsKey(EcuEnum.EQUIVALENT_FUEL_LOWER_HEATING_VALUE.get()))
				equivalentLowerHeatingValueText.setTextConfirmed(String.valueOf(data.get(EcuEnum.EQUIVALENT_FUEL_LOWER_HEATING_VALUE.get())), confirmed);
			if(data.containsKey(EcuEnum.WHEEL_DIAMETER.get()))
				wheelDiameterText.setTextConfirmed(String.valueOf(data.get(EcuEnum.WHEEL_DIAMETER.get())), confirmed);
			if(data.containsKey(EcuEnum.PULSES_PER_REVOLUTION.get()))
				pulsesPerRevolutionText.setTextConfirmed(String.valueOf(data.get(EcuEnum.PULSES_PER_REVOLUTION.get())), confirmed);
		});
	}

	/**
	 * Produce a JsonArray containing all settings that need to be saved when using the "Save Configuration" function
	 *
	 * @return jsonArray with the settings to save
	 */
	public JsonArray getAll() {
		// Convert all configuration values and LUT table data to a JSON array with the EcuEnum key
		JsonArray array = new JsonArray();
		JsonArray lutArray = TableUtil.lutValuesToJsonArray(lutData.toArray(new LutValue[0]));
		array.addAll(lutArray);
		JsonObject object = new JsonObject();
		object.addProperty(EcuEnum.ENGINE_CALIBRATION.get(), engineCalibrationText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.MAX_RPM.get(), maxRpmText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.STARTER_LIMIT.get(), starterLimitText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.IDLE_LIMIT.get(), idleLimitText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.FUEL_TANK_VOLUME.get(), fuelTankVolumeText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.IGNITION_DWELL_TIME.get(), ignitionDwellTimeText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.INJECTION_TIMING.get(), injectionTimingText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.IDLE_INJECTION.get(), idleInjectionText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.GEAR_CHANGE_WAIT.get(), gearChangeWaitText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.GEAR_CHANGE_LIMIT.get(), gearChangeLimitText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.GEAR_UP_SPEED.get(), gearUpSpeedText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.GEAR_DOWN_SPEED.get(), gearDownSpeedText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.GEAR_NEUTRAL_PWM.get(), gearNeutralPwmText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.GEAR_1_PWM.get(), gear1PwmText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.GEAR_2_PWM.get(), gear2PwmText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.FUEL_CONVERSION_SLOPE.get(), fuelConversionSlopeText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.FUEL_CONVERSION_INTERCEPT.get(), fuelConversionInterceptText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.LAMBDA_CONVERSION_SLOPE.get(), lambdaConversionSlopeText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.LAMBDA_CONVERSION_INTERCEPT.get(), lambdaConversionInterceptText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.FUEL_DENSITY.get(), fuelDensityText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.FUEL_LOWER_HEATING_VALUE.get(), fuelLowerHeatingValueText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.EQUIVALENT_FUEL_LOWER_HEATING_VALUE.get(), equivalentLowerHeatingValueText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.WHEEL_DIAMETER.get(), wheelDiameterText.getValue());
		array.add(object);
		object = new JsonObject();
		object.addProperty(EcuEnum.PULSES_PER_REVOLUTION.get(), pulsesPerRevolutionText.getValue());
		array.add(object);
		return array;
	}
}

package com.dtucar.ecucp.controller;

import com.dtucar.ecucp.communication.TeensySync;
import com.dtucar.ecucp.model.ConfigValue;
import com.dtucar.ecucp.util.*;
import com.dtucar.ecucp.communication.TeensySync.EcuEnum;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import eu.hansolo.medusa.Gauge;
import eu.hansolo.medusa.GaugeBuilder;
import eu.hansolo.medusa.Section;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.chart.ScatterChart;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.MouseButton;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.text.Text;
import javafx.stage.Modality;
import javafx.stage.Stage;

import java.util.HashMap;
import java.util.Optional;

/**
 * The <code>CockpitController</code> is the code behind the <code>CockpitView</code>.<br>
 * This view is the startup view and contains visual elements to indicate the car's state:
 * - 3 temperature gauges (Water, Exhaust, Oil)
 * - Speed gauge
 * - RPM gauge
 * - Lambda indicator
 * - Lambda/RPM chart
 * - Small stuff: Battery voltage, Starter state, Brake state, Distance, Fuel consumption
 * It provides controls for the user to control:
 * - Burn
 * - Idle
 * - Fuel correction value
 * - Gear (Auto, Neutral, 1, 2)
 */
public class CockpitController extends BaseController {
	private static final String TAG = "CockpitController";

	/**
	 * The main GridPane that contains everything in the CockpitView
	 */
	@FXML
	private GridPane cockpit;
	/**
	 * The GridPane that contains the top row of gauges (temperature, speed and RPM)
	 */
	@FXML
	private GridPane gaugePane;
	/**
	 * The Slider allowing to change the fuel correction value
	 */
	@FXML
	private Slider fuelCorrectionSlider;
	/**
	 * The Text above the fuel correction slider
	 */
	@FXML
	private Text fuelCorrectionText;
	/**
	 * The CheckBox to set autogear
	 */
	@FXML
	private CheckBox autogearCheckbox;
	/**
	 * The RadioButton to select neutral gear
	 */
	@FXML
	private RadioButton neutralButton;
	/**
	 * The RadioButton to select gear 1
	 */
	@FXML
	private RadioButton gear1Button;
	/**
	 * The RadioButton to select gear 2
	 */
	@FXML
	private RadioButton gear2Button;
	/**
	 * The Text below the battery (showing the exact voltage)
	 */
	@FXML
	private Text batteryText;
	/**
	 * The TextField showing the distance
	 */
	@FXML
	private TextField distanceText;
	/**
	 * The TextField showing the fuel consumption (km/l)
	 */
	@FXML
	private TextField fuelConsumptionText;
	/**
	 * The ToggleButton to toggle burning on and off
	 */
	@FXML
	private ToggleButton burnButton;
	/**
	 * The ToggleButton to toggle idle on and off
	 */
	@FXML
	private ToggleButton idleButton;
	/**
	 * The image that shows the brake state
	 */
	@FXML
	private ImageView brakeImage;
	/**
	 * The image that shows the starter state
	 */
	@FXML
	private ImageView starterImage;
	/**
	 * The Text below the lambda gauge showing the exact lambda value
	 */
	@FXML
	private Text lambdaText;
	/**
	 * The VBox into which the lambda gauge is added
	 */
	@FXML
	private VBox lambdaGaugeBox;

	/**
	 * The battery gauge (a small battery icon)
	 */
	@FXML
	private Gauge batteryGauge;
	/**
	 * The speed gauge (a dial)
	 */
	private Gauge speedGauge;
	/**
	 * The RPM gauge (a dial)
	 */
	private Gauge rpmGauge;
	/**
	 * The water temperature gauge (a thermometer-like column)
	 */
	private Gauge h2oTempGauge;
	/**
	 * The oil temperature gauge (a thermometer-like column)
	 */
	private Gauge oilTempGauge;
	/**
	 * The exhaust temperature gauge (a thermometer-like column)
	 */
	private Gauge exhaustTempGauge;
	/**
	 * The lambda gauge (a dial with colored areas)
	 */
	private Gauge lambdaGauge;

	/**
	 * The voltage value at or below which the battery is considered fully discharged on the battery gauge
	 */
	private static final float MIN_BATTERY = 11.0f;
	/**
	 * The voltage value at or above which the battery is considered fully charged on the battery gauge
	 */
	private static final float MAX_BATTERY = 12.5f;
	/**
	 * Whether a response was received since the last communication request
	 */
	private boolean lastRequestAnswered = true;
	/**
	 * Boolean to prevent the fuel correction slider to be programmatically moved while the user is changing the input
	 */
	private boolean fuelCorrectionValueHasJustChangedFromUserInput = false;

	/**
	 * The images for the brake state
	 */
	private Image brakeOkImage, brakeUnknownImage, brakeWarningImage;
	/**
	 * The Tooltip that shows up over the brake image
	 */
	private Tooltip brakeImageTooltip;
	/**
	 * The Tooltip that shows up over the fuel correction slider
	 */
	private Tooltip fuelCorrectionTooltip;
	/**
	 * The Tooltip that shows up over the exhaust temperature (showing the Teensy Core temperature)
	 */
	private Tooltip exhaustTooltip;

	/**
	 * The initialization method runs when the View is added to a Screen but before it is shown (on application start)
	 */
	@FXML
	public void initialize() {
		// Add the gauges
		addGauges();
		// Add Lambda/RPM chart
		final ScatterChart<Number, Number> chart = Charts.createLambdaRpmChartCockpit();
		cockpit.add(chart, 0, 2);
		// Bind the gear radio buttons to be disabled when autogear is selected
		neutralButton.disableProperty().bind(autogearCheckbox.selectedProperty());
		gear1Button.disableProperty().bind(autogearCheckbox.selectedProperty());
		gear2Button.disableProperty().bind(autogearCheckbox.selectedProperty());
		// Add listener to the fuel correction slider
		fuelCorrectionSlider.valueProperty().addListener((observable, oldValue, newValue) -> {
			float fuelCorrectionValue = ((int) (fuelCorrectionSlider.getValue() * 1000) / 1000.0f);
			fuelCorrectionTooltip.setText("Value: " + fuelCorrectionValue);
			fuelCorrectionValueHasJustChangedFromUserInput = true;
		});
		// Set context menu (to allow reset) for distance
		ContextMenu distanceContext = new ContextMenu();
		MenuItem distanceReset = new MenuItem("Reset");
		distanceReset.setOnAction(event -> TeensySync.sendConfig(new ConfigValue(EcuEnum.RESET_DISTANCE, 1)));
		distanceContext.getItems().addAll(distanceReset);
		distanceText.setContextMenu(distanceContext);
		// Set context menu (to allow reset) for fuel consumption
		ContextMenu fuelContext = new ContextMenu();
		MenuItem fuelReset = new MenuItem("Reset");
		fuelReset.setOnAction(event -> TeensySync.sendConfig(new ConfigValue(EcuEnum.RESET_FUEL_CONSUMPTION, 1)));
		fuelContext.getItems().addAll(fuelReset);
		fuelConsumptionText.setContextMenu(fuelContext);
		// Initialize brake images
		brakeUnknownImage = new Image(Locator.getMainApp().getClass().getResource("assets/brake_unknown.png").toString());
		brakeOkImage = new Image(Locator.getMainApp().getClass().getResource("assets/brake_ok.png").toString());
		brakeWarningImage = new Image(Locator.getMainApp().getClass().getResource("assets/brake_warning.png").toString());
		// Add tooltip to the brake image (showing the exact values when connected) and show it on click
		brakeImageTooltip = new Tooltip();
		brakeImageTooltip.setAutoHide(true);
		Tooltip.install(brakeImage, brakeImageTooltip);
		brakeImageTooltip.setText("Not connected");
		brakeImage.setOnMouseClicked(event -> {
			if(!brakeImageTooltip.isShowing()) brakeImageTooltip.show(Locator.getMainApp().getStage(), event.getScreenX(), event.getScreenY());
			else brakeImageTooltip.hide();
		});
		// Initialize starter image tooltip (showing starter state) and show it on click
		Tooltip starterImageTooltip = new Tooltip("Starter is running");
		starterImageTooltip.setAutoHide(true);
		Tooltip.install(starterImage, starterImageTooltip);
		starterImage.setOnMouseClicked(event -> {
			if(!starterImageTooltip.isShowing()) starterImageTooltip.show(Locator.getMainApp().getStage(), event.getScreenX(), event.getScreenY());
			else starterImageTooltip.hide();
		});
		// Initialize battery gauge tooltip (showing the exact values when connected) and show it on click
		Tooltip batteryGaugeTooltip = new Tooltip("Accessory battery voltage");
		batteryGaugeTooltip.setAutoHide(true);
		Tooltip.install(batteryGauge, batteryGaugeTooltip);
		batteryGauge.setOnMouseClicked(event -> {
			if(!batteryGaugeTooltip.isShowing()) batteryGaugeTooltip.show(Locator.getMainApp().getStage(), event.getScreenX(), event.getScreenY());
			else batteryGaugeTooltip.hide();
		});
		// Initialize exhaust tooltip (showing the exact values when connected) and show it on click
		exhaustTooltip = new Tooltip();
		exhaustTooltip.setAutoHide(true);
		Tooltip.install(exhaustTempGauge, exhaustTooltip);
		exhaustTempGauge.setOnMouseClicked(event -> {
			if(!exhaustTooltip.isShowing() && event.getButton() == MouseButton.SECONDARY)
				exhaustTooltip.show(Locator.getMainApp().getStage(), event.getScreenX(), event.getScreenY());
			else exhaustTooltip.hide();
		});
		// Initialize fuel correction tooltip (showing the exact values when connected)
		Tooltip.install(fuelCorrectionText, new Tooltip(Locator.getResources().getString("fuel_correction_tooltip")));
		fuelCorrectionTooltip = new Tooltip();
		fuelCorrectionTooltip.setAutoHide(true);
		Tooltip.install(fuelCorrectionSlider, fuelCorrectionTooltip);
		// Show the fuel correction tooltip while the left mouse button is being held down
		fuelCorrectionSlider.setOnMouseReleased(event -> {
			if(event.getButton() == MouseButton.PRIMARY) {
				fuelCorrectionTooltip.hide();
			}
		});
		fuelCorrectionSlider.setOnMousePressed(event -> {
			if(event.getButton() == MouseButton.PRIMARY) {
				fuelCorrectionTooltip.show(Locator.getMainApp().getStage(), event.getScreenX() + 20, event.getScreenY() + 20);
			}
		});
		// Open the fuel correction dialog (allowing precise text input for the value) on right click
		fuelCorrectionSlider.setOnMouseClicked(event -> {
			if(event.getButton() == MouseButton.SECONDARY) {
				// Create a minimal TextInputDialog for the input of the fuel correction value
				TextInputDialog dialog = new TextInputDialog(Float.toString((int) (fuelCorrectionSlider.getValue() * 1000) / 1000.0f));
				dialog.initOwner(Locator.getMainApp().getStage());
				dialog.initModality(Modality.NONE);
				dialog.setHeaderText(null);
				dialog.setGraphic(null);
				dialog.setTitle("Fuel correction value");
				Stage stage = (Stage) dialog.getDialogPane().getScene().getWindow();
				// Close it when focus is lost
				stage.focusedProperty().addListener((ov, t, t1) -> {
					if(!t1) dialog.close();
				});
				// Show the dialog
				Optional<String> result = dialog.showAndWait();
				// Parse the result
				result.ifPresent(r -> {
					try {
						fuelCorrectionSlider.setValue(Float.parseFloat(r));
						fuelCorrectionValueHasJustChangedFromUserInput = true;
						TeensySync.sendConfig(new ConfigValue(EcuEnum.FUEL_CORRECTION, ((int) (fuelCorrectionSlider.getValue() * 1000) / 1000.0f)));
					} catch(NumberFormatException ignored) {
					}
				});
			}
		});
	}

	/**
	 * Add and configure the gauges
	 */
	private void addGauges() {
		// Create all the gauges
		speedGauge = GaugeBuilder.create()
				.skinType(Gauge.SkinType.MODERN)
				.prefSize(300, 300)
				.title("SPEED")
				.unit("km/h")
				.maxValue(80)
				.build();
		rpmGauge = GaugeBuilder.create()
				.skinType(Gauge.SkinType.MODERN)
				.prefSize(300, 300)
				.title("RPM")
				.unit("RPM")
				.maxValue(5000)
				.build();
		h2oTempGauge = GaugeBuilder.create()
				.skinType(Gauge.SkinType.LINEAR)
				.prefSize(100, 300)
				.title("Water")
				.unit("°C")
				.maxValue(110)
				.areasVisible(true)
				.areas(new Section(75, 120, Color.RED))
				.build();
		oilTempGauge = GaugeBuilder.create()
				.skinType(Gauge.SkinType.LINEAR)
				.prefSize(100, 300)
				.title("Oil")
				.unit("°C")
				.maxValue(110)
				.areasVisible(true)
				.areas(new Section(90, 120, Color.RED), new Section(0, 50, Color.ORANGE))
				.build();
		exhaustTempGauge = GaugeBuilder.create()
				.skinType(Gauge.SkinType.LINEAR)
				.prefSize(100, 300)
				.title("Exhaust")
				.unit("°C")
				.maxValue(500)
				.areasVisible(true)
				.areas(new Section(400, 500, Color.RED), new Section(200, 400, Color.ORANGE))
				.build();
		lambdaGauge = GaugeBuilder.create()
				.skinType(Gauge.SkinType.HORIZONTAL)
				.prefSize(200, 200)
				.title("Lambda")
				.minValue(0.8)
				.maxValue(1.6)
				.decimals(2)
				.areasVisible(true)
				.tickLabelDecimals(2)
				.areas(new Section(1.1, 1.3, Color.GREEN), new Section(0.8, 1.1, Color.ORANGE), new Section(1.3, 1.6, Color.ORANGE))
				.build();
		// Prevent gauges from taking focus when using keyboard navigation
		h2oTempGauge.setFocusTraversable(false);
		speedGauge.setFocusTraversable(false);
		exhaustTempGauge.setFocusTraversable(false);
		rpmGauge.setFocusTraversable(false);
		oilTempGauge.setFocusTraversable(false);
		lambdaGauge.setFocusTraversable(false);
		// Add the gauges to the appropriate views
		gaugePane.add(h2oTempGauge, 0, 0);
		gaugePane.add(speedGauge, 1, 0);
		gaugePane.add(exhaustTempGauge, 2, 0);
		gaugePane.add(rpmGauge, 3, 0);
		gaugePane.add(oilTempGauge, 4, 0);
		VBox.setVgrow(lambdaGauge, Priority.ALWAYS);
		lambdaGaugeBox.getChildren().add(0, lambdaGauge);
	}

	/**
	 * Called when the autogear checkbox selection is changed (checked or unchecked)
	 */
	@FXML
	private void onAutogearChecked() {
		TeensySync.sendConfig(new ConfigValue(EcuEnum.AUTOGEAR, autogearCheckbox.isSelected() ? 1 : 2));
	}

	/**
	 * Called when the neutral gear radiobutton selection is changed
	 */
	@FXML
	private void onNeutralButtonPressed() {
		TeensySync.sendConfig(new ConfigValue(EcuEnum.NEUTRAL, neutralButton.isSelected() ? 1 : 0));
	}

	/**
	 * Called when the gear 1 radiobutton selection is changed
	 */
	@FXML
	private void onGear1ButtonPressed() {
		TeensySync.sendConfig(new ConfigValue(EcuEnum.GEAR_1, gear1Button.isSelected() ? 1 : 0));
	}

	/**
	 * Called when the gear 2 radiobutton selection is changed
	 */
	@FXML
	private void onGear2ButtonPressed() {
		TeensySync.sendConfig(new ConfigValue(EcuEnum.GEAR_2, gear2Button.isSelected() ? 1 : 0));
	}

	/**
	 * Called when the burn button is pressed
	 */
	@FXML
	private void onBurnButtonPressed() {
		// Burn only if not currently in idle
		if(idleButton.isSelected()) burnButton.setSelected(false);
		// If stopping a burn, stop immediately (start is handled in loop which checks burnButton.isSelected())
		if(!burnButton.isSelected()) TeensySync.sendConfig(new ConfigValue(EcuEnum.BURN, 0));
	}

	/**
	 * Called when the idle button is pressed
	 */
	@FXML
	private void onIdleButtonPressed() {
		// Idle only if not currently burning
		if(!burnButton.isSelected()) {
			// Disable autogear (the car does this as well, so this is mostly to indicate this)
			if(autogearCheckbox.isSelected()) autogearCheckbox.fire();
			// Ensure neutral gear and set idle (which is also update in the loop which checks idleButton.isSelected())
			TeensySync.sendConfig(new ConfigValue(EcuEnum.NEUTRAL, 1), new ConfigValue(EcuEnum.IDLE, idleButton.isSelected() ? 1 : 0));
		} else {
			idleButton.setSelected(false);
		}
	}

	/**
	 * The loop that runs every {@link Constants#BASE_LOOP_PERIOD}
	 */
	@Override
	protected void loop() {
		// Loop!
		if(!Locator.getCommunication().isConnected()) {
			// If we're not connected, set burn and idle off
			lastRequestAnswered = true;
			burnButton.setSelected(false);
			idleButton.setSelected(false);
		} else if(lastRequestAnswered) {
			// If the last request was answered, send the next one
			lastRequestAnswered = false;
			TeensySync.requestConfig(
					EcuEnum.STARTER,
					EcuEnum.BATTERY_VOLTAGE,
					EcuEnum.DISTANCE,
					EcuEnum.SPEED,
					EcuEnum.RPM,
					EcuEnum.FUEL_CONSUMPTION,
					EcuEnum.FUEL_CONSUMED,
					EcuEnum.LAMBDA,
					EcuEnum.OIL_TEMPERATURE,
					EcuEnum.WATER_TEMPERATURE,
					EcuEnum.EXHAUST_TEMPERATURE,
					EcuEnum.CORE_TEMPERATURE,
					EcuEnum.GEAR,
					EcuEnum.FUEL_CORRECTION,
					EcuEnum.AUTOGEAR,
					EcuEnum.BRAKE_FRONT,
					EcuEnum.BRAKE_REAR
			);
			// Send fuel correction, burn and idle configs
			TeensySync.sendConfig(
					new ConfigValue(EcuEnum.FUEL_CORRECTION, ((int) (fuelCorrectionSlider.getValue() * 1000) / 1000.0f)),
					new ConfigValue(EcuEnum.BURN, burnButton.isSelected() ? 1 : 0),
					new ConfigValue(EcuEnum.IDLE, idleButton.isSelected() ? 1 : 0));
			// Reset fuel correction changed flag
			fuelCorrectionValueHasJustChangedFromUserInput = false;
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
	 * This function is called from {@link MainController} when the associated tab or page is shown to the user
	 */
	@Override
	public void show() {
		super.show();
		lastRequestAnswered = true;
	}

	/**
	 * This function is called from {@link MainController} when the associated tab or page is hidden from the user
	 */
	@Override
	public void hide() {
		super.hide();
		// Set buttons to off and send stop burn signal to ECU
		burnButton.setSelected(false);
		idleButton.setSelected(false);
		TeensySync.sendConfig(new ConfigValue(EcuEnum.BURN, 0));
	}

	/**
	 * Called when new data is received to display it on the UI
	 *
	 * @param data the hash map of the new data received
	 */
	@Override
	public void onDataReceived(HashMap<String, Object> data) {
		// Data received from Teensy
		Platform.runLater(() -> {
			if(data.containsKey(EcuEnum.STARTER.get())) {
				boolean isStarterRunning = ((Number) data.get(EcuEnum.STARTER.get())).intValue() == 1;
				starterImage.setVisible(isStarterRunning);
			}
			if(data.containsKey(EcuEnum.BATTERY_VOLTAGE.get())) {
				float batteryVoltage = ((Number) data.get(EcuEnum.BATTERY_VOLTAGE.get())).floatValue();
				batteryText.setText(StringUtil.doubleToShortString(batteryVoltage));
				batteryGauge.setValue((batteryVoltage - MIN_BATTERY) / (MAX_BATTERY - MIN_BATTERY) * 100);
			}
			if(data.containsKey(EcuEnum.DISTANCE.get()))
				distanceText.setText(StringUtil.doubleToShortString(((Number) data.get(EcuEnum.DISTANCE.get())).floatValue()) + " m");
			if(data.containsKey(EcuEnum.SPEED.get())) speedGauge.setValue(((Number) data.get(EcuEnum.SPEED.get())).floatValue());
			if(data.containsKey(EcuEnum.RPM.get())) rpmGauge.setValue(((Number) data.get(EcuEnum.RPM.get())).floatValue());
			if(data.containsKey(EcuEnum.FUEL_CONSUMPTION.get()))
				fuelConsumptionText.setText(StringUtil.doubleToShortString(((Number) data.get(EcuEnum.FUEL_CONSUMPTION.get())).floatValue()) + " km/l");
			if(data.containsKey(EcuEnum.FUEL_CONSUMED.get()))
				fuelConsumptionText.getTooltip().setText(StringUtil.doubleToShortString(((Number) data.get(EcuEnum.FUEL_CONSUMED.get())).floatValue()) + " g");
			if(data.containsKey(EcuEnum.LAMBDA.get())) {
				float lambda = ((Number) data.get(EcuEnum.LAMBDA.get())).floatValue();
				if(lambda < lambdaGauge.getMinValue() || lambda > lambdaGauge.getMaxValue()) {
					lambdaGauge.setValue(lambda < lambdaGauge.getMinValue() ? lambdaGauge.getMinValue() : lambdaGauge.getMaxValue());
					lambdaText.setText(StringUtil.doubleToShortString(lambda));
					lambdaText.setVisible(true);
				} else {
					lambdaGauge.setValue(lambda);
					lambdaText.setVisible(false);
				}
			}
			if(data.containsKey(EcuEnum.WATER_TEMPERATURE.get())) h2oTempGauge.setValue(((Number) data.get(EcuEnum.WATER_TEMPERATURE.get())).floatValue());
			if(data.containsKey(EcuEnum.OIL_TEMPERATURE.get())) oilTempGauge.setValue(((Number) data.get(EcuEnum.OIL_TEMPERATURE.get())).floatValue());
			if(data.containsKey(EcuEnum.EXHAUST_TEMPERATURE.get()))
				exhaustTempGauge.setValue(((Number) data.get(EcuEnum.EXHAUST_TEMPERATURE.get())).floatValue());
			if(data.containsKey(EcuEnum.CORE_TEMPERATURE.get()))
				exhaustTooltip.setText("Teensy core:\n" + String.valueOf(data.get(EcuEnum.CORE_TEMPERATURE.get())) + " °C");
			if(data.containsKey(EcuEnum.GEAR.get())) {
				int gear = ((Number) data.get(EcuEnum.GEAR.get())).intValue();
				switch(gear) {
					case 0:
						neutralButton.setSelected(true);
						break;
					case 1:
						gear1Button.setSelected(true);
						break;
					case 2:
						gear2Button.setSelected(true);
						break;
				}
			}
			if(data.containsKey(EcuEnum.BRAKE_FRONT.get()) && data.containsKey(EcuEnum.BRAKE_REAR.get())) {
				float brakeFrontPressure = ((Number) data.get(EcuEnum.BRAKE_FRONT.get())).floatValue();
				float brakeRearPressure = ((Number) data.get(EcuEnum.BRAKE_REAR.get())).floatValue();
				float brakeAveragePressure;
				if(brakeFrontPressure > 0) brakeAveragePressure = (brakeFrontPressure + brakeRearPressure) / 2.0f;
				else brakeAveragePressure = brakeRearPressure;
				if(brakeAveragePressure > Constants.BRAKE_PRESSURE_THRESHOLD) brakeImage.setImage(brakeWarningImage);
				else if(brakeAveragePressure > -1) brakeImage.setImage(brakeOkImage);
				else brakeImage.setImage(brakeUnknownImage);
				brakeImageTooltip.setText("Average: " + brakeAveragePressure + " bar\nFront: " + brakeFrontPressure + " bar\nRear: " + brakeRearPressure + " bar");
			}
			if(data.containsKey(EcuEnum.FUEL_CORRECTION.get())) {
				float fuelCorrectionValue = ((Number) data.get(EcuEnum.FUEL_CORRECTION.get())).floatValue();
				if(!fuelCorrectionValueHasJustChangedFromUserInput) fuelCorrectionSlider.setValue(fuelCorrectionValue);
				fuelCorrectionText.setText("Fuel correction: " + fuelCorrectionValue);
			}
			if(data.containsKey(EcuEnum.AUTOGEAR.get())) {
				autogearCheckbox.selectedProperty().setValue(((Number) data.get(EcuEnum.AUTOGEAR.get())).intValue() > 0);
			}
			lastRequestAnswered = true;
		});
	}

	/**
	 * Produce a JsonArray containing all settings that need to be saved when using the "Save Configuration" function
	 *
	 * @return jsonArray with the settings to save
	 */
	public JsonArray getAll() {
		JsonArray array = new JsonArray();
		JsonObject object = new JsonObject();
		object.addProperty(EcuEnum.FUEL_CORRECTION.get(), ((int) (fuelCorrectionSlider.getValue() * 1000) / 1000.0f));
		array.add(object);
		return array;
	}
}

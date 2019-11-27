package com.dtucar.ecucp.controller;

import com.dtucar.ecucp.util.Charts;
import com.dtucar.ecucp.util.Constants;
import com.dtucar.ecucp.util.Locator;
import com.dtucar.ecucp.communication.TeensySync;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.chart.XYChart;
import javafx.scene.control.TitledPane;
import javafx.scene.layout.VBox;

import java.util.HashMap;

/**
 * The <code>ChartsController</code> is the code behind the <code>ChartsView</code>.<br>
 * This view shows several charts in a VBox with TitlePanes.<br>
 * Exceptionally, this Controller has its {@link #show()} method called at application start from the {@link MainController}
 * and its {@link #hide()} method has been overridden.
 * That means it will keep requesting data from the ECU in the background and draw charts unless paused through the {@link MainController#setPausedCharts(boolean)} method,
 * which is accessible in the user interface through a right click on the tab title.<br>
 * On application start, the charts drawing is paused, and it is only started once the user has visited the ChartsView once (on {@link #show()}.
 */
public class ChartsController extends BaseController {
	private static final String TAG = "ChartsController";

	/**
	 * The VBox containing all the TitlePanes with the charts
	 */
	@FXML
	private VBox chartsBox;

	/**
	 * The previously received distance
	 */
	private float previousDistance = 0;
	/**
	 * Whether a response was received since the last communication request
	 */
	private boolean lastRequestAnswered = true;
	/**
	 * Whether the charts drawing is paused or not
	 */
	private boolean paused = true;

	/**
	 * The initialization method runs when the View is added to a Screen but before it is shown (on application start)
	 */
	@FXML
	public void initialize() {
		// Add the oil temperature over time chart
		TitledPane oilPane = new TitledPane();
		oilPane.setText("Oil Temperature");
		oilPane.setContent(Charts.createOilTimeChart());
		chartsBox.getChildren().add(oilPane);
		// Add the water temperature over time chart
		TitledPane waterPane = new TitledPane();
		waterPane.setText("Water Temperature");
		waterPane.setContent(Charts.createWaterTimeChart());
		chartsBox.getChildren().add(waterPane);
		// Add the battery voltage over time chart
		TitledPane batteryPane = new TitledPane();
		batteryPane.setText("Battery");
		batteryPane.setContent(Charts.createBatteryChart());
		chartsBox.getChildren().add(batteryPane);
	}

	/**
	 * The loop that runs every {@link Constants#BASE_LOOP_PERIOD}
	 */
	@Override
	protected void loop() {
		if(!Locator.getCommunication().isConnected()) {
			// Do nothing if communication is not connected
			lastRequestAnswered = true;
			Charts.setTimeChartsStartTime(System.currentTimeMillis());
		} else if(lastRequestAnswered && !paused) {
			// If the charts drawing is not paused, request the data to be used for the drawing of charts
			// Here, all the values for all the charts used in the application are requested, since the onDataReceived function of the ChartsController
			// also manages the data series associated with charts shown in other views (not shown on the ChartsView).
			lastRequestAnswered = false;
			TeensySync.requestConfig(
					TeensySync.EcuEnum.STARTER,
					TeensySync.EcuEnum.BATTERY_VOLTAGE,
					TeensySync.EcuEnum.DISTANCE,
					TeensySync.EcuEnum.SPEED,
					TeensySync.EcuEnum.RPM,
					TeensySync.EcuEnum.FUEL_CONSUMPTION,
					TeensySync.EcuEnum.LAMBDA,
					TeensySync.EcuEnum.OIL_TEMPERATURE,
					TeensySync.EcuEnum.WATER_TEMPERATURE,
					TeensySync.EcuEnum.GEAR
			);
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
		// Perform the usual show tasks from the BaseController
		super.show();
		// Resume or start requesting data to draw charts
		setPaused(false);
		lastRequestAnswered = true;
	}

	/**
	 * Called when new data is received to add it to the associated charts series (for all charts in {@link Charts})
	 *
	 * @param data the hash map of the new data received
	 */
	@Override
	public void onDataReceived(HashMap<String, Object> data) {
		Platform.runLater(() -> {
			// Lambda/RPM scatter chart
			if(data.containsKey(TeensySync.EcuEnum.LAMBDA.get()) && data.containsKey(TeensySync.EcuEnum.RPM.get()) && data.containsKey(TeensySync.EcuEnum.GEAR.get())) {
				// If the RPM is not 0
				if(((Number) data.get(TeensySync.EcuEnum.RPM.get())).intValue() != 0) {
					// Create to data points (necessary, since the Lambda/RPM chart is both added to the CockpitView and TestView
					final XYChart.Data<Number, Number> xyDataC = new XYChart.Data<>(((Number) data.get(TeensySync.EcuEnum.RPM.get())).floatValue(), ((Number) data.get(TeensySync.EcuEnum.LAMBDA.get())).floatValue());
					final XYChart.Data<Number, Number> xyDataT = new XYChart.Data<>(((Number) data.get(TeensySync.EcuEnum.RPM.get())).floatValue(), ((Number) data.get(TeensySync.EcuEnum.LAMBDA.get())).floatValue());
					// Add the data point to either the gear 1 or gear 2 series (for coloring and toggling according to gear)
					int gear = ((Number) data.get(TeensySync.EcuEnum.GEAR.get())).intValue();
					if(gear == 2) {		// Gear 2
						Charts.lambdaG2SeriesCockpit.getData().add(xyDataC);
						if(Charts.lambdaG2SeriesCockpit.getData().size() > 10000) Charts.lambdaG2SeriesCockpit.getData().remove(0);
						Charts.lambdaG2SeriesTest.getData().add(xyDataT);
						if(Charts.lambdaG2SeriesTest.getData().size() > 10000) Charts.lambdaG2SeriesTest.getData().remove(0);
					} else {        	// Gear 1
						Charts.lambdaG1SeriesCockpit.getData().add(xyDataC);
						if(Charts.lambdaG1SeriesCockpit.getData().size() > 10000) Charts.lambdaG1SeriesCockpit.getData().remove(0);
						Charts.lambdaG1SeriesTest.getData().add(xyDataT);
						if(Charts.lambdaG1SeriesTest.getData().size() > 10000) Charts.lambdaG1SeriesTest.getData().remove(0);
					}
				}
			}
			// Distance/time chart
			if(data.containsKey(TeensySync.EcuEnum.DISTANCE.get())) {
				// Get the time elapsed since we started drawing charts
				float time = (System.currentTimeMillis() - Charts.getTimeChartsStartTime()) / 1000.0f;    // [s]
				// Add a distance/time data point
				final XYChart.Data<Number, Number> xyData = new XYChart.Data<>(time, ((Number) data.get(TeensySync.EcuEnum.DISTANCE.get())).floatValue());
				Charts.distanceSeries.getData().add(xyData);
				// Remove oldest data point is series size gets too big
				if(Charts.distanceSeries.getData().size() > 1000) Charts.distanceSeries.getData().remove(0);
			}
			// Speed/time chart
			if(data.containsKey(TeensySync.EcuEnum.SPEED.get())) {
				// Get the time elapsed since we started drawing charts
				float time = (System.currentTimeMillis() - Charts.getTimeChartsStartTime()) / 1000.0f;    // [s]
				// Add a speed/time data point
				final XYChart.Data<Number, Number> xyData = new XYChart.Data<>(time, ((Number) data.get(TeensySync.EcuEnum.SPEED.get())).floatValue());
				Charts.speedTimeSeries.getData().add(xyData);
				// Remove oldest data point is series size gets too big
				if(Charts.speedTimeSeries.getData().size() > 1000) Charts.speedTimeSeries.getData().remove(0);
			}
			// Oil temperature/time chart
			if(data.containsKey(TeensySync.EcuEnum.OIL_TEMPERATURE.get())) {
				// Get the time elapsed since we started drawing charts
				float time = (System.currentTimeMillis() - Charts.getTimeChartsStartTime()) / 1000.0f;    // [s]
				// Add a oil temperature/time data point
				final XYChart.Data<Number, Number> xyData = new XYChart.Data<>(time, ((Number) data.get(TeensySync.EcuEnum.OIL_TEMPERATURE.get())).floatValue());
				Charts.oilSeries.getData().add(xyData);
				// Remove oldest data point is series size gets too big
				if(Charts.oilSeries.getData().size() > 1000) Charts.oilSeries.getData().remove(0);
			}
			// Water temperature/time chart
			if(data.containsKey(TeensySync.EcuEnum.WATER_TEMPERATURE.get())) {
				// Get the time elapsed since we started drawing charts
				float time = (System.currentTimeMillis() - Charts.getTimeChartsStartTime()) / 1000.0f;    // [s]
				// Add a water temperature/time data point
				final XYChart.Data<Number, Number> xyData = new XYChart.Data<>(time, ((Number) data.get(TeensySync.EcuEnum.WATER_TEMPERATURE.get())).floatValue());
				Charts.waterSeries.getData().add(xyData);
				// Remove oldest data point is series size gets too big
				if(Charts.waterSeries.getData().size() > 1000) Charts.waterSeries.getData().remove(0);
			}
			// Speed/distance and fuel consumption/distance charts
			if(data.containsKey(TeensySync.EcuEnum.DISTANCE.get()) && data.containsKey(TeensySync.EcuEnum.FUEL_CONSUMPTION.get()) && data.containsKey(TeensySync.EcuEnum.SPEED.get())) {
				float distance = ((Number) data.get(TeensySync.EcuEnum.DISTANCE.get())).floatValue();
				// If the distance has changed
				if(distance != previousDistance) {
					previousDistance = distance;
					// Add a speed/distance data point
					float speed = ((Number) data.get(TeensySync.EcuEnum.SPEED.get())).floatValue();
					final XYChart.Data<Number, Number> speedData = new XYChart.Data<>(distance, speed);
					Charts.speedDistanceSeries.getData().add(speedData);
					// Remove oldest data point is series size gets too big
					if(Charts.speedDistanceSeries.getData().size() > 10000) Charts.speedDistanceSeries.getData().remove(0);
					// Add a fuel consumption/distance data point
					final XYChart.Data<Number, Number> fuelData = new XYChart.Data<>(distance, ((Number) data.get(TeensySync.EcuEnum.FUEL_CONSUMPTION.get())).floatValue());
					Charts.fuelSeries.getData().add(fuelData);
					// Remove oldest data point is series size gets too big
					if(Charts.fuelSeries.getData().size() > 10000) Charts.fuelSeries.getData().remove(0);
				}
			}
			// Battery voltage/time chart
			if(data.containsKey(TeensySync.EcuEnum.BATTERY_VOLTAGE.get())) {
				// Get the time elapsed since we started drawing charts
				float time = (System.currentTimeMillis() - Charts.getTimeChartsStartTime()) / 1000.0f;    // [s]
				// Add a battery voltage/time data point
				final XYChart.Data<Number, Number> xyData = new XYChart.Data<>(time, ((Number) data.get(TeensySync.EcuEnum.BATTERY_VOLTAGE.get())).floatValue());
				Charts.batterySeries.getData().add(xyData);
				// Remove oldest data point is series size gets too big
				if(Charts.batterySeries.getData().size() > 1000) Charts.batterySeries.getData().remove(0);
			}
			lastRequestAnswered = true;
		});
	}

	/**
	 * Override the usual hide behavior from the BaseController to prevent unregistering the data received listener
	 * and thus save incoming charts data to the associated data series
	 */
	@Override
	public void hide() {
		// Keep charts updating
	}

	/**
	 * Pause the data requesting performed by the ChartsController to request data associated with charts (this can be helpful when debugging communication).
	 * This function can be accessed in the UI by a right click on the tab title.
	 *
	 * @param paused true if data requesting should be paused
	 */
	public void setPaused(boolean paused) {
		this.paused = paused;
	}

	/**
	 * Get the current state of the charts data requesting which occurs every {@link Constants#BASE_LOOP_PERIOD}
	 *
	 * @return true if paused
	 */
	public boolean getPaused() {
		return paused;
	}
}

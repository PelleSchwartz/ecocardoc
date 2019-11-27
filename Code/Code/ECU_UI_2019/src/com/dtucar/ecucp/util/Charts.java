package com.dtucar.ecucp.util;

import com.sun.javafx.charts.Legend;
import javafx.application.Platform;
import javafx.geometry.Side;
import javafx.scene.Cursor;
import javafx.scene.Node;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.ScatterChart;
import javafx.scene.chart.XYChart;
import javafx.scene.input.MouseButton;

import java.util.HashMap;

import static com.dtucar.ecucp.util.ChartsUtil.configChart;

/**
 * The Charts class contains all the methods for creating charts.
 * Their associated series are updated through the {@link com.dtucar.ecucp.controller.ChartsController}.
 */
public class Charts {
	public static final ScatterChart.Series<Number, Number> lambdaG1SeriesCockpit = new ScatterChart.Series<>();
	public static final ScatterChart.Series<Number, Number> lambdaG2SeriesCockpit = new ScatterChart.Series<>();
	public static final ScatterChart.Series<Number, Number> lambdaG1SeriesTest = new ScatterChart.Series<>();
	public static final ScatterChart.Series<Number, Number> lambdaG2SeriesTest = new ScatterChart.Series<>();
	public static final LineChart.Series<Number, Number> waterSeries = new LineChart.Series<>();
	public static final LineChart.Series<Number, Number> oilSeries = new LineChart.Series<>();
	public static final LineChart.Series<Number, Number> speedTimeSeries = new LineChart.Series<>();
	public static final LineChart.Series<Number, Number> distanceSeries = new LineChart.Series<>();
	public static final LineChart.Series<Number, Number> speedDistanceSeries = new LineChart.Series<>();
	public static final LineChart.Series<Number, Number> fuelSeries = new LineChart.Series<>();
	public static final LineChart.Series<Number, Number> batterySeries = new LineChart.Series<>();

	/**
	 * The start time in millis at which the time charts should start
	 */
	private static long timeChartsStartTime = 0;
	/**
	 * HashMap for the visibility of series (currently only used for Lambda/RPM charts, toggling gear 1 and gear 2 series)
	 */
	private static final HashMap<String, Boolean> seriesVisibility = new HashMap<>();

	/**
	 * Clear the data of all chart series
	 */
	public static void clearAllCharts() {
		Platform.runLater(() -> {
			lambdaG1SeriesCockpit.getData().clear();
			lambdaG2SeriesCockpit.getData().clear();
			lambdaG1SeriesTest.getData().clear();
			lambdaG2SeriesTest.getData().clear();
			waterSeries.getData().clear();
			oilSeries.getData().clear();
			speedTimeSeries.getData().clear();
			distanceSeries.getData().clear();
			speedDistanceSeries.getData().clear();
			fuelSeries.getData().clear();
			batterySeries.getData().clear();
		});
	}

	/**
	 * Create the Lambda/RPM chart for the cockpit view (this is necessary because charts annoyingly cannot use the same series)
	 *
	 * @return the chart
	 */
	public static ScatterChart<Number, Number> createLambdaRpmChartCockpit() {
		return createLambdaRpmChart(lambdaG1SeriesCockpit, lambdaG2SeriesCockpit);
	}

	/**
	 * Create the Lambda/RPM chart for the test view (this is necessary because charts annoyingly cannot use the same series)
	 *
	 * @return the chart
	 */
	public static ScatterChart<Number, Number> createLambdaRpmChartTest() {
		return createLambdaRpmChart(lambdaG1SeriesTest, lambdaG2SeriesTest);
	}


	/**
	 * Create the Lambda/RPM scatter chart (lambda values with respect to RPM, colored depending on gear)
	 *
	 * @param lambdaG1Series the series for gear 1
	 * @param lambdaG2Series the series for gear
	 * @return the chart
	 */
	private static ScatterChart<Number, Number> createLambdaRpmChart(ScatterChart.Series<Number, Number> lambdaG1Series, ScatterChart.Series<Number, Number> lambdaG2Series) {
		// Create Lambda/RPM chart
		final NumberAxis xAxis = new NumberAxis(0, 4000, 100);
		final NumberAxis yAxis = new NumberAxis(0.8, 1.6, 0.05);
		xAxis.setForceZeroInRange(false);
		xAxis.setLabel("RPM");
		xAxis.setAnimated(false);
		yAxis.setLabel("Lambda");
		yAxis.setAutoRanging(false);
		// Prepare series
		final String liveScatterChartCss = Charts.class.getResource("/com/dtucar/ecucp/view/LiveScatterChart.css").toExternalForm();
		final ScatterChart<Number, Number> chart = new ScatterChart<>(xAxis, yAxis);
		lambdaG1Series.getData().add(new XYChart.Data<>(0, 0));
		lambdaG1Series.setName("Gear 1");
		lambdaG2Series.getData().add(new XYChart.Data<>(0, 0));
		lambdaG2Series.setName("Gear 2");
		chart.getData().add(lambdaG1Series);
		chart.getData().add(lambdaG2Series);
		chart.getStylesheets().add(liveScatterChartCss);
		chart.setLegendVisible(true);
		chart.setLegendSide(Side.RIGHT);
		// Configure chart context menus
		configChart(chart);
		// Make legends toggle series
		lambdaG1Series.getData().clear();
		lambdaG2Series.getData().clear();
		for(Node node : chart.getChildrenUnmodifiable()) {
			if(node instanceof Legend) {
				Legend legend = (Legend) node;
				for(Legend.LegendItem legendItem : legend.getItems()) {
					for(XYChart.Series<Number, Number> series : chart.getData()) {
						if(series.getName().equals(legendItem.getText())) {
							legendItem.getSymbol().setCursor(Cursor.HAND);
							legendItem.getSymbol().setOnMouseClicked(mouseEvent -> {
								if(mouseEvent != null && mouseEvent.getButton() == MouseButton.PRIMARY) {
									boolean visible = false;
									if(seriesVisibility.containsKey(series.getName())) {
										visible = !seriesVisibility.get(series.getName());
									}
									seriesVisibility.put(series.getName(), visible);
									for(XYChart.Data<Number, Number> data : series.getData()) {
										if(data.getNode() != null) {
											data.getNode().setVisible(visible);
										}
									}
								}
							});
							break;
						}
					}
				}
			}
		}
		return chart;
	}

	/**
	 * Create the water temperature over time line chart
	 *
	 * @return the chart
	 */
	public static LineChart<Number, Number> createWaterTimeChart() {
		// Water temperature chart
		final NumberAxis xAxis = new NumberAxis();
		final NumberAxis yAxis = new NumberAxis();
		xAxis.setForceZeroInRange(false);
		xAxis.setLabel("Time [s]");
		xAxis.setAutoRanging(true);
		xAxis.setAnimated(true);
		xAxis.setForceZeroInRange(false);
		yAxis.setLabel("Water temperature [\u00B0C]");
		yAxis.setAutoRanging(true);
		yAxis.setAnimated(true);
		yAxis.setForceZeroInRange(false);
		// Prepare series
		final LineChart<Number, Number> chart = new LineChart<>(xAxis, yAxis);
		chart.getData().add(waterSeries);
		chart.setCreateSymbols(false);
		chart.setLegendVisible(false);
		chart.setAnimated(false);
		// Configure chart context menus
		configChart(chart);
		return chart;
	}

	/**
	 * Create the oil temperature over time line chart
	 *
	 * @return the chart
	 */
	public static LineChart<Number, Number> createOilTimeChart() {
		// Water temperature chart
		final NumberAxis xAxis = new NumberAxis();
		final NumberAxis yAxis = new NumberAxis();
		xAxis.setForceZeroInRange(false);
		xAxis.setLabel("Time [s]");
		xAxis.setAutoRanging(true);
		xAxis.setAnimated(true);
		xAxis.setForceZeroInRange(false);
		yAxis.setLabel("Oil temperature [\u00B0C]");
		yAxis.setAutoRanging(true);
		yAxis.setAnimated(true);
		yAxis.setForceZeroInRange(false);
		// Prepare series
		final LineChart<Number, Number> chart = new LineChart<>(xAxis, yAxis);
		chart.getData().add(oilSeries);
		chart.setCreateSymbols(false);
		chart.setLegendVisible(false);
		chart.setAnimated(false);
		// Configure chart context menus
		configChart(chart);
		return chart;
	}

	/**
	 * Create the speed over time line chart
	 *
	 * @return the chart
	 */
	public static LineChart<Number, Number> createSpeedTimeChart() {
		// Velocity chart
		final NumberAxis xAxis = new NumberAxis();
		final NumberAxis yAxis = new NumberAxis();
		xAxis.setForceZeroInRange(false);
		xAxis.setLabel("Time [s]");
		xAxis.setAutoRanging(true);
		xAxis.setAnimated(true);
		xAxis.setForceZeroInRange(false);
		yAxis.setLabel("Velocity [km/h]");
		yAxis.setAutoRanging(true);
		yAxis.setAnimated(true);
		yAxis.setForceZeroInRange(false);
		// Prepare series
		final LineChart<Number, Number> chart = new LineChart<>(xAxis, yAxis);
		chart.getData().add(speedTimeSeries);
		chart.setCreateSymbols(false);
		chart.setLegendVisible(false);
		chart.setAnimated(false);
		// Configure chart context menus
		configChart(chart);
		return chart;
	}

	/**
	 * Create the distance over time line chart
	 *
	 * @return the chart
	 */
	public static LineChart<Number, Number> createDistanceTimeChart() {
		// Distance chart
		final NumberAxis xAxis = new NumberAxis();
		final NumberAxis yAxis = new NumberAxis();
		xAxis.setForceZeroInRange(false);
		xAxis.setLabel("Time [s]");
		xAxis.setAutoRanging(true);
		xAxis.setAnimated(true);
		xAxis.setForceZeroInRange(false);
		yAxis.setLabel("Distance [m]");
		yAxis.setAutoRanging(true);
		yAxis.setAnimated(true);
		yAxis.setForceZeroInRange(false);
		// Prepare series
		final LineChart<Number, Number> chart = new LineChart<>(xAxis, yAxis);
		chart.getData().add(distanceSeries);
		chart.setCreateSymbols(false);
		chart.setLegendVisible(false);
		chart.setAnimated(false);
		// Configure chart context menus
		configChart(chart);
		return chart;
	}

	/**
	 * Create the speed over distance line chart
	 *
	 * @return the chart
	 */
	public static LineChart<Number, Number> createSpeedDistanceChart() {
		// Speed chart
		final NumberAxis xAxis = new NumberAxis(0, 500, 25);
		final NumberAxis yAxis = new NumberAxis(0, 50, 10);
		xAxis.setForceZeroInRange(false);
		xAxis.setLabel("Distance [m]");
		xAxis.setAnimated(false);
		yAxis.setLabel("Speed [km/h]");
		yAxis.setAutoRanging(false);
		// Prepare series
		LineChart<Number, Number> speedChart = new LineChart<>(xAxis, yAxis);
		speedChart.getData().add(speedDistanceSeries);
		speedChart.setLegendVisible(false);
		speedChart.setCreateSymbols(false);
		speedChart.setAnimated(false);
		// Configure chart context menus
		configChart(speedChart);
		return speedChart;
	}

	/**
	 * Create the fuel consumption over distance line chart
	 *
	 * @return the chart
	 */
	public static LineChart<Number, Number> createFuelConsumptionDistanceChart() {
		// Fuel consumption chart
		final NumberAxis xAxis = new NumberAxis(0, 500, 25);
		final NumberAxis yAxis = new NumberAxis(0, 600, 100);
		xAxis.setForceZeroInRange(false);
		xAxis.setLabel("Distance [m]");
		xAxis.setAnimated(false);
		yAxis.setLabel("Fuel consumption [km/l]");
		yAxis.setAutoRanging(true);
		// Prepare series
		LineChart<Number, Number> fuelChart = new LineChart<>(xAxis, yAxis);
		fuelChart.getData().add(fuelSeries);
		fuelChart.setLegendVisible(false);
		fuelChart.setCreateSymbols(false);
		fuelChart.setAnimated(false);
		// Configure chart context menus
		configChart(fuelChart);
		return fuelChart;
	}

	/**
	 * Create the battery chart
	 *
	 * @return the chart
	 */
	public static LineChart<Number, Number> createBatteryChart() {
		// Distance chart
		final NumberAxis xAxis = new NumberAxis();
		final NumberAxis yAxis = new NumberAxis();
		xAxis.setForceZeroInRange(false);
		xAxis.setLabel("Time [s]");
		xAxis.setAutoRanging(true);
		xAxis.setAnimated(true);
		xAxis.setForceZeroInRange(false);
		yAxis.setLabel("Voltage [V]");
		yAxis.setAutoRanging(true);
		yAxis.setAnimated(true);
		yAxis.setForceZeroInRange(false);
		// Prepare series
		final LineChart<Number, Number> chart = new LineChart<>(xAxis, yAxis);
		chart.getData().add(batterySeries);
		chart.setCreateSymbols(false);
		chart.setLegendVisible(false);
		chart.setAnimated(false);
		// Configure chart context menus
		configChart(chart);
		return chart;
	}

	/**
	 * Get the start time for the time charts
	 *
	 * @return start time in millis
	 */
	public static long getTimeChartsStartTime() {
		return timeChartsStartTime;
	}

	/**
	 * Set the start time for the time charts
	 *
	 * @param timeChartsStartTime start time in millis
	 */
	public static void setTimeChartsStartTime(long timeChartsStartTime) {
		Charts.timeChartsStartTime = timeChartsStartTime;
	}
}

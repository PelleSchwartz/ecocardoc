package com.dtucar.ecucp.model;

import com.dtucar.ecucp.util.Constants.TestState;

import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;

/**
 * The TestLogValue class is used to temporarily store values when running a test, before saving it to the test log file
 */
public class TestLogValue {
	/**
	 * The delimiter used for the test log file ("," for comma-separated file)
	 */
	private static final String D = ",";

	/**
	 * Values to be added to the test log file
	 */
	private long onTimeMillis, timeUtcMillis;
	private boolean starter, autoGear;
	private TestState testState;
	private int gear;
	private float batteryVoltage = -1,
			distance = -1,
			speed = -1,
			rpm = -1,
			fuelCorrection = -1,
			fuelConsumption = -1,
			fuelConsumed = -1,
			lambda = -1,
			oilTemperature = -1,
			waterTemperature = -1,
			exhaustTemperature = -1,
			brakeFront = -1,
			brakeRear = -1,
			ecuState = -1;

	/**
	 * This returns a string header separated by {@link #D} used as the first row in the test log in the same order as {@link #toString()}.
	 * The format is made so it matches the one used by ECU in dataLogger.cpp and the order of values used in the ECU.ino file.
	 * @return string line to be added as the first line in the test log
	 */
	public static String getHeader() {
		return "OnTime (ms)" + D + "Time (UTC)" + D + "Speed (km/h)" + D + "Distance (m)" + D + "RPM" + D + "Lambda" + D + "Starter" + D + "Gear" + D + "Fuel correction" + D + "Fuel consumption (km/l)" + D + "Fuel consumed (g)" + D + "Battery voltage (V)" + D + "Oil temperature (\u00B0C)" + D + "Water temperature (\u00B0C)" + D + "Exhaust temperature (\u00B0C)" + D + "Brake front (bar)" + D + "Brake rear (bar)" + D + "State" + D + "TestState" + System.lineSeparator();
	}

	/**
	 * This returns a string separated by {@link #D} representing all the values in this class in the same order as {@link #getHeader()}.<br/>
	 * The format is made so it matches the one used by ECU in dataLogger.cpp and the order of values used in the ECU.ino file.
	 * @return string line to be added to the test log
	 */
	public String toString() {
		DateTimeFormatter dtf = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss.SSS");
		LocalDateTime dateTime = LocalDateTime.ofInstant(Instant.ofEpochMilli(timeUtcMillis), ZoneId.systemDefault());
		return onTimeMillis + D + dtf.format(dateTime) + D + speed + D + distance + D + rpm + D + lambda + D + starter + D + (autoGear ? "A" : "M") + gear + D + fuelCorrection + D + fuelConsumption + D + fuelConsumed + D + batteryVoltage + D + oilTemperature + D + waterTemperature + D + exhaustTemperature + D + brakeFront + D + brakeRear + D + ecuState + D + testState + System.lineSeparator();
	}

	//region Setters for all the test log values
	public void setStarter(boolean starter) {
		this.starter = starter;
	}

	public void setBatteryVoltage(float batteryVoltage) {
		this.batteryVoltage = batteryVoltage;
	}

	public void setDistance(float distance) {
		this.distance = distance;
	}

	public void setSpeed(float speed) {
		this.speed = speed;
	}

	public void setRpm(float rpm) {
		this.rpm = rpm;
	}

	public void setFuelCorrection(float fuelCorrection) {
		this.fuelCorrection = fuelCorrection;
	}

	public void setFuelConsumption(float fuelConsumption) {
		this.fuelConsumption = fuelConsumption;
	}

	public void setFuelConsumed(float fuelConsumed) {
		this.fuelConsumed = fuelConsumed;
	}

	public void setLambda(float lambda) {
		this.lambda = lambda;
	}

	public void setOilTemperature(float oilTemperature) {
		this.oilTemperature = oilTemperature;
	}

	public void setWaterTemperature(float waterTemperature) {
		this.waterTemperature = waterTemperature;
	}

	public void setExhaustTemperature(float exhaustTemperature) {
		this.exhaustTemperature = exhaustTemperature;
	}

	public void setOnTimeMillis(long onTimeMillis) {
		this.onTimeMillis = onTimeMillis;
	}

	public void setTimeUtcMillis(long timeUtcMillis) {
		this.timeUtcMillis = timeUtcMillis;
	}

	public void setTestState(TestState testState) {
		this.testState = testState;
	}

	public void setGear(int gear) {
		this.gear = gear;
	}

	public void setAutoGear(boolean autoGear) {
		this.autoGear = autoGear;
	}

	public void setBrakeFront(float brakeFront) {
		this.brakeFront = brakeFront;
	}

	public void setBrakeRear(float brakeRear) {
		this.brakeRear = brakeRear;
	}

	public void setEcuState(float ecuState) {
		this.ecuState = ecuState;
	}
	//endregion Setters for all the test log values
}
package com.dtucar.ecucp.model;

import javafx.beans.property.SimpleIntegerProperty;

/**
 * A TestValue represents 1 row in the test table in the {@link com.dtucar.ecucp.controller.TestController}.<br/>
 * The test will start by going to the maxSpeed of the TestValue with index 0, then coast to the minSpeed of index 1, and then burn to maxSpeed of index 1.
 */
public class TestValue {
	/**
	 * The index of the TestValue. This dictates the order of the values.
	 */
	private final SimpleIntegerProperty index;
	/**
	 * The minSpeed is the speed down to which the test will let the car coast before burning.
	 */
	private final SimpleIntegerProperty minSpeed;
	/**
	 * If the speed is at or below the minSpeed, the test will let the car burn until it reaches maxSpeed.
	 */
	private final SimpleIntegerProperty maxSpeed;

	/**
	 * Construct a new TestValue representing 1 row in the test table
	 * @param index the row index of the value
	 * @param minSpeed the speed to coast down to
	 * @param maxSpeed the speed to burn up to
	 * @see com.dtucar.ecucp.controller.TestController
	 */
	public TestValue(int index, int minSpeed, int maxSpeed) {
		this.index = new SimpleIntegerProperty(index);
		this.minSpeed = new SimpleIntegerProperty(minSpeed);
		this.maxSpeed = new SimpleIntegerProperty(maxSpeed);
	}

	//region Getters and setters
	public int getIndex() {
		return index.get();
	}

	public int getMinSpeed() {
		return minSpeed.get();
	}

	public void setMinSpeed(int minSpeed) {
		this.minSpeed.set(minSpeed);
	}

	public int getMaxSpeed() {
		return maxSpeed.get();
	}

	public void setMaxSpeed(int maxSpeed) {
		this.maxSpeed.set(maxSpeed);
	}
	//endregion
}
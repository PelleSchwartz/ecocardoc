package com.dtucar.ecucp.model;

import javafx.beans.property.SimpleIntegerProperty;

/**
 * A LutValue represents 1 row in the LUT table for the engine map
 */
public class LutValue {
	/**
	 * The RPM
	 */
	private final SimpleIntegerProperty rpm;
	/**
	 * The injection timing [us] for gear 1
	 */
	private final SimpleIntegerProperty injection1;
	/**
	 * The injection timing [us] for gear 2
	 */
	private final SimpleIntegerProperty injection2;
	/**
	 * The ignition timing [CAD BTDC]
	 */
	private final SimpleIntegerProperty ignition;

	/**
	 * Construct a new LutValue representing 1 row in the LUT table for the engine map
	 * @param rpm the RPM of the row (acts as the key for this value)
	 * @param injection1 the injection time when in 1st gear
	 * @param injection2 the injection time when in 2nd gear
	 * @param ignition the ignition timing
	 */
	public LutValue(int rpm, int injection1, int injection2, int ignition) {
		this.rpm = new SimpleIntegerProperty(rpm);
		this.injection1 = new SimpleIntegerProperty(injection1);
		this.injection2 = new SimpleIntegerProperty(injection2);
		this.ignition = new SimpleIntegerProperty(ignition);
	}

	//region Getters and setters
	public int getRpm() {
		return rpm.get();
	}

	public int getInjection1() {
		return injection1.get();
	}

	public void setInjection1(int injection) {
		this.injection1.set(injection);
	}

	public int getInjection2() {
		return injection2.get();
	}

	public void setInjection2(int injection) {
		this.injection2.set(injection);
	}

	public int getIgnition() {
		return ignition.get();
	}

	public void setIgnition(int ignition) {
		this.ignition.set(ignition);
	}
	//endregion
}
package com.dtucar.ecucp.model;

import com.dtucar.ecucp.communication.TeensySync.EcuEnum;

/**
 * The ConfigValue class is a simple data model class for storing a {@link EcuEnum} paired with a {@link Number} value.
 * The Number can be any of the Number subclasses, such as float, int, long, etc.
 */
public class ConfigValue {
	private final EcuEnum ecuEnum;
	private Number value;

	/**
	 * Construct a new ConfigValue
	 * @param ecuEnum the key of the value
	 * @param value the value to be stored
	 */
	public ConfigValue(EcuEnum ecuEnum, Number value) {
		this.ecuEnum = ecuEnum;
		this.value = value;
	}

	/**
	 * Get the EcuEnum key value
	 * @return ecuEnum associated with the value
	 */
	public EcuEnum getEcuEnum() {
		return ecuEnum;
	}

	/**
	 * Get the config value
	 * @return value
	 */
	public Number getValue() {
		return value;
	}

	/**
	 * Set the config value (can be float, int, long...)
	 * @param value to be set
	 */
	public void setValue(Number value) {
		this.value = value;
	}
}

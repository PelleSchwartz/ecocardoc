package com.dtucar.ecucp.util;

public class Constants {
	/**
	 * General debug (mostly to enable/disable various println statements)
	 */
	public static final boolean DEBUG = true;
	/**
	 * The brake pressure threshold (brake pressures above this will be interpreted as braking)
	 */
	public static final int BRAKE_PRESSURE_THRESHOLD = 1;    // [bar]
	/**
	 * Key (for hash maps) for the source of data
	 */
	public static final String SOURCE_KEY = "SOURCE";
	/**
	 * Value (for hash maps) for the source of data when from a save file (as opposed to from communication)
	 */
	public static final String SOURCE_SAVE_AND_LOAD = "SAVE_AND_LOAD";
	/**
	 * Key (for hash maps) for the test log folder in a save file
	 */
	public static final String TEST_LOG_FOLDER_KEY = "TEST_LOG_DIR";
	/**
	 * Whether or not to show unexpected exceptions to the user in a dialog ({@link DialogUtil#showExceptionDialog(Throwable)})
	 */
	public static final boolean SHOW_EXCEPTIONS = true;
	/**
	 * The maximum number of characters in a text area (for example older characters in debug view will be removed if the number of characters exceeds this number)
	 */
	public static final int MAX_TEXT_IN_TEXT_AREA = 100000;
	/**
	 * The minimum amount of time to wait before sending another message to the serial port (see {@link com.dtucar.ecucp.communication.Communication}
	 */
	public static final int MINIMUM_SEND_WAIT_TIME = 1;    	// [ms]
	/**
	 * The period of the teensy sync thread (every x ms, the {@link com.dtucar.ecucp.communication.TeensySync} loop sends the next scheduled message)
	 */
	public static final int TEENSY_SYNC_PERIOD = 10;   		// [ms]
	/**
	 * The period of the main loop that calls the "loop()" method in all the controllers extending the {@link com.dtucar.ecucp.controller.BaseController}
	 */
	public static final int BASE_LOOP_PERIOD = 100;        	// [ms]
	/**
	 * The mask used in the ECU state (see STATE_ECU in canbusKeywords.h)
	 */
	public static final int STATE_ECU_ALIVE_MASK = 0x01;

	/**
	 * The TestState enum is used in the {@link com.dtucar.ecucp.controller.TestController} to indicate the state of the test
	 */
	public enum TestState {
		TEST_STOPPED,
		TEST_BURNING,
		TEST_COASTING
	}
}

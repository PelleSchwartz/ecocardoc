package com.dtucar.ecucp.communication;

import com.dtucar.ecucp.model.ConfigValue;
import com.dtucar.ecucp.model.LutValue;
import com.dtucar.ecucp.model.TestValue;
import com.dtucar.ecucp.util.AppLog;
import com.dtucar.ecucp.util.Constants;
import com.dtucar.ecucp.util.Locator;
import com.dtucar.ecucp.util.TableUtil;
import com.google.gson.*;
import com.google.gson.stream.JsonReader;
import com.google.gson.stream.JsonToken;
import com.google.gson.stream.MalformedJsonException;

import java.io.IOException;
import java.io.StringReader;
import java.util.*;

/**
 * The TeensySync class encompasses the logic for communicating with the ECU Teensy.<br/>
 * Requesting and sending configurations to/from the Teensy is done through this class, which buffers requests and converts them to JSON strings to send them through {@link Communication}.<br/>
 * The TeensySync class also contains the {@link #parseData(String)} function, which is called by <code>Communication</code> to parse incoming JSON data strings into a HashMap.
 */
public class TeensySync {
	private static final String TAG = "TeensySync";

	/**
	 * The EcuEnum contains all the keys used in the JSON exchange with the ECU.<br/>
	 * This should always been in sync with the defined values in PCSync.h in the ECU project.<br/>
	 * Every key must be unique (any duplicates in this enum will cause an exception).<br/>
	 * In addition to the keys used for ECU communication, a few extras have been added here
	 */
	public enum EcuEnum {
		// Status field always sent from Teensy (see STATE_ECU in canbusKeywords.h for bitmask)
		STATE("S"),
		// LUT fields (fields for the LUT configuration that will be saved to persistent memory)
		LUT_RPM("LRP"),
		LUT_INJECTION1("IN1"),
		LUT_INJECTION2("IN2"),
		LUT_IGNITION("IGN"),
		// Encoder fields (sent from Teensy on answer to ENCODER)
		ENC_NUM_PROBLEMS("ENP"),
		ENC_DISTANCE_0("ED0"),
		ENC_DISTANCE_1("ED1"),
		// Command fields (runtime commands to ECU that will not be saved to persistent memory)
		BURN("BURN"),
		IDLE("IDLE"),
		AUTOGEAR("AG"),
		NEUTRAL("NEU"),
		GEAR_1("G1"),
		GEAR_2("G2"),
		PARTY("PAR"),
		RESET_LOG("RSL"),
		RESET_DISTANCE("RSD"),
		RESET_FUEL_CONSUMPTION("RFC"),
		// Request fields (used to obtain data from Teensy, Teensy will reply with current value)
		LUT("LUT"),
		LOG("LOG"),
		CAN("CAN"),
		ENCODER("ENC"),
		RS232("RS2"),
		ENGINE_ENCODER_A_PULSE_COUNT("ENA"),
		ENGINE_ENCODER_B_PULSE_COUNT("ENB"),
		ENGINE_ENCODER_Z_PULSE_COUNT("ENZ"),
		STARTER("STA"),
		BATTERY_VOLTAGE("BAT"),
		DISTANCE("DIS"),
		SPEED("SPD"),
		RPM("RPM"),
		FUEL_CONSUMPTION("KML"),
		FUEL_CONSUMED("FUL"),
		LAMBDA("LAM"),
		OIL_TEMPERATURE("OIL"),
		WATER_TEMPERATURE("H2O"),
		EXHAUST_TEMPERATURE("EXH"),
		CORE_TEMPERATURE("CTP"),
		GEAR("GEA"),
		BRAKE_FRONT("BRF"),
		BRAKE_REAR("BRR"),
		// Configuration fields (Teensy will save these to persistent memory and reply with current value)
		FUEL_CORRECTION("F_C"),
		ENGINE_CALIBRATION("E_C"),
		MAX_RPM("M_R"),
		STARTER_LIMIT("S_L"),
		IDLE_LIMIT("I_L"),
		FUEL_TANK_VOLUME("F_T_V"),
		IGNITION_DWELL_TIME("I_D_T"),
		INJECTION_TIMING("I_T"),
		IDLE_INJECTION("I_I"),
		GEAR_CHANGE_WAIT("G_C_W"),
		GEAR_CHANGE_LIMIT("G_C_L"),
		GEAR_UP_SPEED("G_U_S"),
		GEAR_DOWN_SPEED("G_D_S"),
		GEAR_NEUTRAL_PWM("G_0_P"),
		GEAR_1_PWM("G_1_P"),
		GEAR_2_PWM("G_2_P"),
		FUEL_CONVERSION_SLOPE("F_C_S"),
		FUEL_CONVERSION_INTERCEPT("F_C_I"),
		LAMBDA_CONVERSION_SLOPE("L_C_S"),
		LAMBDA_CONVERSION_INTERCEPT("L_C_I"),
		FUEL_DENSITY("F_D"),
		FUEL_LOWER_HEATING_VALUE("F_L_H"),
		EQUIVALENT_FUEL_LOWER_HEATING_VALUE("E_F_L_H"),
		WHEEL_DIAMETER("W_D"),
		PULSES_PER_REVOLUTION("P_P_R"),
		TIME("TIME"),
		LOG_PERIOD("L_P"),
		// Values not sent to Teensy (used for local configuration save and load)
		TEST_VALUES("TEST"),
		TEST_VALUE_INDEX("TIND"),
		TEST_VALUE_MAX("TMAX"),
		TEST_VALUE_MIN("TMIN"),
		TEST_DISTANCE("TDIS");

		private final String name;

		EcuEnum(String name) {
			this.name = name;
		}

		/**
		 * This returns the string value of the enum entry: the key value
		 *
		 * @return the string value of the enum entry
		 */
		public String get() {
			return this.name;
		}

		@Override
		public String toString() {
			return this.name;
		}
	}

	/**
	 * The JSON object that allows for JSON parsing and constructing
	 */
	private static final Gson gson = new Gson();
	/**
	 * The request list is used to pool together requests, e.g. sent from multiple controller.
	 * For example, if both CockpitController and TestController requests the same value within the same {@link Constants#TEENSY_SYNC_PERIOD},
	 * the requests will be combined and only sent once.
	 */
	private static final List<EcuEnum> requestList = new ArrayList<>(EcuEnum.values().length);
	/**
	 * The config list is used to pool together configs, e.g. sent right after one another.
	 * For example, if CockpitController sends two fuel correction values within the same {@link Constants#TEENSY_SYNC_PERIOD},
	 * the configs will be combined and only the newest one will be sent.
	 */
	private static final List<ConfigValue> configList = new ArrayList<>(EcuEnum.values().length);
	/**
	 * Whether an answer was received from the last request.<br/>
	 * This is used to prevent clogging of the communication line.
	 * Since the Teensy always replies to all messages (with at least its status), we wait until we get an answer before sending more.
	 */
	private static boolean lastRequestAnswered = true;
	/**
	 * This is the period that TeensySync should wait after a log message has been received before executing any requests.
	 * Since the Teensy is busy sending the log file, any requests during this time would just clog the communication line.
	 */
	private static final int LOG_WAIT_PERIOD = 1000;
	/**
	 * The millis time since the last log message was received
	 */
	private static long lastLogReceivedTime = 0;

	/**
	 * This is called once on initialization to ensure that all strings in the {@link EcuEnum} are unique.
	 *
	 * @throws Exception if duplicates are found
	 */
	public static void validateEnums() throws Exception {
		Set<String> allItems = new HashSet<>();
		for(EcuEnum ecuEnum : EcuEnum.values()) {
			boolean result = allItems.add(ecuEnum.get());
			if(!result) throw new Exception("Duplicates found in EcuEnum. Each member must be unique!");
		}
	}

	/**
	 * The TeensySync loop is called from {@link com.dtucar.ecucp.controller.LeftController} in a separate thread every {@link Constants#TEENSY_SYNC_PERIOD} ms.
	 * It executes the sending of configs and requests from the {@link #configList} and {@link #requestList} if an answer has been received from the last execution.
	 */
	public static void loop() {
		if(!Locator.getCommunication().isConnected()) lastRequestAnswered = true;
		else if(lastRequestAnswered && !configList.isEmpty()) {
			// Pool configs together and execute only if answer received
			lastRequestAnswered = false;
			executeConfigs();
		} else if(lastRequestAnswered && !requestList.isEmpty() && System.currentTimeMillis() - lastLogReceivedTime >= LOG_WAIT_PERIOD) {
			// Pool requests together and execute only if answer received
			// Log wait period is used to keep as much bandwidth to the log transfer and not spam the Teensy while it is transferring the log file
			lastRequestAnswered = false;
			executeRequests();
		}
	}

	/**
	 * <p>When connected, this adds a list of config values to the {@link #configList} to schedule them for sending within the next {@link Constants#TEENSY_SYNC_PERIOD} ms.<br/>
	 * If the list already contains a scheduled config for the same <code>EcuEnum</code>, the existing one will be overridden by this new one.</p>
	 * Note: This is an asynchronous function and returns almost immediately. It should not be called too fast, otherwise the communication may not be able to empty the buffer
	 * fast enough and there will be a lag in communicating with the Teensy.
	 *
	 * @param configs the configs to schedule for sending
	 */
	public static void sendConfig(ConfigValue... configs) {
		if(configs == null || !Locator.getCommunication().isConnected()) return;
		for(int i = 0; i < configs.length; i++) {
			final ConfigValue config = configs[i];
			synchronized(configList) {
				Optional<ConfigValue> existingConfig = configList.stream().filter(c -> c.getEcuEnum() == config.getEcuEnum()).findAny();
				if(existingConfig.isPresent()) existingConfig.get().setValue(config.getValue());
				else if(!configList.contains(configs[i])) configList.add(config);
				if(configList.size() > 100) AppLog.error(TAG + ": Config buffer is very big (>100). Are you sending too fast?");
			}
		}
	}

	/**
	 * <p>When connected, this adds a list of request values to the {@link #requestList} to schedule them for sending within the next {@link Constants#TEENSY_SYNC_PERIOD} ms.<br/>
	 * If the list already contains a scheduled config request for the same <code>EcuEnum</code>, no new one will be added (since it's the exact same request)</p>
	 * Note: This is an asynchronous function and returns almost immediately. It should not be called too fast, otherwise the communication may not be able to empty the buffer
	 * fast enough and there will be a lag in communicating with the Teensy.
	 *
	 * @param types the configs to schedule for requesting
	 */
	public static void requestConfig(EcuEnum... types) {
		if(types == null || !Locator.getCommunication().isConnected()) return;
		for(int i = 0; i < types.length; i++) {
			final EcuEnum type = types[i];
			synchronized(requestList) {
				if(requestList.stream().noneMatch(t -> t == type)) requestList.add(type);
				if(requestList.size() > 100) AppLog.error(TAG + ": Request buffer is very big (>100). Are you requesting too fast?");
			}
		}
	}

	/**
	 * This executes the sending of the pooled config values in {@link #configList} and constructs a JSON string to be sent to {@link Communication#send(String)}.<br/>
	 * This should only be called from {@link #loop()}.
	 */
	private static void executeConfigs() {
		String message;
		synchronized(configList) {
			JsonArray array = new JsonArray(configList.size());
			for(int i = 0; i < configList.size(); i++) {
				if(configList.get(i) != null && configList.get(i).getEcuEnum() != null) {
					JsonObject object = new JsonObject();
					object.addProperty(configList.get(i).getEcuEnum().get(), configList.get(i).getValue());
					array.add(object);
				}
			}
			configList.clear();
			message = gson.toJson(array);
		}
		Locator.getCommunication().send(message);
	}

	/**
	 * This executes the sending of the pooled requested config values in {@link #requestList} and constructs a JSON string to be sent to {@link Communication#send(String)}.<br/>
	 * This should only be called from {@link #loop()}.
	 */
	private static void executeRequests() {
		String message;
		synchronized(requestList) {
			JsonArray array = new JsonArray(requestList.size());
			for(int i = 0; i < requestList.size(); i++) {
				if(requestList.get(i) != null) {
					JsonObject object = new JsonObject();
					object.addProperty(requestList.get(i).get(), "");
					array.add(object);
				}
			}
			requestList.clear();
			message = gson.toJson(array);
		}
		Locator.getCommunication().send(message);
	}

	/**
	 * "Emergency stop"<br/>
	 * This immediately stops burn and idle and calls {@link Communication#stop(String)} to halt all communication.
	 */
	public static void stop() {
		ConfigValue[] configs = {new ConfigValue(EcuEnum.BURN, 0), new ConfigValue(EcuEnum.IDLE, 0)};
		JsonArray array = new JsonArray(configs.length);
		for(ConfigValue config : configs) {
			JsonObject object = new JsonObject();
			object.addProperty(config.getEcuEnum().get(), config.getValue());
			array.add(object);
		}
		String message = gson.toJson(array);
		Locator.getCommunication().stop(message);
	}

	/**
	 * Clear all values in the {@link #configList} and {@link #requestList}.
	 */
	public static void clearBuffers() {
		synchronized(configList) {
			configList.clear();
		}
		synchronized(requestList) {
			requestList.clear();
		}
	}

	/**
	 * This constructs a JSON string from the LUT values and sends it to {@link Communication#send(String)}.<br/>
	 * No pooling is done on this, since it is only called when a single value is changed on the array (and you can't do that fast enough to cause any problems),
	 * or when a saved configuration file is loaded, in which case all are sent at once (already pooled).
	 *
	 * @param lutValues the LUT values to send
	 */
	public static void sendLutUpdate(LutValue... lutValues) {
		String message = gson.toJson(TableUtil.lutValuesToJsonArray(lutValues));
		Locator.getCommunication().send(message);
	}

	/**
	 * The big parse function!<br/>
	 * This function is called when:<br/>
	 * - A new message has been received from Communication<br/>
	 * - A saved configuration file is loaded<br/>
	 * The function cleans up the message, which is expected in JSON format, and then stores the result in a hash map with the keys from {@link EcuEnum} for easy processing later by the data received listener.<br/>
	 * Any data that can not be parsed as JSON is sent to the {@link AppLog}.<br/>
	 * The JSON string has an array as the parent element, and then any number of objects inside. For example: [{"F_C":1.0,"KML":374.2,"GEA":1}]
	 *
	 * @param data the string to be parsed
	 * @return the result of the parsing in a hash map (or null if no valid JSON could be found)
	 */
	public static List<HashMap<String, Object>> parseData(String data) {
		if(Constants.DEBUG) System.out.println(TAG + " received: " + data);
		lastRequestAnswered = true;
		// Clean up JSON (it doesn't like NaN or Infinity)
		data = data.replaceAll(":NaN", ":-1");
		data = data.replaceAll(":Infinity", ":-1");
		// The following lines are all "clean-up" functions. They ensure that we parse all the valid JSON, even if we receive some garbage in between
		// For example, in the following string, both JSON packets would be correctly extracted: <<Debug1[{"KML":374.2}]qwerty[{"GEA":1}]Debug2>>
		// and the debug message would be sent to the log.
		// Check for at least one valid start of a JSON array
		int startIndex = data.indexOf('[');
		if(startIndex < 0) {
			// The entire string does not contain a JSON array
			AppLog.log("RX (no valid '['): " + data);
			return null;
		} else if(startIndex > 0) {
			// There is extra data not in the JSON array at the beginning of the string
			// In the example above, that would be <<Debug1>>
			AppLog.log("RX (extra data start): " + data.substring(0, startIndex));
		}
		// Find the last index of the end of a JSON array
		int endIndex = data.lastIndexOf(']');
		if(endIndex < 0) {
			// The entire string does not contain a JSON array
			AppLog.log("RX (no valid ']' after " + startIndex + "): " + data);
			return null;
		} else if(endIndex <= startIndex) {
			// The location of the end tag is before the start tag
			AppLog.log("RX (']' at " + endIndex + " not after '[' at " + startIndex + "): " + data);
			return null;
		} else if(endIndex < data.length() - 1 && !data.substring(endIndex + 1).trim().isEmpty()) {
			// There is extra data not in the JSON array at the end of the string
			// In the example above, that would be <<Debug2>>
			AppLog.log("RX (extra data end): " + data.substring(endIndex + 1));
		}
		// Get the part of the string that starts with "[" and ends with "]"
		// In the example above, we would now have <<[{"KML":374.2}]SomeDebugMessage[{"GEA":1}]>>
		data = data.substring(startIndex, endIndex + 1);
		// This regex replace all occurrences of ]*[ (where * is any number of characters) with ]$[
		// In the example above, we would now have <<[{"KML":374.2}]$[{"GEA":1}]>>
		data = data.replaceAll("](?s).*\\[", "]\\$[");
		// Then the string is split into an array of strings, which now should be valid JSON arrays
		// In the example above, we would now have <<[{"KML":374.2}]>> and <<[{"GEA":1}]>> in the array
		String[] jsonStrings = data.split("\\$");
		List<HashMap<String, Object>> hashMapList = new ArrayList<>();
		// Loop through all the JSON strings we have found (most commonly, there is only 1)
		for(int i = 0; i < jsonStrings.length; i++) {
			String jsonString = jsonStrings[i];
			// Parse the JSON string
			try {
				// Check if the string is valid JSON
				if(!isJsonValid(jsonString)) {
					AppLog.log("RX (no valid JSON): " + jsonString);
					return null;
				}
				// Parse JSON
				JsonElement element = new JsonParser().parse(jsonString);
				// This should be an array
				if(!element.isJsonArray()) {
					AppLog.log("RX (no JSON array): " + jsonString);
					return null;
				}
				// We now have our top JSON array will all the good stuff inside
				JsonArray array = element.getAsJsonArray();
				// Initialize storage lists (not all of these will be used, so we check if they're empty at the end)
				HashMap<String, Object> hashMap = new HashMap<>(array.size());
				List<LutValue> lutValueList = new ArrayList<>();
				List<TestValue> testValueList = new ArrayList<>();
				List<String> logStringList = new ArrayList<>();
				// Loop through all the elements in the JSON array
				for(int j = 0; j < array.size(); j++) {
					// Every element in the array should be a JSON object, if not, something is wrong and we skip it
					if(!array.get(j).isJsonObject()) {
						AppLog.log("RX (no JSON object): " + array.get(j).toString());
						continue;
					}
					// Get the JSON object
					JsonObject object = array.get(j).getAsJsonObject();
					// Now begins the parsing into the HashMap:
					// We have several "special need" values that are treated first (e.g. they are not simple object with one value or they have string values).
					// Most normal configs (a key and a number value e.g. {"E_C":56.0}) are simply assigned directly into the HashMap (second to last case).
					if(object.has(EcuEnum.LUT_RPM.get()) && object.has(EcuEnum.LUT_INJECTION1.get()) && object.has(EcuEnum.LUT_INJECTION2.get()) && object.has(EcuEnum.LUT_IGNITION.get())) {
						// LUT values come in a quadruple int package (e.g. {"LRP":1000,"INJ1":6521,"INJ2":4321,"IGN":30})
						lutValueList.add(new LutValue(object.get(EcuEnum.LUT_RPM.get()).getAsInt(), object.get(EcuEnum.LUT_INJECTION1.get()).getAsInt(), object.get(EcuEnum.LUT_INJECTION2.get()).getAsInt(), object.get(EcuEnum.LUT_IGNITION.get()).getAsInt()));
					} else if(object.has(EcuEnum.TEST_VALUE_INDEX.get()) && object.has(EcuEnum.TEST_VALUE_MIN.get()) & object.has(EcuEnum.TEST_VALUE_MAX.get())) {
						// TEST_VALUES values (from a loaded saved configuration file) come in a triple package (e.g. {"TIND":0,"TMIN":12,"TMAX":25})
						testValueList.add(new TestValue(object.get(EcuEnum.TEST_VALUE_INDEX.get()).getAsInt(), object.get(EcuEnum.TEST_VALUE_MIN.get()).getAsInt(), object.get(EcuEnum.TEST_VALUE_MAX.get()).getAsInt()));
					} else if(object.has(EcuEnum.ENC_NUM_PROBLEMS.get()) && object.has(EcuEnum.ENC_DISTANCE_0.get()) && object.has(EcuEnum.ENC_DISTANCE_1.get())) {
						// ENC values com in a triple package with multiple values (e.g. {"ENP":0,"ED0":24,"ED1":25})
						hashMap.put(EcuEnum.ENC_NUM_PROBLEMS.get(), object.get(EcuEnum.ENC_NUM_PROBLEMS.get()).getAsString());
						hashMap.put(EcuEnum.ENC_DISTANCE_0.get(), object.get(EcuEnum.ENC_DISTANCE_0.get()).getAsString());
						hashMap.put(EcuEnum.ENC_DISTANCE_1.get(), object.get(EcuEnum.ENC_DISTANCE_1.get()).getAsString());
					} else if(object.has(EcuEnum.LOG.get())) {
						// LOG file (actually just the beginning of the log file, since the rest is not parsed and sent directly as raw non-JSON string for faster transfer)
						lastLogReceivedTime = System.currentTimeMillis();
						logStringList.add(object.get(EcuEnum.LOG.get()).getAsString());
					} else if(object.has(EcuEnum.CAN.get())) {
						// CAN state (a long string)
						hashMap.put(EcuEnum.CAN.get(), object.get(EcuEnum.CAN.get()).getAsString());
					} else if(object.has(EcuEnum.RS232.get())) {
						// RS232 state (a long string)
						hashMap.put(EcuEnum.RS232.get(), object.get(EcuEnum.RS232.get()).getAsString());
					} else if(object.has(Constants.TEST_LOG_FOLDER_KEY)) {
						// Test folder location loaded from the saved configuration file (a string)
						hashMap.put(Constants.TEST_LOG_FOLDER_KEY, object.get(Constants.TEST_LOG_FOLDER_KEY).getAsString());
					} else if(object.keySet().size() == 1) {
						// NORMAL config and data packages have one key and one number value (e.g. {"E_C":56.0}) and are put directly into the hashMap
						String key = object.keySet().iterator().next();
						if(!object.get(key).isJsonNull()) hashMap.put(key, object.get(key).getAsNumber());
					} else {
						// An object that we did not expect
						AppLog.log(TAG + " did not parse: " + object);
					}
				}
				// If we have received some of the special values, put them into the main hashMap
				if(!lutValueList.isEmpty()) hashMap.put(EcuEnum.LUT.get(), lutValueList);
				if(!testValueList.isEmpty()) hashMap.put(EcuEnum.TEST_VALUES.get(), testValueList);
				if(!logStringList.isEmpty()) hashMap.put(EcuEnum.LOG.get(), logStringList);
				if(Constants.DEBUG) System.out.println(TAG + " parsed: " + Arrays.toString(hashMap.entrySet().toArray()));
				// Add our result to the hashMapList which will be returned once we have parsed all our arrays (of which there is probably only 1)
				hashMapList.add(hashMap);
			} catch(Exception e) {
				// Well this is embarrassing. We did all that hard work to ensure the JSON was valid and as we expected it, and still got an exception!
				// This should not happen.
				AppLog.error("JSON exception: " + jsonString);
				e.printStackTrace();
			}
		}
		return hashMapList;
	}

	/**
	 * Check if string is valid JSON
	 *
	 * @param json the JSON string
	 * @return true if JSON valid
	 * @throws IOException if something goes wrong
	 */
	private static boolean isJsonValid(final String json) throws IOException {
		final JsonReader jsonReader = new JsonReader(new StringReader(json));
		try {
			JsonToken token;
			while((token = jsonReader.peek()) != JsonToken.END_DOCUMENT && token != null) {
				skipToken(jsonReader);
			}
			return true;
		} catch(final MalformedJsonException ignored) {
			return false;
		}
	}

	/**
	 * Utility function to skip a JSON token when checking for valid JSON
	 *
	 * @param reader the jsonReader
	 * @throws IOException if something goes wrong
	 */
	private static void skipToken(final JsonReader reader) throws IOException {
		final JsonToken token = reader.peek();
		switch(token) {
			case BEGIN_ARRAY:
				reader.beginArray();
				break;
			case END_ARRAY:
				reader.endArray();
				break;
			case BEGIN_OBJECT:
				reader.beginObject();
				break;
			case END_OBJECT:
				reader.endObject();
				break;
			case NAME:
				reader.nextName();
				break;
			case STRING:
			case NUMBER:
			case BOOLEAN:
			case NULL:
				reader.skipValue();
				break;
			case END_DOCUMENT:
			default:
				throw new AssertionError(token);
		}
	}
}

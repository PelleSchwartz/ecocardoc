package com.dtucar.ecucp.communication;

import com.dtucar.ecucp.util.Locator;

import java.util.HashMap;

/**
 * The data received listener is an interface for parsed data stored in a hash map with the keys from {@link TeensySync.EcuEnum}.
 *
 * @see TeensySync#parseData(String)
 * @see Locator#registerDataListener(DataReceivedListener)
 */
public interface DataReceivedListener {
	/**
	 * <p>Will be called by {@link Communication} through the {@link Locator} when new received data has been successfully parsed by {@link TeensySync#parseData(String)}.
	 * The keys for the hash map are from {@link TeensySync.EcuEnum}.</p>
	 * Note: This will not be called from the UI thread. Any UI manipulation needs to use {@link javafx.application.Platform#runLater(Runnable)}.
	 *
	 * @param data the hash map of the new data received
	 */
	void onDataReceived(HashMap<String, Object> data);
}

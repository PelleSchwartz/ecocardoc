package com.dtucar.ecucp.util;

import com.dtucar.ecucp.Main;
import com.dtucar.ecucp.communication.Communication;
import com.dtucar.ecucp.communication.DataReceivedListener;
import com.dtucar.ecucp.communication.MessageReceivedListener;
import com.dtucar.ecucp.communication.TeensySync;
import com.dtucar.ecucp.controller.MainController;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.ResourceBundle;

/**
 * This static class acts as a locator for application-wide singleton classes.<br>
 * It is a utility to prevent having to pass around global instances.
 */
public class Locator {
	private static Communication communication;
	private static Main mainApp;
	private static MainController mainController;
	private static ResourceBundle resources;
	private static List<DataReceivedListener> dataReceivedListenerList;
	private static List<MessageReceivedListener> messageReceivedListenerList;

	/**
	 * Initialize the references (this should only be called once from Main)
	 *
	 * @param mainApp reference to {@link Main}
	 * @throws Exception if {@link TeensySync#validateEnums()} throws an exception
	 */
	public static void initialize(Main mainApp) throws Exception {
		Locator.mainApp = mainApp;
		dataReceivedListenerList = new ArrayList<>(10);
		messageReceivedListenerList = new ArrayList<>(10);
		communication = new Communication();
		TeensySync.validateEnums();
	}

	/**
	 * Get the {@link Communication} reference
	 *
	 * @return communication
	 */
	public static Communication getCommunication() {
		return communication;
	}

	/**
	 * Get the {@link Main} reference
	 *
	 * @return main
	 */
	public static Main getMainApp() {
		return mainApp;
	}

	/**
	 * Set the {@link ResourceBundle} reference (only called from Main)
	 *
	 * @param resources resources
	 */
	public static void setResources(ResourceBundle resources) {
		Locator.resources = resources;
	}

	/**
	 * Get the {@link ResourceBundle} reference
	 *
	 * @return resources
	 */
	public static ResourceBundle getResources() {
		return resources;
	}

	/**
	 * Get the main controller
	 *
	 * @return mainController
	 */
	public static MainController getMainController() {
		return mainController;
	}

	/**
	 * Set the main controller
	 *
	 * @param mainController mainController
	 */
	public static void setMainController(MainController mainController) {
		Locator.mainController = mainController;
	}

	/**
	 * Dispose of all references (called from Main onStop)
	 */
	public static void dispose() {
		if(communication != null) communication.disconnect();
		resources = null;
		mainApp = null;
	}

	/**
	 * Send to all registered dataReceivedListeners (called from Communication after TeensySync has parsed the data into a HashMap)
	 *
	 * @param hashMap the hashMap containing the data
	 */
	public static void sendToDataReceivedListeners(HashMap<String, Object> hashMap) {
		if(hashMap != null) {
			for(int i = 0; i < dataReceivedListenerList.size(); i++) {
				dataReceivedListenerList.get(i).onDataReceived(hashMap);
			}
		}
	}

	/**
	 * Register a dataReceivedListener to be notified when new data is received (from Communication parsed through TeensySync)
	 *
	 * @param dataReceivedListener the listener
	 */
	public static void registerDataListener(DataReceivedListener dataReceivedListener) {
		if(!dataReceivedListenerList.contains(dataReceivedListener)) {
			dataReceivedListenerList.add(dataReceivedListener);
		}
	}

	/**
	 * Unregister a dataReceivedListener
	 *
	 * @param dataReceivedListener the listener
	 */
	public static void unregisterDataListener(DataReceivedListener dataReceivedListener) {
		dataReceivedListenerList.remove(dataReceivedListener);
	}

	/**
	 * Send to all registered messageReceivedListeners (called from Communication with the raw string message received)
	 *
	 * @param message the message
	 */
	public static void sendToMessageReceivedListeners(String message) {
		if(message != null) {
			for(int i = 0; i < messageReceivedListenerList.size(); i++) {
				messageReceivedListenerList.get(i).onMessageReceived(message);
			}
		}
	}

	/**
	 * Register a messageReceivedListener to be notified when a new message has been received (raw from Communication)
	 *
	 * @param messageReceivedListener the listener
	 */
	public static void registerMessageListener(MessageReceivedListener messageReceivedListener) {
		if(!messageReceivedListenerList.contains(messageReceivedListener)) {
			messageReceivedListenerList.add(messageReceivedListener);
		}
	}

	/**
	 * Unregister the messagerReceivedListener
	 *
	 * @param messageReceivedListener the listener
	 */
	public static void unregisterMessageListener(MessageReceivedListener messageReceivedListener) {
		messageReceivedListenerList.remove(messageReceivedListener);
	}
}

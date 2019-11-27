package com.dtucar.ecucp.util;

import com.dtucar.ecucp.communication.MessageReceivedListener;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.List;

/**
 * The AppLog class is a static class used to log certain events.<br>
 * Any class can call the {@link #log(String)} and {@link #error(String)} methods, which will notify the log listeners (currently just the {@link com.dtucar.ecucp.controller.LeftController}
 * which displays all these log events in the left view.
 * The {@link #logToFile(String)} method is currently only called from {@link DialogUtil#showExceptionDialog(Throwable)} when an unhandled exception is caught and the exception stack trace is saved to the app log file.
 */
public class AppLog {
	/**
	 * The prefix to prepend to error messages when sent to the log listeners
	 */
	private static final String ERROR_PREFIX = "{ERROR} ";
	/**
	 * The list of log listeners to notify
	 */
	private static List<MessageReceivedListener> logListeners;

	/**
	 * Log a message (println and notify log listeners)
	 *
	 * @param string the string to log
	 */
	public static void log(String string) {
		System.out.println(string);
		for(MessageReceivedListener listener : logListeners) {
			listener.onMessageReceived(string);
		}
	}

	/**
	 * Log an error (same as {@link #log(String)} except {@link #ERROR_PREFIX} is prepended to the string sent to the listeners)
	 *
	 * @param string the string to log
	 */
	public static void error(String string) {
		System.err.println(string);
		for(MessageReceivedListener listener : logListeners) {
			listener.onMessageReceived(ERROR_PREFIX + string);
		}
	}

	/**
	 * Register a log listener. This listener will be notified whenever {@link #log(String)} or {@link #error(String)} is called.
	 *
	 * @param listener the log listener
	 */
	public static void registerListener(MessageReceivedListener listener) {
		if(logListeners == null) logListeners = new ArrayList<>();
		if(!logListeners.contains(listener)) {
			logListeners.add(listener);
		}
	}

	/**
	 * Append the string to the "ECU_UI_AppLog.txt" file saved in the same directory as the program is executed in.
	 * If the file does not exist, it is created. The current time stamp is also prepended to the string.
	 *
	 * @param string the string to log to the app log file
	 */
	public static void logToFile(String string) {
		try {
			File logFile = new File("." + File.separator + "ECU_UI_AppLog.txt");
			OutputStreamWriter writer = new OutputStreamWriter(new FileOutputStream(logFile, true), "UTF-8");
			DateTimeFormatter dtf = DateTimeFormatter.ofPattern("yyyy-MM-dd HH-mm-ss");
			LocalDateTime now = LocalDateTime.now();
			String time = "########## " + dtf.format(now) + " ##########" + System.lineSeparator();
			writer.write(time);
			writer.write(string);
			writer.flush();
			writer.close();
		} catch(IOException e) {
			AppLog.error("Error in logToFile");
			e.printStackTrace();
		}
	}

	/**
	 * Unregister a listener
	 *
	 * @param listener the listener to unregister
	 */
	public void unregisterListener(MessageReceivedListener listener) {
		if(listener != null) logListeners.remove(listener);
	}
}

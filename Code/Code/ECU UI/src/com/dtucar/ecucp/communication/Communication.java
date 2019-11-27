package com.dtucar.ecucp.communication;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.dtucar.ecucp.util.AppLog;
import com.dtucar.ecucp.util.Callback;
import com.dtucar.ecucp.util.Constants;
import com.dtucar.ecucp.util.Locator;
import com.fazecast.jSerialComm.*;
import javafx.concurrent.Task;

import static java.lang.Thread.sleep;

/**
 * The Communication class is the master class responsible for the communication between the ECU and the UI.
 * It is the lowest level of the communication classes in this application. This is where bytes are sent to and received from the serial ports.
 */
public class Communication {
	private static final String TAG = "Communication";

	/**
	 * The communication can occur over different channel types.<br/>
	 * Currently, only USB serial communication is implemented.
	 */
	public enum CommunicationType {
		/**
		 * Standard serial connection through USB
		 */
		SERIAL,
		/**
		 * Communication through Ethernet or WiFi (currently not implemented)
		 */
		@SuppressWarnings("unused")
		TCP_IP,
		/**
		 * Communication using Bluetooth (currently not implemented)
		 */
		@SuppressWarnings("unused")
		BLUETOOTH,
	}

	/**
	 * The currently selected type of communication
	 */
	private CommunicationType communicationType;
	/**
	 * The serial port object for the USB serial communication
	 */
	private SerialPort serialPort;
	/**
	 * Whether or not we are connected to the ECU
	 */
	private boolean isConnected;
	/**
	 * The STOP flag can be used to halt all communication.
	 * The sendBuffer will be emptied first (to allow sending burn=0) and then the send thread will terminate.
	 */
	private boolean stop;
	/**
	 * The disableParsing flag is used only when fetching the log file in {@link com.dtucar.ecucp.controller.LogController}.
	 * When true, this will only send the message to the messageReceivedListeners (see {@link Locator#sendToMessageReceivedListeners(String)})
	 * and not send the received message to {@link TeensySync} and the dataReceivedListeners as usual.
	 */
	private boolean disableParsing;
	/**
	 * The send buffer is used to temporarily store the strings and schedule them for sending by the send thread.
	 * The send thread is set to send the oldest string in this list every {@link Constants#MINIMUM_SEND_WAIT_TIME} ms.
	 * Pooling of commands occurs before the string messages are sent to the communication (in {@link TeensySync}).
	 */
	private final List<String> sendBuffer = new ArrayList<>(10);
	/**
	 * Since sending occurs on a separate thread than the one adding to the send buffer, a lock object needs to be used,
	 * to synchronize and prevent concurrent modification on the send buffer.<br/>
	 * The send thread removes and send the oldest string of the send buffer list, shifting all the other elements in the list.
	 * Without synchronization, if send is called at the same time, an exception would be thrown.
	 */
	private final Object sendBufferLock = new Object();

	/**
	 * Get the currently selected communication type.
	 *
	 * @return the communication type
	 */
	@SuppressWarnings("unused")
	public CommunicationType getCommunicationType() {
		return communicationType;
	}

	/**
	 * Connect to the serial port without any callback.
	 * This is equivalent to calling {@link #connectSerial(String, Callback)} with callback null.
	 *
	 * @param port the serial port descriptor (Windows: COM[*], Linux: /dev/tty[*])
	 */
	@SuppressWarnings("unused")
	public void connectSerial(String port) {
		connectSerial(port, null);
	}

	/**
	 * Connect to the serial port.<br/>
	 * This establishes the connection, starts the send thread and registers the data listener which is called when data is received.<br/>
	 * This is an asynchronous operation. The call to this function returns immediately. The connection result will be available through the callback function.
	 *
	 * @param port     the serial port descriptor (Windows: COM[*], Linux: /dev/tty[*])
	 * @param callback will be called with the connection result (if successful "true" else "false")
	 */
	public void connectSerial(String port, Callback callback) {
		communicationType = CommunicationType.SERIAL;
		stop = false;
		disableParsing = false;
		// Establishing the connection is done in a separate thread
		// This thread also creates another thread responsible for sending data
		Task connectionTask = new Task<Void>() {
			@Override
			protected Void call() {
				// Connect to serial
				serialPort = SerialPort.getCommPort(port);
				isConnected = serialPort.openPort();
				// Set timeouts
				serialPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING | SerialPort.TIMEOUT_WRITE_BLOCKING, 100, 100);
				// Add the listener, which is called by the serial port when data is received
				serialPort.addDataListener(new SerialPortDataListener() {
					@Override
					public int getListeningEvents() {
						return SerialPort.LISTENING_EVENT_DATA_AVAILABLE;
					}

					// This is called when new data is received from the serial port
					@Override
					public void serialEvent(SerialPortEvent serialPortEvent) {
						if(serialPortEvent.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) return;
						int bytesAvailable = serialPort.bytesAvailable();
						if(bytesAvailable <= 0) return;
						// The new data received (in bytes)
						byte[] newData = new byte[bytesAvailable];
						int numRead = serialPort.readBytes(newData, newData.length);
						if(numRead != newData.length) {
							AppLog.error("ReadBytes unsuccessful");
							return;
						}
						// The new data received (as a String)
						String receiveString = new String(newData);
						// Send the (raw) string to messageReceivedListeners
						Locator.sendToMessageReceivedListeners(receiveString);
						if(!disableParsing) {
							// When parsing is enabled, call TeensySync to parse the data
							List<HashMap<String, Object>> hashMapList = TeensySync.parseData(receiveString);
							// Then, if parsing is successful, send the parsed data to the dataReceivedListeners
							// Since we could have received several JSON packages at once, we parse each package as a separate hash map and send each hash map to the listeners
							if(hashMapList != null) {
								for(int i = 0; i < hashMapList.size(); i++) {
									if(hashMapList.get(i) != null) {
										hashMapList.get(i).put(Constants.SOURCE_KEY, communicationType.toString());
										Locator.sendToDataReceivedListeners(hashMapList.get(i));
									}
								}
							}
						}
					}
				});
				// Call the callback with the connection result (as a string)
				if(callback != null) callback.callback(String.valueOf(isConnected));
				// Start the send thread
				if(isConnected()) {
					Task sendTask = new Task<Void>() {
						@Override
						protected Void call() {
							try {
								sleep(Constants.MINIMUM_SEND_WAIT_TIME);
							} catch(InterruptedException e) {
								e.printStackTrace();
							}
							// Keep the thread running as long as we're connected
							while(isConnected()) {
								if(communicationType == CommunicationType.SERIAL) {
									synchronized(sendBufferLock) {
										// For every string in the send buffer
										for(int i = 0; i < sendBuffer.size(); i++) {
											// Send the bytes to the serial port
											byte[] bytes = sendBuffer.get(i).getBytes();
											int result = serialPort.writeBytes(bytes, bytes.length);
											if(result != bytes.length) AppLog.error("WriteBytes unsuccessful: " + result);
											if(Constants.DEBUG) System.out.println(TAG + " sent: " + sendBuffer.get(i));
											// To prevent blocking the port with too much data, wait a little bit before sending the next message
											try {
												sleep(Constants.MINIMUM_SEND_WAIT_TIME);
											} catch(InterruptedException e) {
												e.printStackTrace();
											}
										}
										// Once all data has been sent, clear the buffer
										sendBuffer.clear();
									}
								}
								// If STOP has been pressed and the last message is out, end the thread
								if(stop && sendBuffer.isEmpty()) return null;
							}
							return null;
						}
					};
					// The send thread keeps running
					Thread sendThread = new Thread(sendTask);
					sendThread.setDaemon(true);
					sendThread.start();
				}
				return null;
			}
		};
		// The connection thread establishes the connection and starts the send thread, then exits (receiving is done as a callback)
		Thread connectionThread = new Thread(connectionTask);
		connectionThread.setDaemon(true);
		connectionThread.start();
	}

	/**
	 * Whether or not we are connected to the ECU
	 *
	 * @return true when we are connected
	 */
	public boolean isConnected() {
		if(communicationType == CommunicationType.SERIAL && serialPort != null) return serialPort.isOpen();
		return isConnected;
	}

	/**
	 * Disconnect the communication: Closes the communication port, ending the send thread and clearing the send buffer.
	 * This is an asynchronous command: Reconnection may not be immediately available (should wait ~500ms).
	 */
	public void disconnect() {
		if(serialPort != null && serialPort.isOpen()) serialPort.closePort();
		isConnected = false;
		sendBuffer.clear();
	}

	/**
	 * Adds the data to the send buffer, scheduling it for immediate sending (which occurs every {@link Constants#MINIMUM_SEND_WAIT_TIME} ms).<br/>
	 * The data will be ignored if communication is not connected or {@link #stop(String)} has been called.
	 *
	 * @param data string to be sent
	 */
	public void send(final String data) {
		if(!isConnected() || stop) return;
		synchronized(sendBufferLock) {
			sendBuffer.add(data);
			if(sendBuffer.size() > 10) AppLog.error(TAG + ": SendBuffer is very big (>10). Are you sending too fast?");
		}
	}

	/**
	 * The STOP flag can be used to halt all communication.
	 * One last data message can be sent (to allow sending burn=0) and then the send thread will terminate.
	 *
	 * @param data the last message to send before terminating
	 */
	public void stop(final String data) {
		if(!isConnected()) return;
		stop = true;
		synchronized(sendBufferLock) {
			sendBuffer.clear();
			sendBuffer.add(data);
		}
	}

	/**
	 * When the disableParsing flag is true, received messages will only go to the messageReceivedListeners
	 * and not be parsed through <code>TeensySync</code>, so the dataReceivedListeners will not be called.
	 *
	 * @param disableParsing whether or not to disable parsing
	 */
	public void setDisableParsing(boolean disableParsing) {
		this.disableParsing = disableParsing;
	}
}
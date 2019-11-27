package com.dtucar.ecucp.communication;

/**
 * This <code>MessageReceivedListener</code> is a generic interface used when String messages are received and transmitted to listeners.<br/>
 * It's main use in this application is for notifying of new raw string data received from {@link Communication} and for the listener on the {@link com.dtucar.ecucp.util.AppLog}.
 *
 * @see com.dtucar.ecucp.util.Locator#registerMessageListener(MessageReceivedListener)
 * @see com.dtucar.ecucp.util.AppLog#registerListener(MessageReceivedListener)
 */
public interface MessageReceivedListener {
	/**
	 * <p>Will be called to notify of a new message.</p>
	 * Note: This can be called from any thread. Any UI manipulation in this method needs to use {@link javafx.application.Platform#runLater(Runnable)}.
	 *
	 * @param message the string received
	 */
	void onMessageReceived(String message);
}

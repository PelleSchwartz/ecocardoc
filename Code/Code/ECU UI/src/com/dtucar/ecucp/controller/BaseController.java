package com.dtucar.ecucp.controller;

import com.dtucar.ecucp.util.Constants;
import com.dtucar.ecucp.communication.DataReceivedListener;
import com.dtucar.ecucp.util.Locator;

import java.util.Timer;
import java.util.TimerTask;

/**
 * The abstract <code>BaseController</code> class acts as the underlying class behind all the Controllers associated with tabs
 * (that means all controllers except the {@link MainController}).<br>
 * It handles the code that needs to run on all tabs:<br>
 * - Register a <code>DataReceivedListener</code> when it is being shown<br>
 * - Start the timer that calls {@link #loop()} every {@link Constants#BASE_LOOP_PERIOD} ms when shown<br>
 * - Unregister the <code>DataReceivedListener</code> when it is being hidden<br>
 * - Stop the timer when it is being hidden<br>
 * Since it implemented <code>DataReceivedListener</code>, any super class should implement that listener.<br>
 * The {@link #show()} and {@link #hide()} functions are called from the {@link MainController}.<br>
 * If a new tab is added to the application, it should extend this class and its hide and show functions should be added to {@link MainController}.<br>
 * If the hide and show functionality needs to be adapted by the super class, they can be overridden by the super class using "@Override" (this is done in {@link ChartsController} to keep plotting in the background).
 */
public abstract class BaseController implements DataReceivedListener {
	/**
	 * The main timer which runs the main {@link #loop()} every {@link Constants#BASE_LOOP_PERIOD}
	 */
	private Timer timer;
	/**
	 * Variable to manage if the tab is showing
	 */
	private boolean isShowing;

	/**
	 * This function is called from {@link MainController} when the associated tab or page is shown to the user
	 */
	public void show() {
		if(Constants.DEBUG) System.out.println(getTag() + ": " + "show()");
		isShowing = true;
		// Register the data listener to receive data received events from the Communication
		Locator.registerDataListener(this);
		// Start the loop timer
		if(timer == null) {
			timer = new Timer(true);
			TimerTask timerTask = new TimerTask() {
				@Override
				public void run() {
					if(Locator.getMainApp() != null && Locator.getMainApp().isAlive()) loop();
				}
			};
			timer.scheduleAtFixedRate(timerTask, Constants.BASE_LOOP_PERIOD, Constants.BASE_LOOP_PERIOD);
		}
	}

	/**
	 * This function is called from {@link MainController} when the associated tab or page is hidden from the user
	 */
	public void hide() {
		if(Constants.DEBUG) System.out.println(getTag() + ": " + "hide()");
		isShowing = false;
		// Unregister the data received listener
		Locator.unregisterDataListener(this);
		// Cancel the loop timer
		timer.cancel();
		timer = null;
	}

	/**
	 * Abstract method that is called by {@link #timer} every {@link Constants#BASE_LOOP_PERIOD}.<br>
	 * Abstract means that it needs to be implemented by the super class. If the super class doesn't need it, simply implement an empty function.<br>
	 * Note that this is not called from the UI thread, and thus UI modifications must use {@link javafx.application.Platform#runLater(Runnable)}.
	 */
	protected abstract void loop();

	/**
	 * Get the TAG string, identifying the controller (typically just a string of the class name, mostly used for debug and println statements)
	 * @return TAG
	 */
	protected abstract String getTag();

	/**
	 * Whether or not the tab is currently being shown
	 * @return is the tab showing
	 */
	public boolean isShowing() {
		return isShowing;
	}
}

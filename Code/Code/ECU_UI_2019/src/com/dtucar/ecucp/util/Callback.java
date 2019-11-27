package com.dtucar.ecucp.util;

/**
 * The Callback interface is used to send a result back to the method caller, when the method caller calls an asynchronous method
 * (that means a method that returns before its work is complete, e.g. it starts a new thread to do a task that takes a long time before completing).
 */
public interface Callback {
	/**
	 * The callback method, that will be called with the result
	 *
	 * @param result the result
	 */
	void callback(String result);
}

/*
 * Example:
 * public void connectSerial(String port, Callback callback) {
 *     Task task = (Task) () -> {
 *         doAsynchronousTask();
 *         callback.callback("success");
 *     };
 *     Thread thread = new Thread(task);
 * 	   thread.setDaemon(true);
 * 	   thread.start();
 * }
 *
 * can be called as
 *
 * connectSerial(port, result -> {
 *    if(result.equals("success")) doSomething();
 * });
 */

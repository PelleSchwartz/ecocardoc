package com.dtucar.ecucp.util;

/**
 * This <code>OnUpdateListener</code> is a generic interface used when float values are received and transmitted to listeners.<br/>
 *
 * @see com.dtucar.ecucp.view.ConfigTextField#setOnUpdateListener(OnUpdateListener)
 */
public interface OnUpdateListener {
	/**
	 * Will be called to notify of new value
	 *
	 * @param value the new value
	 */
	void onUpdate(float value);
}

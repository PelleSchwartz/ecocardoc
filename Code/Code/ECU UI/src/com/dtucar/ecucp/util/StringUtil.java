package com.dtucar.ecucp.util;

import java.text.DecimalFormat;

/**
 * The StringUtil class contains various utility functions related to strings in general1
 */
public class StringUtil {
	/**
	 * Create a nice-looking string version of a double (or float) with the #.## pattern if there are decimals or as an integer, if there are no decimals
	 *
	 * @param d the double to format
	 * @return the formatted string
	 */
	public static String doubleToShortString(double d) {
		if(d < 0.1) return new DecimalFormat("#.#####").format(d);
		d = (Math.round(d * 100)) / 100.0;
		if(d == (long) d) {
			return String.format("%d", (long) d);
		} else {
			return new DecimalFormat("#.##").format(d);
		}
	}

	/**
	 * Replace the last occurrence of a string in another string
	 *
	 * @param string      the string in which things need to be replaced
	 * @param toReplace   the string to look for and replace
	 * @param replacement the string to put in the place of toReplace
	 * @return the resulting string after replacement
	 */
	public static String replaceLast(String string, String toReplace, String replacement) {
		int pos = string.lastIndexOf(toReplace);
		if(pos > -1) {
			return string.substring(0, pos) + replacement + string.substring(pos + toReplace.length(), string.length());
		} else {
			return string;
		}
	}
}

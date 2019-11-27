package com.dtucar.ecucp.util;

import javafx.concurrent.Task;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Sequencer;
import java.io.IOException;
import java.io.InputStream;

/**
 * The TunesUtil class contains methods related to playing tunes
 */
public class TunesUtil {
	/**
	 * Boolean to start/stop playing songs
	 */
	private static boolean playSong = false;

	/**
	 * Enum containing all songs that can be played
	 */
	public enum Song {
		/**
		 * Stop playing songs
		 */
		STOP,
		/**
		 * Epic sax guy
		 */
		PARTY_SAX,
		/**
		 * Never Gonna Give You Up
		 */
		RICK
	}

	/**
	 * Play the specified song
	 *
	 * @param song song to play
	 */
	public static void playSong(Song song) {
		// Get the song path to the MIDI file
		String songPath;
		switch(song) {
			case PARTY_SAX:
				songPath = "assets/Sax.mid";
				break;
			case RICK:
				songPath = "assets/Rick.mid";
				break;
			default:
				TunesUtil.playSong = false;
				return;
		}
		TunesUtil.playSong = true;
		InputStream midiStream = Locator.getMainApp().getClass().getResourceAsStream(songPath);
		// Play the song in a separate thread
		Task task = new Task<Void>() {
			@Override
			protected Void call() {
				try {
					// Prepare the sequencer
					Sequencer sequencer = MidiSystem.getSequencer();
					sequencer.setSequence(MidiSystem.getSequence(midiStream));
					sequencer.setLoopCount(99);
					sequencer.open();
					sequencer.start();
					sequencer.setTempoInBPM(100);
					sequencer.setTempoFactor(1.5f);
					// Keep playing the song indefinitely until stopped
					while(playSong && Locator.getMainApp() != null && Locator.getMainApp().isAlive()) {
						Thread.sleep(1000);
					}
					// Stop the song
					sequencer.stop();
					sequencer.close();
				} catch(MidiUnavailableException e) {
					System.err.println("Midi device unavailable!");
				} catch(InvalidMidiDataException e) {
					System.err.println("Invalid Midi data!");
				} catch(IOException e) {
					System.err.println("I/O Error!");
				} catch(InterruptedException e) {
					System.err.println("InterruptedException!");
				}
				return null;
			}
		};
		Thread thread = new Thread(task);
		thread.setPriority(Thread.MIN_PRIORITY);
		thread.setDaemon(true);
		thread.start();
	}
}

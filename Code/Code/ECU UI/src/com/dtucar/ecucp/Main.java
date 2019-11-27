package com.dtucar.ecucp;

import com.dtucar.ecucp.util.AppLog;
import com.dtucar.ecucp.util.DialogUtil;
import com.dtucar.ecucp.util.Locator;
import com.dtucar.ecucp.communication.TeensySync;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Rectangle2D;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.Pane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.Screen;
import javafx.stage.Stage;
import javafx.stage.StageStyle;

import java.io.IOException;
import java.util.ResourceBundle;
import java.util.Timer;
import java.util.TimerTask;

/**
 * The Main class is the entry point to the Application.
 * It manages the lifecycle of the program and a few select application-wide functions.
 */
public class Main extends Application {
	private Stage mainStage;
	private static boolean isAlive;

	/**
	 * The start method is called by the application after init and is where all our startup functions occur
	 * @param initStage -
	 * @throws Exception if initialization fails (see {@link TeensySync#validateEnums()}
	 */
	@Override
	public void start(final Stage initStage) throws Exception {
		System.out.println("ECU UI Application start");
		// Show the splash screen on the initStage while we load our mainStage
		showSplash(initStage);
		// This will be our mainStage
		Stage stage = new Stage();
		// Initialize the Locator, used by all the controllers to find this Main
		Locator.initialize(this);
		// Set the default uncaught exception handler, so we can log unexpected exceptions
		Thread.setDefaultUncaughtExceptionHandler(Main::showError);
		// Initialize the string bundle
		ResourceBundle resources = ResourceBundle.getBundle("com.dtucar.ecucp.assets.StringBundle");
		Locator.setResources(resources);
		// Set the window title
		stage.setTitle(resources.getString("window_title"));
		this.mainStage = stage;
		// Initialize as maximized
		mainStage.setMaximized(true);
		// The initialization of the main stage is delayed by 50 ms, to allow the splash screen to appear on screen before we continue (since we run everything on the UI thread)
		new Timer(true).schedule(
				new TimerTask() {
					@Override
					public void run() {
						// Load the MainView, which references all our other views through the TabPane, so actually ALL our views get loaded at once
						FXMLLoader loader = new FXMLLoader(getClass().getResource("view/MainView.fxml"), Locator.getResources());
						Platform.runLater(() -> {
							Pane rootLayout;
							try {
								// Load all FXML views
								rootLayout = loader.load();
							} catch(IOException e) {
								e.printStackTrace();
								return;
							}
							// Initialize the scene with our newly loaded layout
							Scene scene = new Scene(rootLayout);
							mainStage.setScene(scene);
							// Set the icon that appears in the top left and the taskbar
							mainStage.getIcons().add(new Image(getClass().getResourceAsStream("assets/logo64.png")));
							// Show the main stage and hide the splash screen
							mainStage.show();
							initStage.hide();
							// The application is now alive
							isAlive = true;
						});
					}
				},
				50
		);
	}

	/**
	 * The splash screen is shown while the main stage is loading.
	 * Since all pages are loaded at once, this can take a few seconds, and this splash screen indicates that the application is starting.
	 * @param initStage the stage to use for the splash screen
	 */
	private void showSplash(final Stage initStage) {
		ImageView imageView = new ImageView(Main.class.getResource("assets/logo128.png").toString());
		VBox vBox = new VBox();
		vBox.getChildren().add(imageView);
		Scene splashScene = new Scene(vBox, Color.TRANSPARENT);
		splashScene.setFill(Color.TRANSPARENT);
		final Rectangle2D bounds = Screen.getPrimary().getBounds();
		initStage.setScene(splashScene);
		initStage.setX(bounds.getMinX() + bounds.getWidth() / 2 - 128 / 2);
		initStage.setY(bounds.getMinY() + bounds.getHeight() / 2 - 128 / 2);
		initStage.initStyle(StageStyle.TRANSPARENT);
		initStage.setAlwaysOnTop(true);
		initStage.show();
		initStage.toFront();
	}

	/**
	 * Any uncaught exception in the application is caught by this function.
	 * This prevents an application crash and calls {@link DialogUtil#showExceptionDialog(Throwable)} which logs the exception.
	 * Note that even though the application does not crash, at this point, something has gone horribly wrong, and the application can no longer be considered alive.
	 * We don't know where the exception occurred, so the application is in an unknown, probably not recoverable state.
	 * @param t The thread on which the exception occurred
	 * @param e The exception
	 * @see Thread#setDefaultUncaughtExceptionHandler(Thread.UncaughtExceptionHandler)
	 */
	private static void showError(Thread t, Throwable e) {
		isAlive = false;
		TeensySync.stop();
		System.err.println("***Default exception handler***");
		System.err.println("An unexpected error occurred in " + t);
		e.printStackTrace();
		AppLog.error("Unexpected error: " + e.getMessage());
		DialogUtil.showExceptionDialog(e);
	}

	/**
	 * Get the main stage of the application (e.g. to use as a parent for tooltips and dialogs).
	 * @return the main stage
	 */
	public Stage getStage() {
		return mainStage;
	}

	/**
	 * This method is called when the application is going down.
	 */
	@Override
	public void stop() {
		System.out.println("ECU UI Application stop");
		isAlive = false;
		Locator.dispose();
	}

	/**
	 * The isAlive value is set to false if the application is going down or if an unexpected exception was caught.
	 * @return true if the application can be considered alive
	 */
	public boolean isAlive() {
		return isAlive;
	}

	/**
	 * The main entry point when launching the application from the command line.
	 * @param args Command line arguments
	 */
	public static void main(String[] args) {
		launch(args);
	}
}
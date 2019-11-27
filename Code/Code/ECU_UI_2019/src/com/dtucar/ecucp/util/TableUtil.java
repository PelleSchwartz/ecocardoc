package com.dtucar.ecucp.util;

import com.dtucar.ecucp.communication.TeensySync.EcuEnum;
import com.dtucar.ecucp.model.LutValue;
import com.dtucar.ecucp.model.TestValue;
import com.dtucar.ecucp.model.TuningValue;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.sun.javafx.scene.control.skin.TableColumnHeader;
import javafx.application.Platform;
import javafx.collections.ObservableList;
import javafx.event.Event;
import javafx.scene.control.*;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;

import java.io.IOException;

/**
 * The TableUtil class contains utility functions related to tables in general
 */
public class TableUtil {
	/**
	 * Creates a context menu containing the import and export menu items for the table
	 *
	 * @param tableData the data associated with the table
	 * @return the context menu
	 */
	public static ContextMenu getImportExportContextMenu(ObservableList<?> tableData) {
		ContextMenu contextMenu = new ContextMenu();
		MenuItem importTable = new MenuItem("Import");
		importTable.setOnAction(event -> {
			if(tableData == null || tableData.size() == 0) return;
			try {
				importTable(tableData);
			} catch(Exception e) {
				AppLog.error("An error occurred during table import");
			}
		});
		MenuItem exportTable = new MenuItem("Export");
		exportTable.setOnAction(event -> {
			if(tableData == null || tableData.size() == 0) return;
			try {
				exportTable(tableData);
			} catch(IOException e) {
				AppLog.error("An error occurred during table export");
			}
		});
		contextMenu.getItems().addAll(importTable, exportTable);
		return contextMenu;
	}

	/**
	 * This function is called from the context menu to export the data in the table to a file
	 *
	 * @param tableData the data to export
	 * @throws IOException if an exception occurs
	 */
	@SuppressWarnings("unchecked")
	private static void exportTable(ObservableList<?> tableData) throws IOException {
		if(tableData.size() > 0 && tableData.get(0) instanceof LutValue) {
			SaveAndLoad.exportLutTable((ObservableList<LutValue>) tableData);
		} else if(tableData.size() > 0 && tableData.get(0) instanceof TestValue) {
			SaveAndLoad.exportTestTable((ObservableList<TestValue>) tableData);
		}
	}

	/**
	 * This function is called from the context menu to import the data to the table from a file
	 *
	 * @param tableData the data to import
	 */
	@SuppressWarnings("unchecked")
	private static void importTable(ObservableList<?> tableData) {
		if(tableData.size() > 0 && tableData.get(0) instanceof LutValue) {
			// TODO: Should only load LutValues and only notify ConfigurationController
			Locator.getMainController().loadConfiguration();
		} else if(tableData.size() > 0 && tableData.get(0) instanceof TestValue) {
			// TODO: Should only load TestValues and only notify TestController
			Locator.getMainController().loadConfiguration();
		}
	}

	/**
	 * Setup the table with general things, such as the selection model (navigating the the keyboard using enter and tab)
	 *
	 * @param tableView the table
	 */
	public static void setupTable(TableView tableView) {
		tableView.getSelectionModel().setCellSelectionEnabled(true);
		tableView.getSelectionModel().setSelectionMode(SelectionMode.SINGLE);
		// React on keyboard presses in the table
		tableView.addEventFilter(KeyEvent.KEY_PRESSED, (KeyEvent t) -> {
			if(tableView.getEditingCell() == null && t.getCode() == KeyCode.ENTER) {
				if(t.isShiftDown()) {
					// Pressing shift+enter goes to the cell above
					tableView.getSelectionModel().selectAboveCell();
				} else {
					// Pressing enter goes to the cell below
					tableView.getSelectionModel().selectBelowCell();
				}
				t.consume();
			}
			if(t.isControlDown() && t.getCode() == KeyCode.TAB) {
				if(t.isShiftDown()) {
					// Pressing shift+tab goes to the left cell
					tableView.getSelectionModel().selectLeftCell();
				} else {
					// Pressing tab goes to the right cell
					tableView.getSelectionModel().selectRightCell();
				}
				t.consume();
			}
		});
	}

	/**
	 * Convert LutValues to a JSON array (for export and save)
	 *
	 * @param lutValues the values to convert
	 * @return the JSON array
	 */
	public static JsonArray lutValuesToJsonArray(LutValue... lutValues) {
		JsonArray array = new JsonArray(lutValues.length);
		for(LutValue lutValue : lutValues) {
			JsonObject object = new JsonObject();
			object.addProperty(EcuEnum.LUT_RPM.get(), lutValue.getRpm());
			object.addProperty(EcuEnum.LUT_INJECTION1.get(), lutValue.getInjection1());
			object.addProperty(EcuEnum.LUT_INJECTION2.get(), lutValue.getInjection2());
			object.addProperty(EcuEnum.LUT_IGNITION.get(), lutValue.getIgnition());
			array.add(object);
		}
		return array;
	}

	/**
	 * Convert TestValues to a JSON array (for export and save)
	 *
	 * @param testValues the values to convert
	 * @return the JSON array
	 */
	public static JsonArray testValuesToJsonArray(TestValue... testValues) {
		JsonArray array = new JsonArray(testValues.length);
		for(int i = 0; i < testValues.length; i++) {
			TestValue testValue = testValues[i];
			JsonObject object = new JsonObject();
			object.addProperty(EcuEnum.TEST_VALUE_INDEX.get(), i);
			object.addProperty(EcuEnum.TEST_VALUE_MIN.get(), testValue.getMinSpeed());
			object.addProperty(EcuEnum.TEST_VALUE_MAX.get(), testValue.getMaxSpeed());
			array.add(object);
		}
		return array;
	}

	/**
	 * Convert TuningValues to a JSON array (for export and save)
	 *
	 * @param testValues the values to convert
	 * @return the JSON array
	 */

	/**
	 * Set a tooltip on the top label of a table column
	 *
	 * @param table       the table
	 * @param id          the id of the column (set using tableColumn.setId("myId"))
	 * @param tooltipText the text of the tooltip
	 */
	public static void setTooltipOnColumn(TableView table, String id, String tooltipText) {
		Tooltip tooltip = new Tooltip(tooltipText);
		tooltip.setWrapText(true);
		tooltip.setMaxWidth(300);
		TableColumnHeader header = (TableColumnHeader) table.lookup("#" + id);
		Label label = (Label) header.lookup(".label");
		label.setTooltip(tooltip);
		label.setMaxSize(Double.MAX_VALUE, Double.MAX_VALUE);
	}

	//region EditingCell

	/**
	 * The EditingCell is the cell of the tables which are editable<br>
	 * This probably doesn't need to be changed.
	 *
	 * @param <T>
	 */
	public static class EditingCell<T> extends TableCell<T, Integer> {
		private TextField textField;
		private String oldText;

		public EditingCell() {
		}

		@Override
		public void startEdit() {
			if(!isEmpty()) {
				super.startEdit();
				createTextField();
				setText(null);
				setGraphic(textField);
				oldText = textField.getText();
				Platform.runLater(() -> textField.requestFocus());
			}
		}

		@Override
		public void cancelEdit() {
			super.cancelEdit();
			setText(getItem().toString());
			setGraphic(null);
		}

		@Override
		public void updateItem(Integer item, boolean empty) {
			super.updateItem(item, empty);
			if(empty) {
				setText(null);
				setGraphic(null);
			} else {
				if(isEditing()) {
					if(textField != null) {
						textField.setText(getString());
					}
					setText(null);
					setGraphic(textField);
				} else {
					setText(getString());
					setGraphic(null);
				}
			}
		}

		private void saveValue() {
			if(oldText.equals(textField.getText())) return;
			int value = 0;
			try {
				value = Integer.parseInt(textField.getText());
			} catch(NumberFormatException ignored) {
			}
			commitEdit(value);
			oldText = textField.getText();
		}

		@Override
		public void commitEdit(final Integer valNew) {
			if(isEditing()) {
				super.commitEdit(valNew);
			} else {
				final TableView<T> tbl = getTableView();
				if(tbl != null) {
					final TablePosition<T, Integer> pos = new TablePosition<>(tbl, getTableRow().getIndex(), getTableColumn());
					final TableColumn.CellEditEvent<T, Integer> ev = new TableColumn.CellEditEvent<>(tbl, pos, TableColumn.editCommitEvent(), valNew);
					Event.fireEvent(getTableColumn(), ev);
				}
				updateItem(valNew, false);
				if(tbl != null) {
					tbl.edit(-1, null);
				}
			}
		}

		private void createTextField() {
			textField = new TextField(getString());
			textField.setStyle("-fx-padding: 1px 1px 1px 1px; -fx-font-size: 10pt;");
			textField.focusedProperty().addListener((arg0, oldFocused, newFocused) -> {
				if(!newFocused) {
					saveValue();
				}
			});
			textField.setOnKeyReleased(event -> {
				if(event.getCode() == KeyCode.ENTER) {
					saveValue();
					EditingCell.this.getTableView().requestFocus();
					EditingCell.this.getTableView().getSelectionModel().selectBelowCell();
				} else if(event.getCode() == KeyCode.ESCAPE) {
					cancelEdit();
				}
			});
			textField.requestFocus();
			textField.selectAll();
		}

		private String getString() {
			return getItem() == null ? "" : getItem().toString();
		}
	}

	//endregion
}

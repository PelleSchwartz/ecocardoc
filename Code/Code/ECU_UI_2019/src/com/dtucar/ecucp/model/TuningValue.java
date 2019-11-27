package com.dtucar.ecucp.model;

import javafx.beans.property.SimpleIntegerProperty;

/**
 * A TestValue represents 1 row in the test table in the {@link com.dtucar.ecucp.controller.TestController}.<br/>
 * The test will start by going to the maxSpeed of the TestValue with index 0, then coast to the minSpeed of index 1, and then burn to maxSpeed of index 1.
 */
public class TuningValue {
    /**
     * The index of the TestValue. This dictates the order of the values.
     */
    private final SimpleIntegerProperty index;
    /**
     * The minSpeed is the speed down to which the test will let the car coast before burning.
     */
    private final SimpleIntegerProperty tuningMinSpeed;
    /**
     * If the speed is at or below the minSpeed, the test will let the car burn until it reaches maxSpeed.
     */
    private final SimpleIntegerProperty tuningMaxSpeed;

    /**
     * Construct a new TestValue representing 1 row in the test table
     *
     * @param index          the row index of the value
     * @param tuningMinSpeed the speed where we start registering tuning values
     * @param tuningMaxSpeed the speed to burn up to
     * @see com.dtucar.ecucp.controller.TestController
     */
    public TuningValue(int index, int tuningMinSpeed, int tuningMaxSpeed) {
        this.index = new SimpleIntegerProperty(index);
        this.tuningMinSpeed = new SimpleIntegerProperty(tuningMinSpeed);
        this.tuningMaxSpeed = new SimpleIntegerProperty(tuningMaxSpeed);
    }

    //region Getters and setters
    public int getIndex() {
        return index.get();
    }

    public int getMinSpeed() {
        return tuningMinSpeed.get();
    }

    public void setMinSpeed(int tuningMinSpeed) {
        this.tuningMinSpeed.set(tuningMinSpeed);
    }

    public int getMaxSpeed() {
        return tuningMaxSpeed.get();
    }

    public void setMaxSpeed(int tuningMaxSpeed) {
        this.tuningMaxSpeed.set(tuningMaxSpeed);
    }
    //endregion
}

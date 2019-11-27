/**
 * <b>ECU Control Panel / ECU UI</b>
 * <p>
 * The ECU Control Panel controls the ECU and allows to set all relevant configuration values, see debug values and run automated tests.<br/>
 * The application loosely follows the Model–view–controller pattern. In a pure implementation, that would mean a complete separation in:<br/>
 * - View: What the user sees (panels, buttons, text, fields, tables etc.)<br/>
 * - Controller: What the user uses (functions called by buttons, listeners reacting on user events)<br/>
 * - Model: All the data manipulated by the Controller that in return backs the View (actual table values...).</p><br/><p>
 * But, this pattern is only loosely, maybe lazily followed (i.e. we don't have a separate CockpitModel):<br/>
 * The Model part of the pattern has for simplicity sake mostly been integrated into the Controller (and some of it just stays in the View).<br/>
 * The Controller contains references to the entire Model (such as the ObservableList that stores all Table values).<br/>
 * The View actually stores many values, such as all values in TextFields (so if you want to query the content of a TextField, you get it directly from the View).<br/>
 * When manipulating an indicator (e.g. the SpeedGauge), the Controller calls setValue directly on the View.<br/>
 * The advantage of this, is that you can go to the Controller and access and manipulate everything relevant to that page.<br/>
 * If you need to change something in the Cockpit logic, you go directly to the CockpitController.
 * </p>
 * @author Henning
 */
package com.dtucar.ecucp;


#include "canbus.h"

uint32_t floatToBinary(float data) {
	union {
		float dataIn;
		uint32_t dataOut;
	}ftb;
	ftb.dataIn = data;
	return ftb.dataOut;
}

float binaryToFloat(uint32_t data) {
	union {
		float  dataOut;
		uint32_t dataIn;
	}ftb;
	ftb.dataIn = data;
	return ftb.dataOut;
}


void can_init(uint16_t tx_id) {
	// Initialize canbus. Attaches canlistener, so everything is handled in interrupt. The default filter let everything pass.
	// The ECU uses the can1 ports instead of can0 Status as of 21-05-2018 22:28 : Doesn't work on the ECU
	Can0.begin(CANRATE);
	delay(10);
	Can0.attachObj(&CAN);

	CAN_filter_t allPassFilter;
	allPassFilter.id = 0;
	allPassFilter.ext = 1;
	allPassFilter.rtr = 0;

	// Leave the first 4 mailboxes to use the default filter. Just change the higher ones
	for(uint8_t filterNum = 4; filterNum < 14; filterNum++) {
		Can0.setFilter(allPassFilter, filterNum);
	}
	for(uint8_t filterNum = 0; filterNum < 14; filterNum++) {
		CAN.attachMBHandler(filterNum);
	}

	// Sets the local ID. In case of the ECU, it uses the MOTOR_ID, to receive and send signals camouflaged as the motorboard.
	CAN.setLocalID(tx_id);
}

void smartCAN::setLocalID(uint8_t id) {
	localTx_ID = id;
}

//Functions for 'CAN'

// Set ID for a status message
void smartCAN::setIdStatus(uint8_t priority) {
	uint8_t rxError;
	uint8_t txError;
	rxError = Can0.getRxError();
	txError = Can0.getTxError();
	cMessage.id = (priority << 24) | (localTx_ID << 18) | (MT_STATUS << 16) | (rxError << 8) | (txError);
	cMessage.ext = 1;
}

// Set ID for a measurement message
void smartCAN::setIdMeasurement(uint8_t priority, uint8_t measurementType, uint16_t sensorID) {
	cMessage.id = (priority << 24) | (localTx_ID << 18) | (MT_MEASURE << 16) | (measurementType << 11) | (sensorID);
	cMessage.ext = 1;
}

// Set mask
void smartCAN::setMask(uint8_t priority, uint8_t nodeID, uint8_t measureType, uint16_t smartTag, uint8_t mailbox) {
	uint32_t mask = (priority << 24) | (nodeID << 18) | (measureType << 16) | (smartTag << 0);
	Can0.setMask(mask, mailbox);
}

// Emergency stop
void smartCAN::emergencyStop(uint8_t errorNo) {
	setIdStatus(0);
	cMessage.len = 1;
	cMessage.buf[0] = errorNo;
	Can0.write(cMessage);
}

// Emergency stop
void smartCAN::emergencyStop() {
	setIdStatus(0);
	cMessage.len = 1;
	cMessage.buf[0] = ERROR_DEFAULT;
	Can0.write(cMessage);
}

void smartCAN::gotFrame(CAN_message_t &frame, int mailbox) {
	count++;

	lastMessage[(frame.id >> 18) & 0x3F] = millis(); // Resets receive timing.

	// Check MT bits
	switch((frame.id >> 16) & 0x03) {
	case 0: // Status
		gotStatus(frame);
		break;
	case 1: // Measurement
		gotMeasurement(frame);
		break;
	default:
		break;
	}
}

void smartCAN::gotStatus(CAN_message_t &frame) {
	int txNode = (frame.id >> 18) & 0x3F;
	uint32_t tempData = (frame.buf[3] << 24) | (frame.buf[2] << 16) | (frame.buf[1] << 8) | (frame.buf[0]);
	// No new data - return
	if(statusMsg[txNode] == tempData) {
		return;
	}
	// Checks priority. If 0 -> emergency stop
	if(frame.id >> 24) {
		// Copy new data. Raise flag.
		statusMsg[txNode] = tempData;
		flag_status = 1;
		// Update error count
		txError[txNode] = frame.id;
		txError[txNode] = (frame.id >> 8);
	} else {
		if(frame.len > 0) {
			//Serial.println("lort");	// TODO What does this mean?
			flag_emergency = frame.buf[0];
			emergencyTxID = txNode;
		}
	}
}

uint8_t smartCAN::checkEmergency() {
	return flag_emergency;
}

uint16_t smartCAN::checkEmergencyTxID(void) {
	return emergencyTxID;
}

void smartCAN::clearEmergency() {
	flag_emergency = 0;
}

void smartCAN::gotMeasurement(CAN_message_t &frame) {
	uint32_t data = (frame.buf[3] << 24) | (frame.buf[2] << 16) | (frame.buf[1] << 8) | frame.buf[0];
	float tempDATA = binaryToFloat(data);
	uint8_t tempID = frame.id & 0x0000FF;
	if(measurement[tempID] == tempDATA) {
		return;
	}
	// Copy new data. Raise flag.
	measurement[tempID] = tempDATA;
	flag_measure = 1;
}

//Get error number
uint8_t smartCAN::getCanTxError(uint8_t tx_id) {
	return txError[tx_id];
}

uint8_t smartCAN::getCanRxError(uint8_t tx_id) {
	return rxError[tx_id];
}

// Check if node has timed out
uint8_t smartCAN::goneToShit(uint8_t txId) {
	if(millis() - lastMessage[txId] > TIMEOUT_PERIOD) {
		return 1;
	} else {
		return 0;
	}
}

// Send status
void smartCAN::sendStatus(uint8_t priority) {
	setIdStatus(priority);
	cMessage.len = 0;
	Can0.write(cMessage);
}

//Send status with data
void smartCAN::sendStatus(uint8_t priority, uint32_t data) {
	statusMsg[localTx_ID] = data;
	setIdStatus(priority);
	cMessage.len = 4;
	for(uint8_t i = 0; i < 4; i++) {
		cMessage.buf[i] = ((data >> 8 * i));
	}
	Can0.write(cMessage);
}

// Send measurement
void smartCAN::sendMeasurement(uint8_t priority, uint8_t measurementType, uint16_t sensorID, float data) {
	measurement[sensorID] = data;
	uint32_t binaryData = floatToBinary(data);
	setIdMeasurement(priority, measurementType, sensorID);
	cMessage.len = 4;
	for(uint8_t i = 0; i < cMessage.len; i++) {
		cMessage.buf[i] = ((binaryData >> 8 * i));
	}
	Can0.write(cMessage);
}

// Get measurement
float smartCAN::getMeasurement(uint8_t sensor) {
	return measurement[sensor];
}

// Get status data
uint32_t smartCAN::getStatus(uint8_t nodeID) {
	return statusMsg[nodeID];
}

// Get status data with bit mask
uint8_t smartCAN::getSystemState(uint8_t nodeID, uint8_t state) {
	return (statusMsg[nodeID] >> state) & 0x00000001;
}

uint8_t smartCAN::newMeasure() {
	if(flag_measure) {
		flag_measure = 0;
		return 1;
	}
	return 0;
}

uint8_t smartCAN::newStatus() {
	if(flag_status) {
		flag_status = 0;
		return 1;
	}
	return 0;
}

uint32_t smartCAN::messageCount() {
	uint32_t temp = count;
	count = 0;
	return temp;
}

smartCAN CAN;

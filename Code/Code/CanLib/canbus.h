#ifndef _CANBUS_H
#define _CANBUS_H
#include "FlexCAN.h"
#include <stdint.h>
#include "canbusKeywords.h"

#define CANRATE 125000
#define TIMEOUT_PERIOD 100

class smartCAN : public CANListener {
	uint8_t flag_measure;
	uint8_t flag_status;
	uint8_t flag_emergency;
	uint16_t emergencyTxID;

	// Mainly debug -> message received. Resets on check.
	volatile uint32_t count = 0;

	uint32_t lastMessage[NUM_OF_TX_NODES];
	float measurement[NUM_OF_SENSORS];
	uint32_t statusMsg[NUM_OF_TX_NODES];
	uint8_t rxError[NUM_OF_TX_NODES];
	uint8_t txError[NUM_OF_TX_NODES];
	CAN_message_t cMessage;

	uint8_t localTx_ID;

	void setIdStatus(uint8_t priority);
	void setIdMeasurement(uint8_t priority, uint8_t measurementType, uint16_t sensorID);
	void setMask(uint8_t priority, uint8_t nodeID, uint8_t measureType, uint16_t smartTag, uint8_t mailbox);
	void gotFrame(CAN_message_t &frame, int mailbox); // Overrides the parent version so we can actually do something
	void gotStatus(CAN_message_t &frame);
	void gotMeasurement(CAN_message_t &frame);

public:
	void sendStatus(uint8_t priority);
	void sendStatus(uint8_t priority, uint32_t data);
	void sendMeasurement(uint8_t priority, uint8_t measurementType, uint16_t sensorID, float data);
	void emergencyStop();
	void emergencyStop(uint8_t errorNo);
	void setLocalID(uint8_t id);
	void clearEmergency();
	uint8_t goneToShit(uint8_t txId);

	float getMeasurement(uint8_t sensor);
	uint8_t getCanTxError(uint8_t tx_id);
	uint8_t getCanRxError(uint8_t tx_id);
	uint32_t getStatus(uint8_t nodeID);
	uint8_t getSystemState(uint8_t nodeID, uint8_t state);
	uint16_t checkEmergencyTxID(void);
	uint8_t checkEmergency();
	uint32_t messageCount();
	uint8_t newStatus();
	uint8_t newMeasure();
};

void can_init(uint16_t tx_id);
extern smartCAN CAN;
#endif



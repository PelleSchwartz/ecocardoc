/*
 * CANSync.h
 *
 */

#pragma once

#define STANDALONE_MODE	1	// 1 = RIO not connected, 0 = RIO connected

extern uint8_t emergency;
void CAN_sync();
void CANtoSerial();
void RIOtoSerial();
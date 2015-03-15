/* Copyright (C) 2014  Alex-Daniel Jakimenko <alex.jakimenko@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#pragma once

#include <stm32f10x.h>

namespace nrf8001 {

void processIncomingPacket();

enum OperatingMode {
    TEST = 1, SETUP = 2, STANDBY = 3,
};

enum HwError {
    NO_ERROR = 0, FATAL_ERROR = 1,
};

enum TestFeature {
    DTM_OVER_UART = 1, DTM_OVER_ACL = 2, EXIT_TEST_MODE = 0xFF,
};

enum Reason {
    REMOTE_USER_TERMINATED_CONNECTION = 1, UNACCEPTABLE_CONNECTION_TIMING = 2,
};

enum RadioTransmitPowerLevel {
    MINUS_18_DBM = 0, MINUS_12_DBM = 1, MINUS_6_DBM = 2, MINUS_0_DBM = 3,
};

enum ApplLatencyMode {
    APPLICATION_LATENCY_DISABLED = 0, APPLICATION_LATENCY_ENABLED = 1,
};


#pragma pack(push, 1)
// System Events

struct DeviceStartedEvent {
    OperatingMode operatingMode; // current device mode
    HwError hwError; // cause of the restart
    u8 dataCreditAvailable; // available buffers
};

struct EchoEvent {
    u8 data[29];
};

struct HardwareErrorEvent {
    u16 lineNumber;
    char fileName[22];
};

struct CommandResponseEvent {
    u8 commandOpCode;
    u8 status; // TODO enum
    u8 responseData[27];
};

struct ConnectedEvent {
    u8 addressType; // TODO enum
    u8 peerAddress[6];
    u16 connectionInterval; // in periods of 1.25 ms
    u16 slaveLatency; // 0..1000 * 10 ms
    u16 supervisionTimeout;
    u8 masterClockAccuracy; // TODO enum?
};

struct DisconnectedEvent {
    u8 aciStatus; // TODO enum
    u8 btLeStatus; // TODO enum
};

struct BondStatusEvent {
    u8 bondStatusCode; // TODO enum
    u8 bondStatusSource; // TODO enum
    u8 bondStatusSecMode1; // TODO enum
    u8 bondStatusSecMode2; // TODO enum
    u8 bondStatusKeyExchSlave; // TODO enum
    u8 bondStatusKeyExchMaster; // TODO enum
};

struct PipeStatusEvent { // TODO
    u8 pipesOpen[8];
    u8 pipesClosed[8];
};

struct TimingEvent {
    u16 connectionInterval; // 6..3200 * 1.25 ms
    u16 slaveLatency; // 0..1000
    u16 supervisionTimeout; // 10.3200 * 10 ms
};

struct DisplayKeyEvent {
    char passkey[6];
};

struct KeyRequestEvent {
    u8 keyType;
};

// Data Events

struct DataCreditEvent {
    u8 dataCredits;
};

struct PipeErrorEvent {
    u8 servicePipeNo; // 1..62
    u8 errorCode; // TODO enum?
    u8 errorData[27];
};

struct DataReceivedEvent {
    u8 servicePipeNo;
    u8 data[20];
};

struct DataAckEvent {
    u8 servicePipeNumber; // 1..62
};

#pragma pack(pop)

}

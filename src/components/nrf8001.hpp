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
#include <sys/types.h>
#include <queue>

namespace {
std::queue<u8> outputBuffer; // TODO !!!
}

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

// System commands

void test(TestFeature mode) { // returns DeviceStartedEvent
    outputBuffer.push(2);
    outputBuffer.push(0x01);
    outputBuffer.push(mode);
}

// size 0..29
void echo(u8 data[], uint size) { // returns EchoEvent
    outputBuffer.push(size + 1);
    outputBuffer.push(0x02);
    for (uint i = 0; i < size; i++)
        outputBuffer.push(data[i]);
}

void dtmCommand(u16 dtmCommand) { // returns CommandResponseEvent
    outputBuffer.push(3);
    outputBuffer.push(0x03);
    outputBuffer.push(0xFF00 & dtmCommand);
    outputBuffer.push(0x00FF & dtmCommand);
}

void sleep() {
    outputBuffer.push(1);
    outputBuffer.push(0x04);
}

void wakeup() { // returns DeviceStartedEvent followed by CommandResponseEvent
    outputBuffer.push(1);
    outputBuffer.push(0x05);
}

// size 1..30
void setup(u8 data[], uint size) { // CommandResponseEvent followed by DeviceStartedEvent upon completion
    outputBuffer.push(size + 1);
    outputBuffer.push(0x06);
    for (uint i = 0; i < size; i++)
        outputBuffer.push(data[i]);
}

void readDynamicData() { // returns CommandResponseEvent
    outputBuffer.push(1);
    outputBuffer.push(0x07);
}

// size 1..27
void writeDynamicData(u8 sequenceNumber, u8 data[], uint size) { // returns CommandResponseEvent
    outputBuffer.push(size + 1);
    outputBuffer.push(0x08);
    outputBuffer.push(sequenceNumber);
    for (uint i = 0; i < size; i++)
        outputBuffer.push(data[i]);
}

void getDeviceVersion() { // returns CommandResponseEvent
    outputBuffer.push(1);
    outputBuffer.push(0x09);
}

void getDeviceAddress() { // returns CommandResponseEvent
    outputBuffer.push(1);
    outputBuffer.push(0x0A);
}

void getBatteryLevel() { // returns CommandResponseEvent
    outputBuffer.push(1);
    outputBuffer.push(0x0B);
}

void XgetTemperature() { // returns CommandResponseEvent
    outputBuffer.push(1);
    outputBuffer.push(0x0C);
}

void radioReset() { // returns CommandResponseEvent
    outputBuffer.push(1);
    outputBuffer.push(0x0E);
}

// timeout 1-16383 (seconds), advInterval 2-16384 (* 0.625 msec)
void connect(u16 timeout, u16 advInterval) { // returns many events, see datasheet
    outputBuffer.push(5);
    outputBuffer.push(0x0F);
    outputBuffer.push(0xFF00 & timeout);
    outputBuffer.push(0x00FF & timeout);
    outputBuffer.push(0xFF00 & advInterval);
    outputBuffer.push(0x00FF & advInterval);
}

// timeout 1-180, advInterval 0x0020 - 0x4000
void radioReset(u16 timeout, u16 advInterval) { // returns many events, see datasheet
    outputBuffer.push(5);
    outputBuffer.push(0x10);
    outputBuffer.push(0xFF00 & timeout);
    outputBuffer.push(0x00FF & timeout);
    outputBuffer.push(0x00FF & advInterval);
    outputBuffer.push(0xFF00 & advInterval);
}

void disconnect(Reason reason) { // returns CommandResponseEvent followed by DisconnectedEvent
    outputBuffer.push(2);
    outputBuffer.push(0x11);
    outputBuffer.push(reason);
}

void setTxPower(RadioTransmitPowerLevel radioTransmitPowerLevel) { // returns CommandResponseEvent
    outputBuffer.push(2);
    outputBuffer.push(0x12);
    outputBuffer.push(radioTransmitPowerLevel);
}

void changeTimingRequest() { // returns CommandResponseEvent followed by TimingEvent
    outputBuffer.push(1);
    outputBuffer.push(0x13);
}

// intervalMin intervalMax 6..3200, slaveLatency 0..1000, timeout 10..3200
void changeTimingRequest(u16 intervalMin, u16 intervalMax, u16 slaveLatency, u16 timeout) { // returns CommandResponseEvent followed by TimingEvent
    outputBuffer.push(9);
    outputBuffer.push(0x13);
    outputBuffer.push(0x00FF & intervalMin);
    outputBuffer.push(0xFF00 & intervalMin);
    outputBuffer.push(0x00FF & intervalMax);
    outputBuffer.push(0xFF00 & intervalMax);
    outputBuffer.push(0x00FF & slaveLatency);
    outputBuffer.push(0xFF00 & slaveLatency);
    outputBuffer.push(0x00FF & timeout);
    outputBuffer.push(0xFF00 & timeout);
}

// servicePipeNumber 1..62
void openRemotePipe(u8 servicePipeNumber) { // returns CommandResponseEvent followed by PipeStatusEvent or PipeErrorEvent
    outputBuffer.push(2);
    outputBuffer.push(0x14);
    outputBuffer.push(servicePipeNumber);
}

// latency 0..(slave latency - 1)
void setApplLatency(ApplLatencyMode applLatencyMode, u16 latency) { // returns CommandResponseEvent
    outputBuffer.push(4);
    outputBuffer.push(0x19);
    outputBuffer.push(applLatencyMode);
    outputBuffer.push(0xFF00 & latency);
    outputBuffer.push(0x00FF & latency);
}

void setKey() { // returns CommandResponseEvent
    outputBuffer.push(2);
    outputBuffer.push(0x1A);
    outputBuffer.push(0); // KeyType
}

void setKey(char (&pin)[6]) { // returns CommandResponseEvent
    outputBuffer.push(2);
    outputBuffer.push(0x1A);
    outputBuffer.push(1); // KeyType
    for (auto curDigit : pin) // XXX packet length could also be 18. What?
        outputBuffer.push(curDigit);
}

//
void openAdvPipe(u8 (&advServiceDataPipes)[8]) { // returns CommandResponseEvent
    outputBuffer.push(9);
    outputBuffer.push(0x1B);
    for (auto curBitmapPiece : advServiceDataPipes)
        outputBuffer.push(curBitmapPiece);
}

// timeout 1..16383 (seconds), advInterval 0x0100..0x4000 (* 0.625 ms)
void broadcast(u16 timeout, u16 advInterval) { // returns CommandResponseEvent
    outputBuffer.push(5);
    outputBuffer.push(0x1C);
    outputBuffer.push(0xFF00 & timeout);
    outputBuffer.push(0x00FF & timeout);
    outputBuffer.push(0xFF00 & advInterval);
    outputBuffer.push(0x00FF & advInterval);
}

void bondSecurityRequest() { // returns CommandResponseEvent
    outputBuffer.push(1);
    outputBuffer.push(0x1D);
}

void directedConnect() { // returns CommandResponseEvent
    outputBuffer.push(1);
    outputBuffer.push(0x1E);
}

// servicePipeNumber 1..62
void closeRemotePipe(u8 servicePipeNumber) { // returns CommandResponseEvent followed by PipeStatusEvent or PipeErrorEvent
    outputBuffer.push(2);
    outputBuffer.push(0x1F);
    outputBuffer.push(servicePipeNumber);
}

// Data commands

// servicePipeNumber 1..62, size 0..20
void setLocalData(u8 servicePipeNumber, u8 data[], uint size) { // returns CommandResponseEvent
    outputBuffer.push(size + 2);
    outputBuffer.push(0x1D);
    outputBuffer.push(servicePipeNumber);
    for (uint i = 0; i < size; i++)
        outputBuffer.push(data[i]);
}

// servicePipeNumber 1..62, size 1..20
void sendData(u8 servicePipeNumber, u8 data[], uint size) { // returns DataCreditEvent
    outputBuffer.push(size + 2);
    outputBuffer.push(0x15);
    outputBuffer.push(servicePipeNumber);
    for (uint i = 0; i < size; i++)
        outputBuffer.push(data[i]);
}

// servicePipeNumber 1..62
void SendDataAck(u8 servicePipeNumber) { // returns nothing
    outputBuffer.push(2);
    outputBuffer.push(0x16);
    outputBuffer.push(servicePipeNumber);
}

// servicePipeNumber 1..62
void RequestData(u8 servicePipeNumber) { // returns DataReceivedEvent or PipeErrorEvent
    outputBuffer.push(2);
    outputBuffer.push(0x17);
    outputBuffer.push(servicePipeNumber);
}

// servicePipeNumber 1..62, errorCode 0x80..0xFF
void SendDataNack(u8 pipeNumber, u8 errorCode) { // returns DataCreditEvent
    outputBuffer.push(3);
    outputBuffer.push(0x18);
    outputBuffer.push(pipeNumber);
    outputBuffer.push(errorCode);
}

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

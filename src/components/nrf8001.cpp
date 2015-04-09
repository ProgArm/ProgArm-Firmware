#include "nrf8001.hpp"

#include <stdio.h>
#include <cstdbool>
#include <cstring>
#include <queue>

#include "../core/connection.hpp"
#include "../core/spi.hpp"
#include "../services.h"

namespace nrf8001 {

struct SetupPiece {
    u8 skip;
    u8 data[32];
};

const SetupPiece setupMessage[NB_SETUP_MESSAGES] = SETUP_MESSAGES_CONTENT;

OperatingMode status = UNKNOWN;
bool connected = false;

TimingEvent lastTimingEvent; // TODO null?
uint dataCredits = 0;

// System commands

void test(TestFeature mode) { // returns DeviceStartedEvent
    spi::outputBuffer.push(2);
    spi::outputBuffer.push(0x01);
    spi::outputBuffer.push(mode);
}

// size 0..29
void echo(u8 data[], uint size) { // returns EchoEvent
    spi::outputBuffer.push(size + 1);
    spi::outputBuffer.push(0x02);
    for (uint i = 0; i < size; i++)
        spi::outputBuffer.push(data[i]);
}

void dtmCommand(u16 dtmCommand) { // returns CommandResponseEvent
    spi::outputBuffer.push(3);
    spi::outputBuffer.push(0x03);
    spi::outputBuffer.push(0xFF00 & dtmCommand);
    spi::outputBuffer.push(0x00FF & dtmCommand);
}

void sleep() {
    spi::outputBuffer.push(1);
    spi::outputBuffer.push(0x04);
}

void wakeup() { // returns DeviceStartedEvent followed by CommandResponseEvent
    spi::outputBuffer.push(1);
    spi::outputBuffer.push(0x05);
}

// size 1..30
void setup(u8 data[], uint size) { // CommandResponseEvent followed by DeviceStartedEvent upon completion
    spi::outputBuffer.push(size + 1);
    spi::outputBuffer.push(0x06);
    for (uint i = 0; i < size; i++)
        spi::outputBuffer.push(data[i]);
}

void readDynamicData() { // returns CommandResponseEvent
    spi::outputBuffer.push(1);
    spi::outputBuffer.push(0x07);
}

// size 1..27
void writeDynamicData(u8 sequenceNumber, u8 data[], uint size) { // returns CommandResponseEvent
    spi::outputBuffer.push(size + 1);
    spi::outputBuffer.push(0x08);
    spi::outputBuffer.push(sequenceNumber);
    for (uint i = 0; i < size; i++)
        spi::outputBuffer.push(data[i]);
}

void getDeviceVersion() { // returns CommandResponseEvent
    spi::outputBuffer.push(1);
    spi::outputBuffer.push(0x09);
}

void getDeviceAddress() { // returns CommandResponseEvent
    spi::outputBuffer.push(1);
    spi::outputBuffer.push(0x0A);
}

void getBatteryLevel() { // returns CommandResponseEvent
    spi::outputBuffer.push(1);
    spi::outputBuffer.push(0x0B);
}

void XgetTemperature() { // returns CommandResponseEvent
    spi::outputBuffer.push(1);
    spi::outputBuffer.push(0x0C);
}

void radioReset() { // returns CommandResponseEvent
    spi::outputBuffer.push(1);
    spi::outputBuffer.push(0x0E);
}

// timeout 1-16383 (seconds), advInterval 2-16384 (* 0.625 msec)
void connect(u16 timeout, u16 advInterval) { // returns many events, see datasheet
    spi::outputBuffer.push(5);
    spi::outputBuffer.push(0x0F);
    spi::outputBuffer.push(0xFF00 & timeout);
    spi::outputBuffer.push(0x00FF & timeout);
    spi::outputBuffer.push(0xFF00 & advInterval);
    spi::outputBuffer.push(0x00FF & advInterval);
}

// timeout 1-180, advInterval 0x0020 - 0x4000
void radioReset(u16 timeout, u16 advInterval) { // returns many events, see datasheet
    spi::outputBuffer.push(5);
    spi::outputBuffer.push(0x10);
    spi::outputBuffer.push(0xFF00 & timeout);
    spi::outputBuffer.push(0x00FF & timeout);
    spi::outputBuffer.push(0x00FF & advInterval);
    spi::outputBuffer.push(0xFF00 & advInterval);
}

void disconnect(Reason reason) { // returns CommandResponseEvent followed by DisconnectedEvent
    spi::outputBuffer.push(2);
    spi::outputBuffer.push(0x11);
    spi::outputBuffer.push(reason);
}

void setTxPower(RadioTransmitPowerLevel radioTransmitPowerLevel) { // returns CommandResponseEvent
    spi::outputBuffer.push(2);
    spi::outputBuffer.push(0x12);
    spi::outputBuffer.push(radioTransmitPowerLevel);
}

void changeTimingRequest() { // returns CommandResponseEvent followed by TimingEvent
    spi::outputBuffer.push(1);
    spi::outputBuffer.push(0x13);
}

// intervalMin intervalMax 6..3200, slaveLatency 0..1000, timeout 10..3200
void changeTimingRequest(u16 intervalMin, u16 intervalMax, u16 slaveLatency, u16 timeout) { // returns CommandResponseEvent followed by TimingEvent
    spi::outputBuffer.push(9);
    spi::outputBuffer.push(0x13);
    spi::outputBuffer.push(0x00FF & intervalMin);
    spi::outputBuffer.push(0xFF00 & intervalMin);
    spi::outputBuffer.push(0x00FF & intervalMax);
    spi::outputBuffer.push(0xFF00 & intervalMax);
    spi::outputBuffer.push(0x00FF & slaveLatency);
    spi::outputBuffer.push(0xFF00 & slaveLatency);
    spi::outputBuffer.push(0x00FF & timeout);
    spi::outputBuffer.push(0xFF00 & timeout);
}

// servicePipeNumber 1..62
void openRemotePipe(u8 servicePipeNumber) { // returns CommandResponseEvent followed by PipeStatusEvent or PipeErrorEvent
    spi::outputBuffer.push(2);
    spi::outputBuffer.push(0x14);
    spi::outputBuffer.push(servicePipeNumber);
}

// latency 0..(slave latency - 1)
void setApplLatency(ApplLatencyMode applLatencyMode, u16 latency) { // returns CommandResponseEvent
    spi::outputBuffer.push(4);
    spi::outputBuffer.push(0x19);
    spi::outputBuffer.push(applLatencyMode);
    spi::outputBuffer.push(0xFF00 & latency);
    spi::outputBuffer.push(0x00FF & latency);
}

void setKey() { // returns CommandResponseEvent
    spi::outputBuffer.push(2);
    spi::outputBuffer.push(0x1A);
    spi::outputBuffer.push(0); // KeyType
}

void setKey(char (&pin)[6]) { // returns CommandResponseEvent
    spi::outputBuffer.push(2);
    spi::outputBuffer.push(0x1A);
    spi::outputBuffer.push(1); // KeyType
    for (auto curDigit : pin) // XXX packet length could also be 18. What?
        spi::outputBuffer.push(curDigit);
}

//
void openAdvPipe(u8 (&advServiceDataPipes)[8]) { // returns CommandResponseEvent
    spi::outputBuffer.push(9);
    spi::outputBuffer.push(0x1B);
    for (auto curBitmapPiece : advServiceDataPipes)
        spi::outputBuffer.push(curBitmapPiece);
}

// timeout 1..16383 (seconds), advInterval 0x0100..0x4000 (* 0.625 ms)
void broadcast(u16 timeout, u16 advInterval) { // returns CommandResponseEvent
    spi::outputBuffer.push(5);
    spi::outputBuffer.push(0x1C);
    spi::outputBuffer.push(0xFF00 & timeout);
    spi::outputBuffer.push(0x00FF & timeout);
    spi::outputBuffer.push(0xFF00 & advInterval);
    spi::outputBuffer.push(0x00FF & advInterval);
}

void bondSecurityRequest() { // returns CommandResponseEvent
    spi::outputBuffer.push(1);
    spi::outputBuffer.push(0x1D);
}

void directedConnect() { // returns CommandResponseEvent
    spi::outputBuffer.push(1);
    spi::outputBuffer.push(0x1E);
}

// servicePipeNumber 1..62
void closeRemotePipe(u8 servicePipeNumber) { // returns CommandResponseEvent followed by PipeStatusEvent or PipeErrorEvent
    spi::outputBuffer.push(2);
    spi::outputBuffer.push(0x1F);
    spi::outputBuffer.push(servicePipeNumber);
}

// Data commands

// servicePipeNumber 1..62, size 0..20
void setLocalData(u8 servicePipeNumber, u8 data[], uint size) { // returns CommandResponseEvent
    spi::outputBuffer.push(size + 2);
    spi::outputBuffer.push(0x1D);
    spi::outputBuffer.push(servicePipeNumber);
    for (uint i = 0; i < size; i++)
        spi::outputBuffer.push(data[i]);
}

// servicePipeNumber 1..62, size 1..20
void sendData(u8 servicePipeNumber, u8 data[], uint size) { // returns DataCreditEvent
    spi::outputBuffer.push(size + 2);
    spi::outputBuffer.push(0x15);
    spi::outputBuffer.push(servicePipeNumber);
    for (uint i = 0; i < size; i++)
        spi::outputBuffer.push(data[i]);
}

// servicePipeNumber 1..62
void SendDataAck(u8 servicePipeNumber) { // returns nothing
    spi::outputBuffer.push(2);
    spi::outputBuffer.push(0x16);
    spi::outputBuffer.push(servicePipeNumber);
}

// servicePipeNumber 1..62
void RequestData(u8 servicePipeNumber) { // returns DataReceivedEvent or PipeErrorEvent
    spi::outputBuffer.push(2);
    spi::outputBuffer.push(0x17);
    spi::outputBuffer.push(servicePipeNumber);
}

// servicePipeNumber 1..62, errorCode 0x80..0xFF
void SendDataNack(u8 pipeNumber, u8 errorCode) { // returns DataCreditEvent
    spi::outputBuffer.push(3);
    spi::outputBuffer.push(0x18);
    spi::outputBuffer.push(pipeNumber);
    spi::outputBuffer.push(errorCode);
}

// Logic
void sendConfiguration() {
    for (auto curMessage : setupMessage)
        for (int i = 0; i < curMessage.data[0] + 1; i++)
            spi::outputBuffer.push(curMessage.data[i]);
}

void onConnect(ConnectedEvent* event) {
    connected = true;
    char buf[50];
    sprintf(buf, "Connected!");
    connection::printPlain(buf);
}

void onDisconnect(DisconnectedEvent* event) {
    connected = false;
    char buf[50];
    sprintf(buf, "Disconnected (AciStatus=%d, BtLeStatus=%d)", event->aciStatus, event->btLeStatus);
    connection::printPlain(buf);
}

void onReceive(DataReceivedEvent* event) {
    // TODO
}

void processIncomingPacket() {
    if (!spi::inputBuffer.empty() && spi::inputBuffer.size() > spi::inputBuffer.front()) {
        uint length = spi::inputBuffer.front() - 1; // -1 because we read event type byte immediately
        spi::inputBuffer.pop();
        uint eventType = spi::inputBuffer.front();
        spi::inputBuffer.pop();

        u8 eventBytes[32]; // TODO what is the max size for input packets?
        for (uint i = 0; i < length; i++) {
            eventBytes[i] = spi::inputBuffer.front();
            spi::inputBuffer.pop();
        }
        char buf[50];
        sprintf(buf, "Packet received: %d", eventType);
        connection::printPlain(buf);
        switch (eventType) {
        case 0x81: {
            auto event = (DeviceStartedEvent*) &eventBytes; // this is unportable, but unfortunately it is the only way to keep the code short
            status = event->operatingMode;
            sprintf(buf, "Mode: %d", status);
            connection::printPlain(buf);
            if (status == SETUP)
                sendConfiguration();
            break;
        }
        case 0x82: {
            auto event = (EchoEvent*) &eventBytes;
            // TODO printPlain it?
            break;
        }
        case 0x83: {
            auto event = (HardwareErrorEvent*) &eventBytes;
            // TODO this should be logged
            break;
        }
        case 0x84: {
            auto event = (CommandResponseEvent*) &eventBytes;
            if (event->status > 1) { // XXX change to enum
                sprintf(buf, "NRF error: %d", event->status);
                connection::printPlain(buf);
            }
            break;
        }
        case 0x85: {
            auto event = (ConnectedEvent*) &eventBytes;
            onConnect(event);
            break;
        }
        case 0x86: {
            auto event = (DisconnectedEvent*) &eventBytes;
            onDisconnect(event);
            break;
        }
        case 0x87: {
            auto event = (BondStatusEvent*) &eventBytes;
            // XXX
            break;
        }
        case 0x88: {
            auto event = (PipeStatusEvent*) &eventBytes;
            // TODO maintain a list of available pipes?
            break;
        }
        case 0x89: {
            auto event = (TimingEvent*) &eventBytes;
            memcpy(&lastTimingEvent, event, sizeof(TimingEvent)); // TODO test it
            break;
        }
        case 0x8E: {
            auto event = (DisplayKeyEvent*) &eventBytes;
            // XXX
            break;
        }
        case 0x8F: {
            auto event = (KeyRequestEvent*) &eventBytes;
            // XXX
            break;
        }
        case 0x8A: {
            auto event = (DataCreditEvent*) &eventBytes;
            dataCredits = event->dataCredits;
            break;
        }
        case 0x8D: {
            auto event = (PipeErrorEvent*) &eventBytes;
            // TODO this should be logged
            break;
        }
        case 0x8C: {
            auto event = (DataReceivedEvent*) &eventBytes;
            onReceive(event);
            break;
        }
        case 0x8B: {
            auto event = (DataAckEvent*) &eventBytes;
            // TODO just ignore it?
            break;
        }
        default:
            break;
        }
    }
}

}

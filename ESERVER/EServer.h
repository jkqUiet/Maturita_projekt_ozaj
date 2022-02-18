#pragma once

#include "WiFi.h"
#include "WiFiUdp.h"
#include <SoftwareSerial.h>

#define UDP_TX_PACKET_MAX_SIZE 8192
 
#define AP_SSID "EServer"
#define AP_PASS "AhojMajo"
#define UDP_PORT 4210

#include "EDevice.h"
#include "ESD.h"
#include "EStack.h"

#define DEV_COUNT 15
#define TASK_COUNT 3
#define PUMP_SIZE_CARD 2
#define MAX_BUFFER_SIZE 15
#define MAX_PACKET_PARAM 15
#define MAX_RESEND_BUFFER_SIZE 10

#define TO_SEND_DATA "/datatosend.txt"

#define TEMP_FILE "/"

class EServer {
    bool isSenderAsked;
    char packet[UDP_TX_PACKET_MAX_SIZE];
    EStack* _buffer; // buffer pre incomming packety
    EStack* _packetParams;

    IPAddress* _local_IP;
    IPAddress* _gateway;
    IPAddress* _subnet;
    unsigned char _responseSender;
    unsigned char _packetSent;

    ESD* _sdCard;
    EDevice* _devices[DEV_COUNT];
    unsigned char _lWritten;
    WiFiUDP* UDP;
    WiFiClass* wifi;
    SoftwareSerial *serverSerial;
    String _packetContent;

    int _timeOutCounter;
    
  public:

    EServer();
    ~EServer();

    bool tryAddDevice(EDevice* dev);
    bool tryFindDevice(int id, EDevice* dev = nullptr);
    
    void startAccessPoint();
    void printDevices();

    void writeToCard(EDevice* tempDevice);
    void writeToDataToSendFile();
   // String* getFromCard();
    
    void checkPackets();
    void parseParams();
    void doStuffPacket();
    void continueRecievePacket();

    void  sendPackets();
    bool trySendDataToSerial();
    bool contactServer();
    void listenFromServer();
    
    void mainThread();
};

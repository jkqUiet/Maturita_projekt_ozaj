#include "EServer.h"

EServer::EServer(): _lWritten(0) { 
  _local_IP = new IPAddress(192,168,12,1);
  _gateway = new IPAddress(192,168,12,1);
  _subnet = new IPAddress(255,255,255,0);
  serverSerial = new SoftwareSerial(13, 12);
  serverSerial->begin(115200);
  serverSerial->setTimeout(10);
  Serial.println("Idem vytvorit _sdCard");
  _sdCard = new ESD();
  Serial.println("Vytvoril som _sdCard");
  _buffer = new EStack(MAX_BUFFER_SIZE);
  Serial.println("Vytvoril som _buffer EStack");
  _packetParams = new EStack(MAX_PACKET_PARAM);
  _packetSent = 'X';
  _responseSender = 'X';
  _timeOutCounter = 0;
}

void EServer::mainThread() {
  unsigned char cycleCounter = 0;
  while(true) {
    switch (cycleCounter) {
      case 0 :       
        checkPackets();         
        break;
      case 1 : {       
        doStuffPacket();
        break;
      }
      case 2 : {
        sendPackets();
        break;
      }
      default: 
        break;
    }
    cycleCounter++;
    if (cycleCounter % TASK_COUNT == 0) cycleCounter = 0;   
  }
}

void EServer::checkPackets() {
  int packetSize = UDP->parsePacket();
  if (packetSize) {
    Serial.print("Received packet! Size: ");
    Serial.println(packetSize); 
    int len = UDP->read(packet, 255);
    if (len > 0)
    {
      packet[len] = '\0';
    }
    String packetFill;
    for( int i = 0; i < 255; i++) {
      packetFill += packet[i];
      if (packet[i] == '@') {
        break;
      }
    }
    Serial.println("Pridal som packet");
    Serial.println(packetFill);

    Serial.print("Pocet packetov v zasobniku : ");
    Serial.println(_buffer->getSize());

    _buffer->push(packetFill);
  }
}

void EServer::writeToCard(EDevice* tempDevice) {
  Serial.print("Pocet parametrov v zasobniku : ");
  Serial.println(_packetParams->getSize());
  String fileName = tempDevice->getFileName();
  String idDevice = String(tempDevice->getId());
  Serial.println(idDevice);
  Serial.println(fileName);
  if (tryFindDevice(idDevice.toInt())) {
    _sdCard->writeData(fileName, _packetParams); 
    Serial.println("Subor zapisany");
  } else {
    Serial.println("Zariadenie " + idDevice + " nieje autorizovane a teda nema moznost zapisu na sd kartu.");
  }
  _packetParams->clear();
    Serial.print("Pocet parametrov v zasobniku : ");
    Serial.println(_packetParams->getSize());
}

void EServer::writeToDataToSendFile(){
  _sdCard->writeData(, _packetParams);
  Serial.println("Subor zapisany do dat na odoslanie");
}

//String* EServer::getFromCard() {
 // return (_sdCard->getData(TO_SEND_DATA));
 
//}

void EServer::continueRecievePacket() {
  Serial.println("Hello world");
}

void EServer::printDevices() {
  for (int i = 0; i < _lWritten; i++) {
    Serial.print(_devices[i]->getId());
    Serial.print(" ");
    Serial.println(_devices[i]->getRoom());
  }
}

bool EServer::tryAddDevice(EDevice* dev) {
  if (!tryFindDevice(dev->getId()) && _lWritten - 1 < DEV_COUNT) {
    EStack tempLast(1);
    _sdCard->getData(dev->getLastSentFileName(),tempLast, -1);
    String vLastSent = "";
    if (tempLast.isEmpty()) {
      tempLast.push("0");
      _sdCard->writeData(dev->getLastSentFileName(), *tempLast);
    } else {
      vLastSent = tempLast().pop();
    }

    dev->setLastSentPosition(_sdCard->getLastSentPosition(dev->getFileName(),vLastSent.toInt()));
    dev->setLastIDSent(vLastSent);
    _devices[_lWritten] = dev;
  
    _lWritten++;
    return true;
  }
  delete dev;
  return false;
}

bool EServer::tryFindDevice(int id, EDevice* dev) {
  for (int i = 0; i < _lWritten; i++) {
    if (_devices[i]->getId() == id)  {
      dev = _devices[i];
      return true;
    }
  }
  dev = nullptr;
  return false;
}

void EServer::parseParams() {
  //String packetData = _buffer->pop();
  String packetData = _packetContent;
  String packetUnit = "";

  for (char v : packetData) {
    if (v == '$') {
      _packetParams->push(packetUnit);
      packetUnit = "";
    } else {
      packetUnit += v;
    }
  }
}

/*Ak je hodnota v packetSent X tak neposlal packet 
 * Ak je hodnota v packetSent Y tak odoslal packet
 * Ak je hodnota v responseSender X tak neposlal packet
 * Ak je hodnota v responseSender D tak poslal dotazovaci
 * Ak je hodnota v responseSender T tak poslal datovy packet
 */

bool EServer::trySendDataToSerial() {
  if (_packetSent == 'X' && _responseSender == 'D'){
    serverSerial->println(_packetContent);
    Serial.println("Bolo poslane " + _packetContent);
    Serial.println("Hodnoty packetSent " + _packetSent);
    Serial.println("Hodnoty responseSender " + _responseSender);
    _packetSent = 'Y';
    _responseSender = 'T';
    return true;
  }
  return false;
}
  /*if (!isSenderAsked){
    serverSerial->println("&");
    isSenderAsked = true;
  }
  else if (serverSerial->available()){
    if ((serverSerial->readString()).indexOf("Y")){
        serverSerial->println("^");
        String gap = "";
        while(!_packetParams->isEmpty()){
          String temp = _packetParams->pop();
          if (temp.isEmpty()){
            return false;
          }
          else{
            gap += temp + " ";
          }
        }
        serverSerial->println(gap);
        Serial.println("Poslané cez Serialku: " + gap);
        isSenderAsked = false;
        return true;
    }
    else if ((serverSerial->readString()).indexOf("N")){
      return false;
    }
  }
  return true;*/

void EServer::sendPackets() {
  for (int i = 0; i < _lWritten; i++) {
    _sdCard->getData(_devices[i]->getFileName(),_buffer, _devices[i]->getLastSentPosition());
  } 
  listenFromServer();
  while (!_buffer->isEmpty()) {
    contactServer();
    if (_packetSent == 'X' && _responseSender == 'D') {
        Serial.println("Posielam na server zo súboru na odoslanie");
        trySendDataToSerial(); 
        Serial.println("Posielam na server");
        trySendDataToSerial();
        _packetParams->clear();
    } 
    else if (_packetSent == 'Y' && _responseSender == 'D'){
      Serial.println("Server neodpovedal");
    }
    delay(10);
  }       
  for (int i = 0; i < _lWritten; i++) {
    _sdCard->replaceContentFile(_devices[i]->getFileName(), _devices[i]->getLastIDSent());
  }
}

void EServer::doStuffPacket() {
  if (_buffer->isEmpty() && _packetContent.isEmpty()) {
    return;
  }
  if (_packetContent.isEmpty()) {  
    _packetContent = _buffer->pop();
  }
  char packetType = (_packetContent[0]);
  
  if (!_packetContent.isEmpty()){
    switch (packetType) {
      case '1' : {
        parseParams();
        if (_packetParams->isEmpty()) {
          break;
        }      
       _packetParams->pop();
        _packetContent = "";
        EDevice* newDevice = new EDevice();        
        String id = _packetParams->pop();
  
        newDevice->setId(id.toInt());
        newDevice->setIp(_packetParams->pop());   
        newDevice->setRoom(_packetParams->pop());
       
        if (tryAddDevice(newDevice)) {
          Serial.println("Zariadenie bolo pridane");
          printDevices();
        } else {
          Serial.println("Zariadenie sa nepodarilo pridat");
        }
        break;
      }
      case '2' : {
        EDevice* tempDev =  nullptr; //pointer z ktorého budeš tahať názvo súboru
        tryFindDevice(toInt(_packetParams->pop()), tempDev);
        if (tempDev != nullptr) {
          writeToCard(tempDev);
        }

        break;
     }
      default:
        break;
    }
  }
}


/*Ak je hodnota v packetSent X tak neposlal packet 
 * Ak je hodnota v packetSent Y tak odoslal packet
 * Ak je hodnota v responseSender X tak neposlal packet
 * Ak je hodnota v responseSender D tak poslal dotazovaci
 * Ak je hodnota v responseSender T tak poslal datovy packet
 
  (_packetSent == 'X' && _responseSender == 'D')
  */

void EServer::listenFromServer() {
  if (_packetSent == 'Y' && _responseSender == 'D' && 
  serverSerial->available() && (serverSerial->readString()).indexOf("YesImHere")) {
    _packetSent = 'X';
    _responseSender = 'D';
    Serial.println("Odpovedal mi");
  }
  if (_packetSent == 'Y' && _responseSender == 'T' &&
  serverSerial->available() && (serverSerial->readString()).indexOf("IGotIt")) {
    _packetContent = "";
    _packetSent = 'X';
    _responseSender = 'X';
  }
}

bool EServer::contactServer() {
  if (_packetSent == 'X' && _responseSender == 'X') {
  serverSerial->println("HelloMyFriend");
  _packetSent = 'Y';
  _responseSender = 'D';
  return true;
  }
  return false;
}

void EServer::startAccessPoint() {
  Serial.println("Starting access point...");
  wifi = new WiFiClass();
  UDP = new WiFiUDP();
  Serial.println("New ESP prebehlo");
  wifi->softAPConfig(*_local_IP, *_gateway, *_subnet);
  Serial.println("SOFTAPCONFIG prebehol");
  wifi->softAP(AP_SSID, AP_PASS);
  Serial.println(wifi->localIP());

  UDP->begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP_PORT);
}

EServer::~EServer() {
  for (int i = 0; i < DEV_COUNT; i++) {
      delete _devices[i];
  }
  delete _buffer, _packetParams;
}

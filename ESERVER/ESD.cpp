#include "ESD.h"

//#define FILE_WRITE (O_READ | O_WRITE | O_CREAT | O_APPEND)

ESD::ESD(): _OFName(""), _isOpen(false){
  _file = new File();
  while(!SD_MMC.begin("/sdcard", true)) {
    Serial.println("NENASIEL SOM KARTU !");
    delay(2000);  }
  Serial.println("Uspech z kartou");
}
/*
String ESD::getFirstRow(String fileName) {
  if (!_isOpen) {
    openFile(fileName, false);
  }
  Serial.println("GetRow Idem");
  String temp = _file->readStringUntil('\n');
  if (temp.isEmpty()){
    return temp;
  }
  closeFile();
  Serial.println(temp);
    *_file = SD_MMC.open(fileName, "r");
    _file->seek(0);
   // if (temp.length() > 3) {
    //char* start = _file->position();

      for (int i = 0; i < 10; i++) {
        Serial.print(*_file);
        if (i%20 == 0 ){
           Serial.println();
        }
        _file++;
      }
   // }
  _file->close();
  openFile(fileName, false);
  return temp;
}
*/
void ESD::getData(String fileName, EStack& stack, int& pos){
  unsigned char cReadedData = 0;
  if (!SD_MMC.exists(fileName)){
    Serial.println("Nenasiel som subor");
    return;
  }
  if(!_isOpen){
    openFile(fileName, false);
  }
  if (pos != -1) {
    _file->seek(pos);
  }
  String temp;
  for(int i = 0; i < stack.getMaxSize(); i++){
    Serial.println("Citam zo suboru");
    temp = _file->readStringUntil('\n');
    if (temp.isEmpty()) {
      return cReadedData;
    }
    stack.push(temp);
    pos++;
  }

}

bool ESD::writeData(String fileName, EStack* data){
  openFile(fileName, true);
  while (!data->isEmpty()){
    Serial.println(fileName);
    String temp = data->pop();
    Serial.println("temp: " + temp);
    if (temp.isEmpty()){
      return false;
    }
    if (!_file->print(temp)){
      
      Serial.println("you failed");
    }
    _file->print("  ;  ");
  }
  _file->println();
  closeFile();
  return true;
}

void ESD::replaceContentFile(String fileName, String content) {
  closeFile();
  *_file =  SD_MMC.open(fileName, FILE_WRITE); 
  _file->write(content);
  closeFile();
}

bool ESD::createFile(String& fileName){
  if (SD_MMC.exists(fileName)){
    return false;
  }
  File lFile = SD_MMC.open(fileName, FILE_WRITE);
  lFile.close();
  return true;
}

void ESD::openFile(String fileName, bool type){
  if (_isOpen){
    closeFile();
  }
  if (type){
    *_file = SD_MMC.open(fileName, FILE_APPEND);
  }
  else{
    *_file = SD_MMC.open(fileName, FILE_READ);
  }
  _isOpen = true;
  _OFName = fileName;
}

int ESD::getLastSentPosition(String fileName, int lastSentPacketOld) {
  int lastSentPositionActual = 0;
  openFile(fileName, false);
  if (_isOpen){
    while (true) {
      String idPac = _file->readStringUntil(';');
      if (idPac.isEmpty()) {
        return lastSentPositionActual;
      }
      int lastSentPacketNew = idPac.toInt();
      if (lastSentPacketNew <= lastSentPacketOld) {
        lastSentPositionActual++;
      }
      if (!_file->readStringUntil('\n')) {
        return lastSentPositionActual;
      }      
    }
  }
  return lastSentPositionActual;
}

void ESD::closeFile(){
  _file->close();
  _isOpen = false;
  _OFName = "";
}

ESD::~ESD(){
  closeFile();
  delete _file;
}

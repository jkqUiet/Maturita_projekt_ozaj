#pragma once
#include "SD_MMC.h"
#include "EStack.h"
#include "SD.h"

#define PUMP_SIZE 2

class ESD
{
  File* _file;
  String _OFName;
  bool _isOpen;
  //String getFirstRow(String fileName);
  
public:
  ESD();
  void getData(String fileName, EStack& stack, int& pos);
  bool writeData(String fileName, EStack* data);  
  void replaceContentFile(String fileName, String content);
  bool createFile(String& fileName);
  void openFile(String fileName, bool type);
  void closeFile();
  int getLastSentPosition(String fileName, int lastSentPacketOld);
  ~ESD();
};

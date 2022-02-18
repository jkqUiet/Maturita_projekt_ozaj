#pragma once
#include <Arduino.h>

class EDevice {

  String _room;
  int _id; // key
  String _ipaddr;
  bool _sending;
  bool _active;
  String _fileName;
  String _lastSentFileName;
  int _lastSentPosition;
  String _lastIDPacketSent = "";

public:
  EDevice(const int id, String ip, String room);
  EDevice();
  ~EDevice();
  bool replyRequest();
  void setSending(bool sending);
  void setId(int id) {_id = id;};
  void setIp(String ip) {_ipaddr = ip;};
  void setRoom(String room) {_room = room;};
  void setLastSentPosition(int lS) {_lastSentPosition = lS;}
  void setLastIDSent(String IDPacket) {_lastIDPacketSent = IDPacket;}
  
  int getId() {return _id;};
  String getIpAddr() {return _ipaddr;};
  String getLastIDSent() {return _lastIDPacketSent;}
  String getRoom() {return _room;};
  int getLSPos() {return _lastSentPosition;}
  bool getSending() {return _sending;};
  String& getFileName() {return _fileName;}
  String& getLastSentFileName() {return _lastSentFileName;}
  bool setActvie(bool active){_active = active;};
};

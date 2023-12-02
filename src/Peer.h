#ifndef PEER_H
#define PEER_H

#include "Msg.h"
#include <ctime>
#include <map>
#include <string>
#include <vector>
#include <msgpack/unpack.hpp>

class Session;

class Peer
{
  enum Type
  {
    RADIO_BOX = 0x01,
    RADIO_BOX_CONTROL = 0x02,
    PC_CLIENT = 0x03,
    PC_CLIENT_CONTROL = 0x04,
    MOBILE_CLIENT = 0x05,
    MOBILE_CLIENT_CONTROL = 0x06,
    UNKNOWN = 0xFF
  };

public:
  Peer(int sock, int epollFd);
  int sock() const;
  void handleReceivedData(char *buf, int len);

private:
  void updatePeerType(char type);
  void handleMessage(Msg msg);
  void sendDataToRemotePeer(char *buf, int len);

private:
  int m_sock = 0;
  int m_epollFd = 0;

  class Session* m_session = nullptr; //not delete in this scope. His remowing when all socketFD is closed

  std::time_t time_created = std::time(nullptr);
  Type m_type = UNKNOWN;
  std::vector<char> m_receivedData;
  msgpack::unpacker m_unpack;
};

#endif // PEER_H

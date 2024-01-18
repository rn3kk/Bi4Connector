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
    PC_CLIENT = 0x03,
    MOBILE_CLIENT = 0x05,
    UNKNOWN = 0xFF
  };

public:
  Peer(int sock, int epollFd, char *ip, uint16_t port);
  ~Peer();

  int sock() const;
  void handleReceivedData(int len, int threadId);

  char *getBufferPtr();
  int getBufferLen();

private:
  void updatePeerType(char type);
  void handleMessage(const Msg &msg, int threadId);
  void sendDataToRemotePeer(char *buf, int len);

private:
  int m_sock = 0;
  int m_epollFd = 0;
  uint16_t m_port = 0;
  std::string m_ip;

  class Session *m_session =
      nullptr; // not delete in this scope. His remowing when all socketFD is closed

  std::time_t time_created = std::time(nullptr);
  Type m_type = UNKNOWN;
  std::vector<char> m_receivedData;
  msgpack::unpacker m_unpack;
};

#endif // PEER_H

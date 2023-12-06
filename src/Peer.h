#ifndef PEER_H
#define PEER_H

#include <ctime>
#include <string>
#include <vector>

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
  Peer(int sock, int epollFd);
  int sock() const;
  void handleReceivedData(char &buff, int len);

private:
  void updatePeerType();

private:
  int m_sock = 0;
  int m_epollFd = 0;
  std::string m_id;
  Session *m_session = nullptr;

  std::time_t time_created = std::time(nullptr);
  Type m_type = UNKNOWN;

  std::vector<char> m_receivedData;
};

#endif // PEER_H

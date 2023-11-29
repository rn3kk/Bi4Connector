#ifndef PEER_H
#define PEER_H

#include <ctime>
#include <string>

class Session;

class Peer
{
  enum Type
  {
    RADIO_BOX = 1,
    RADIO_BOX_CONTROL = 2,
    PC_CLIENT = 2,
    PC_CLIENT_CONTROL = 3,
    MOBILE_CLIENT = 4,
    MOBILE_CLIENT_CONTROL = 5,
    UNKNOWN
  };

public:
  Peer(int sock, int epollFd);

private:
  int m_sock = 0;
  int m_epollFd = 0;
  std::string m_id;
  Session *m_session = nullptr;

  std::time_t time_created = std::time(nullptr);
  Type m_type = UNKNOWN;
};

#endif // PEER_H

#ifndef PEER_H
#define PEER_H

#include <ctime>
#include <string>

class Peer
{
  enum Type
  {
    RADIO_BOX = 1,
    PC_SOWTWARE = 2,
    UNKNOWN
  };

public:
  Peer(std::string hash, int sock);

public:
  int m_sock;

private:
  std::string m_hash;
  std::time_t time_created = std::time(nullptr);
  Type m_type = UNKNOWN;
};

#endif // PEER_H

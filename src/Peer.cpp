#include "Peer.h"

Peer::Peer(std::string hash, int sock)
{
  m_hash = hash;
  m_sock = sock;
}

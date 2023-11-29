#include "Peer.h"

Peer::Peer(int sock, int epollFd)
{
  m_sock = sock;
  m_epollFd = epollFd;
}

#include "Peer.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

Peer::Peer(int sock, int epollFd)
{
  m_sock = sock;
  m_epollFd = epollFd;
  m_receivedData.reserve(1024 * 1024 * 2);
}

int Peer::sock() const { return m_sock; }

void Peer::handleReceivedData(char &buff, int len)
{
  m_receivedData.insert(m_receivedData.end(), buff, buff + len);
  if (m_type == UNKNOWN)
  {
    updatePeerType();
  }
}

void Peer::updatePeerType()
{
  char type = m_receivedData.at(0);
  m_receivedData.erase(m_receivedData.begin());
  switch (type)
  {
  case RADIO_BOX:
    m_type = RADIO_BOX;
    break;  
  case PC_CLIENT:
    m_type = PC_CLIENT;
    break;  
  case MOBILE_CLIENT:
    m_type = MOBILE_CLIENT;
    break;  
  default:
    epoll_ctl(m_epollFd, EPOLL_CTL_DEL, m_sock, nullptr);
    close(m_sock);
    break;
  }
}

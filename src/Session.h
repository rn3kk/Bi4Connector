#ifndef SESSION_H
#define SESSION_H

// class Peer;
#include "Peer.h"

using namespace std;
class Session
{
public:
  Session(string_view radioBoxHash);
  ~Session();

public:
  Peer *m_radio = nullptr;
  Peer *m_client = nullptr;

  std::string sessionId() const;

  void destroyRadioPeer();
  void destroyClentPeer();

private:
  string m_sessionId;
};

Session* getSession(string_view sessId);
void removeFDFromSession(int fd, Session* session);
void removeFd(int fd);

#endif // SESSION_H

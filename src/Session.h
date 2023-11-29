#ifndef SESSION_H
#define SESSION_H

// class Peer;
#include "Peer.h"

class Session
{
public:
  Session();

public:
  Peer *m_radio = nullptr;
  Peer *m_client = nullptr;

  Peer *m_radioControd = nullptr;
  Peer *m_clientControl = nullptr;
};

#endif // SESSION_H

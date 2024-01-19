#include "Session.h"
#include "Logger.h"
#include <list>
#include <map>

// list<Session *> sesions;
static map<string, Session *> sessions;
static std::mutex sessions_edit_mutex;

Session::Session(string_view radioBoxHash)
{
  m_sessionId = radioBoxHash;
  lInfo(-1, "Session() id: " + m_sessionId);
}

Session::~Session() { lInfo(-1, "~Session() id: " + m_sessionId); }

std::string Session::sessionId() const { return m_sessionId; }

void Session::destroyRadioPeer()
{
  delete m_radio;
  m_radio = nullptr;
}

void Session::destroyClentPeer()
{
  delete m_client;
  m_client = nullptr;
}

Session *getSession(std::string_view sesId)
{
  lDebug(-1, "getSession()");
  std::lock_guard<std::mutex> guard(sessions_edit_mutex);
  try
  {
    return sessions.at(std::string(sesId));
  }
  catch (const std::out_of_range &)
  {
    Session *s = new Session(sesId);
    sessions[std::string(sesId)] = s;
    return s;
  }
}

void removeFDFromSession(int fd, Session *session)
{
  if (session == nullptr)
    return;

  if (session->m_client->sock() == fd)
  {
    session->destroyClentPeer();
  }
  else if (session->m_radio->sock() == fd)
  {
    session->destroyRadioPeer();
  }
  std::lock_guard<std::mutex> guard(sessions_edit_mutex);

  if (session->m_radio == nullptr && session->m_client == nullptr)
  {
    for (auto it = sessions.begin(); it != sessions.end(); ++it)
    {
      if (it->second == session)
      {
        delete it->second;
        it = sessions.erase(it);
      }
    }
  }
}

void removeFd(int fd)
{
  if (fd == 0)
    return;
  std::lock_guard<std::mutex> guard(sessions_edit_mutex);
  for (auto i = sessions.begin(); i != sessions.end(); ++i)
  {
    Session *session = i->second;
    if (session->m_client != nullptr && session->m_client->sock() == fd)
    {
      session->destroyClentPeer();
    }
    else if (session->m_radio != nullptr && session->m_radio->sock() == fd)
    {
      session->destroyRadioPeer();
    }

    if (session->m_radio == nullptr && session->m_client == nullptr)
    {
      for (auto it = sessions.begin(); it != sessions.end(); ++it)
      {
        if (it->second == session)
        {
          delete it->second;
          it = sessions.erase(it);
        }
      }
    }
  }
}

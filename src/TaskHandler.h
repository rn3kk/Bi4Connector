#ifndef TASKHANDLER_H
#define TASKHANDLER_H

#include <queue>

class TaskHandler
{
public:
  TaskHandler();
  void addTask(int fd);

private:
  std::queue<int> m_taskQueue;
};

#endif // TASKHANDLER_H

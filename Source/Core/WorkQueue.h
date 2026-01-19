#pragma once

using TaskFunc = void (*)(void*);

struct Task
{
    TaskFunc func;
    void* context;
};

class WorkQueue
{
  public:
    void PushTask(Task task);

    Task PopTask();

  private:
};

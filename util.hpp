#pragma once

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include <QMetaObject>

template <typename Functor, typename FunctorReturnType>
void runOnAppThread(QObject *context, Functor &&function,
                    FunctorReturnType *ret = nullptr) {
  bool ok = QMetaObject::invokeMethod(context, function,
                                      Qt::BlockingQueuedConnection, ret);

  if (!ok) {
    throw "Failed to invoke qt method";
  }
}

template <typename Functor>
void runOnAppThread(QObject *context, Functor &&function) {
  bool ok = QMetaObject::invokeMethod(context, function,
                                      Qt::QueuedConnection);

  if (!ok) {
    throw "Failed to invoke qt method";
  }
}

struct ReturnLock {
  int return_code;
  std::mutex exit_mutex{};
  std::unique_lock<std::mutex> exit_lock;
  std::condition_variable exit_cond{};

public:
  ReturnLock() : return_code(-1), exit_lock(this->exit_mutex) {}

  void setReturn(int code) {
    std::cout << "return1" << std::endl;
    this->return_code = code;
    std::cout << "return2" << std::endl;
    this->exit_lock.unlock();
    std::cout << "return3" << std::endl;
    this->exit_cond.notify_all();
  }

  int awaitReturn() {
    std::cout << "await1" << std::endl;
    if (this->return_code != -1) {
      return this->return_code;
    }

    std::cout << "await2" << std::endl;

    this->exit_cond.wait(this->exit_lock);

    std::cout << "await3" << std::endl;
    return this->return_code;
  }
};

// A threadsafe-queue.
template <class T> 
class SafeQueue {
public:
  SafeQueue(void) : q(), m(), c() {}

  ~SafeQueue(void) {}

  // Add an element to the queue.
  void enqueue(T t) {
    std::lock_guard<std::mutex> lock(m);
    q.push(t);
    c.notify_one();
  }

  // Get the "front"-element.
  // If the queue is empty, wait till a element is avaiable.
  T dequeue(void) {
    std::unique_lock<std::mutex> lock(m);
    while (q.empty()) {
      // release lock as long as the wait and reaquire it afterwards.
      c.wait(lock);
    }
    T val = q.front();
    q.pop();
    return val;
  }

private:
  std::queue<T> q;
  mutable std::mutex m;
  std::condition_variable c;
};

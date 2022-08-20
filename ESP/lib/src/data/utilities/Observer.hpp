#pragma once
#ifndef OBSERVER_HPP
#define OBSERVER_HPP
#include <set>

namespace ObserverEvent
{
  enum Event
  {
    configLoaded = 1,
    deviceConfigUpdated = 2,
    cameraConfigUpdated = 3,
    networksConfigUpdated = 4,
  };
}

class IObserver
{
public:
  void update(ObserverEvent::Event event){};
};

class ISubject
{
private:
  std::set<IObserver *> observers;

public:
  void attach(IObserver *observer)
  {
    this->observers.insert(observer);
  }

  void detach(IObserver *observer)
  {
    this->observers.erase(observer);
  }

  void notify(ObserverEvent::Event event)
  {
    std::set<IObserver *>::iterator iterator = observers.begin();

    while (iterator != observers.end())
    {
      (*iterator)->update(event);
      ++iterator;
    }
  }
};

#endif // !OBSERVER_HPP
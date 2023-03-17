#ifndef OBSERVER_HPP
#define OBSERVER_HPP
#include <memory>
#include <string>
#include <unordered_map>

template <typename EnumT>
class IObserver {
 public:
  virtual void update(EnumT event) = 0;
  virtual std::string getName() = 0;
};

template <typename EnumT>
class ISubject {
 private:
  typedef IObserver<EnumT>* Observer_t;
  typedef std::unordered_map<std::string, Observer_t> Observers_t;

  Observers_t observers;

 public:
  void attach(Observer_t observer) {
    this->observers.emplace(observer->getName(), observer);
  }

  void detach(Observer_t observer) {
    // Note: delete pointer
    delete (*this->observers.find(observer->getName())).second;
    this->observers.erase(observer->getName());
  }

  void detachAll() {
    // Note: delete pointers
    for (auto observer = observers.begin(); observer != observers.end();
         ++observer) {
      delete (*observer).second;
    }
    // Note: clear map
    this->observers.clear();
  }

  void notifyAll(EnumT event) {
    for (auto observer = observers.begin(); observer != observers.end();
         ++observer) {
      (*observer).second->update(event);
    }
  }

  void notify(EnumT event, const std::string& observerName) {
    auto it_map = observers.find(observerName);
    if (it_map != observers.end()) {
      (*it_map).second->update(event);
      return;
    }
    log_e("Invalid Map Index");
    return;
  }
};
#endif  // OBSERVER_HPP

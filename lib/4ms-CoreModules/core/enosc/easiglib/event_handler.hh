#pragma once

#include "buffer.hh"

namespace easiglib
{

constexpr int kEventBufferSize = 64;

template<class Event>
struct EventSource : Nocopy {
  virtual void Poll(std::function<void(Event)> const& put) = 0;
};

template<class T, class Event>
struct EventHandler : crtp<T, EventHandler<T, Event>> {

  using EventStack = BufferReader<Event, kEventBufferSize>;

  void put(Event e) {
    events_.Write(e);
    pending_++;
  }

  void Poll() {
    for(auto& s : (**this).sources_)
      s->Poll([this](Event e){ put(e); });
  }

  void Process() {
    while(pending_ >= 0) {
      (**this).Handle(EventStack(events_, pending_));
      pending_--;
    }
  }

  struct DelayedEventSource : EventSource<Event> {
    int count_ = -1;
    Event event_;
    void Poll(std::function<void(Event)> const& put) final {
      if (count_ >= 0 && count_-- == 0) put(event_);
    }
    void trigger_after(int delay, Event e) {
      event_ = e;
      count_ = delay;
    }
    void Stop() { count_ = -1; }
  };
  
private:
  RingBuffer<Event, kEventBufferSize> events_;
  // number of events remaining to handle - 1
  int pending_ = -1;
};

}

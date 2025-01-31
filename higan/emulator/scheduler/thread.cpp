auto Thread::EntryPoints() -> vector<EntryPoint>& {
  static vector<EntryPoint> entryPoints;
  return entryPoints;
}

auto Thread::Enter() -> void {
  for(uint64_t index : range(EntryPoints().size())) {
    if(co_active() == EntryPoints()[index].handle) {
      auto entryPoint = EntryPoints()[index].entryPoint;
      EntryPoints().remove(index);
      while(true) {
        scheduler.serialize();
        entryPoint();
      }
    }
  }
  struct ThreadNotFound{};
  throw ThreadNotFound{};
}

Thread::~Thread() {
  destroy();
}

auto Thread::active() const -> bool { return co_active() == _handle; }
auto Thread::handle() const -> cothread_t { return _handle; }
auto Thread::frequency() const -> uintmax { return _frequency; }
auto Thread::scalar() const -> uintmax { return _scalar; }
auto Thread::clock() const -> uintmax { return _clock; }

auto Thread::setHandle(cothread_t handle) -> void {
  _handle = handle;
}

auto Thread::setFrequency(double frequency) -> void {
  _frequency = frequency + 0.5;
  _scalar = Second / _frequency;
}

auto Thread::setScalar(uintmax scalar) -> void {
  _scalar = scalar;
}

auto Thread::setClock(uintmax clock) -> void {
  _clock = clock;
}

auto Thread::create(double frequency, function<void ()> entryPoint) -> void {
  if(!_handle) {
    _handle = co_create(64 * 1024 * sizeof(void*), &Thread::Enter);
  } else {
    co_derive(_handle, 64 * 1024 * sizeof(void*), &Thread::Enter);
  }
  EntryPoints().append({_handle, entryPoint});
  setFrequency(frequency);
  setClock(0);
  scheduler.append(*this);
}

auto Thread::destroy() -> void {
  scheduler.remove(*this);
  if(_handle) co_delete(_handle);
  _handle = nullptr;
}

auto Thread::step(uint clocks) -> void {
  _clock += _scalar * clocks;
}

//ensure all threads are caught up to the current thread before proceeding.
auto Thread::synchronize() -> void {
  //note: this will call Thread::synchronize(*this) at some point, but this is safe:
  //the comparison will always fail as the current thread can never be behind itself.
  for(auto thread : scheduler._threads) synchronize(*thread);
}

//ensure the specified thread(s) are caught up the current thread before proceeding.
template<typename... P>
auto Thread::synchronize(Thread& thread, P&&... p) -> void {
  //switching to another thread does not guarantee it will catch up before switching back.
  while(thread.clock() < clock()) {
    //disable synchronization for auxiliary threads during serialization.
    //serialization can begin inside of this while loop.
    if(scheduler.serializing()) break;
    co_switch(thread.handle());
  }
  //convenience: allow synchronizing multiple threads with one function call.
  if constexpr(sizeof...(p) > 0) synchronize(forward<P>(p)...);
}

auto Thread::serialize(serializer& s) -> void {
  s.integer(_frequency);
  s.integer(_scalar);
  s.integer(_clock);
}

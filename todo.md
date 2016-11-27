Implementation:
- [x] State
- [ ] CraneControl (all sensors/motors)
- [ ] CraneData // in update() - remember not to lock more than one devices/sensors at once
- [ ] draft.cpp

additional:
- [ ] Logger <- add log level
- [ ] Logger <- add option to redirect log output to file // or we can just redirect stderr to file
- [ ] add debug logging to State, CraneControl, Crane data


Tests:
- [x] State
- [ ] Poller
- [ ] EventLoop
- [ ] CraneData  // using mocked CraneControl


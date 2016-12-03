Short overview of the classes in the provided ev3dev lego framework. All classes
have been implemented for the basic mechanical-arm lego set. Different
implementations of the classes should base on the provided ones. Classes should
be reimplemented to match the underlying lego set unless stated otherwise. For
the example usage of the framework see [draft.cpp](lego_fw/src/draft.cpp) file.

---

### Safe
`Safe` class is a wrapper around given data and corresponding mutex. It is
primarily used to provide multithread-safe access to the underlying hardware. It
provides overloaded dereference and arrow operators.

**Users should not change the implementation of the `Safe` class**

### CraneControl
Controller for the hardware. It should contain all hardware handles used
throughout application and corresponding mutexes. It should also implement
methods providing multithread-safe access to the underlying hardware by wrapping
hardware handle and corresponding mutex in the `Safe` class instance.

See [CraneControl.cpp](lego_fw/src/CraneControl.cpp) for an example implementation.

### CraneData
This class represents all data from external sensors, which is neccessary to
create state machine. It should implement two methods:
* `update` - updates all data fields
* `==` operator - used to compare old read with new one and indicate possible
    change in the reads. User might include eventual tollerance for chosen
    parameters.
Please note that `update` method will be run in a tight loop, therefore it is
crucial to read neccessary data only.

`CraneData` interacts with underlying hardware via `CraneControler` instance
provided as a reference in the constructor.

See [CraneData.cpp](lego_fw/src/CraneData.cpp) for an example implementation.

### Poller
Class responsible for polling the underlying hardware and providing
multithread-safe mechanism for acquiring the newest data.

**Users should not change the implementation of the `Poller` class**

### State
Defines state in the state machine. This class stores actions, which will be
executed once state will be reached. Actions are represented as functions
pointers. Transitions to the next state are stored as function pointers. Each
transition is a tuple of function pointer and next state. Functions should
return a boolean value indicating whether or not the transition should be made.
Transition functions get the newest data and elapsed time (duration of time
already spent in the current state) as their arguments. Each `State` can store
multiple actions and transitions.

**Users should not change the implementation of the `State` class**

### EventLoop
Representation of the state machine. Can be run from the given starting state
via `run` method. Machine can be stopped via `stop` method.

**Users should not change the implementation of the `EventLoop` class**

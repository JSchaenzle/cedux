<div>
  <img src='cedux.png'></img>
</div>

##### A Redux-like model for C

Cedux is a tool which enables developers to write clean C applications that don't require an operating system.

It works like this:
  - Application state is stored in a global tree which is created by the `CEDUX_DEFINE_STORE()` macro.
  - Producers of data (ISR, etc.) dispatch actions to the store via calls to `cedux_dispatch_x()`.
  - _Reducers_, which are registered during application initialization, receive dispatched actions, along with the current state tree, and are responsible for updating the state as needed.
  - _Subscribers_, which are also registered during application initialization, receive the new state tree after reducers process any actions.
  - _Linked Subscribers_ run any time the Reducer they are linked to does work.
  - In the `main` loop, calls to `cedux_run_x()` check for dispatched actions and forward them to all registered reducers.

## Recommendations
  - Try to only modify the state tree from within reducers.
  - Use a tagged-union for the action type (see `example.c`)
  - Use subscribers to perform side effects and dispact more actions if needed.

## Example
Take a look at `example.c` for a simple example usage.
You can compile and run it like this: `gcc example.c -o cedux.out && ./cedux.out`

## The Nitty-Gritty Details
### Cedux Usage

#### Create a Store
To use _Cedux_ you must first create a _store_. A store consists of just your state tree, a queue for holding dispatched actions and a list for holding registered reducers. 
The following macro is used to create a store:
`CEDUX_DEFINE_STORE(TREE_TYPE, ACTION_TYPE, STORE_NAME)`  
In this macro, `TREE_TYPE` is the type (structure definition) that describes your state tree.
`ACTION_TYPE` is the type that describes the actions that can be dispatched to the store.
`STORE_NAME` is used as the suffix to the generated store instance name and the suffix for the generated methods described below. In the following documentation, _x_ represents the store name.

For example, `CEDUX_DEFINE_STORE(struct my_app_state, struct action, my_store)` would create a store which contains a state tree of type `struct my_app_state`. Actions of type `struct action` could be dispatched to the store. After the macro, a variable `my_store` exists which is the store. The state tree is accessible via `my_store.tree`;

### Initialize the Store
To initialize the store call `cedux_init_x()`. This sets up the internal list and queue and returns the store.

### Register Reducers
`cedux_register_x_reducer(store, reducer)` where `store` is a pointer to the store created by `CEDUX_DEFINE_STORE` and `reducer` is a function pointer to a reducer function. The reducer function must have a signature of `void reducer(<tree type pointer>, action)`

### Register Generic Subscribers
`cedux_register_x_subscriber(store, subscriber, data)` where `store` is a pointer to the store created by `CEDUX_DEFINE_STORE`, `subscriber` is the subscriber function, and `data` is optional extra data to be passed with each call to the subscriber. The subscriber function must have a signature of `void subscriber(<store handle>, <tree type pointer>, void *data)`.  Cedux does not look at or modify `data` at all, so you can use it for whatever extra information you need, or just set it to `NULL` and ignore it.
A generic subscriber will get called any time any reducer does work.

### Register Linked Subscribers
`cedux_register_x_linked_subscriber(store, subscriber, data, reducer)` where `store` is a pointer to the store created by `CEDUX_DEFINE_STORE`, `subscriber` is the subscriber function, and `data` is optional extra data to be passed with each call to the subscriber, and reducer is the reducer to link it to. The subscriber function must have a signature of `void subscriber(<store handle>, <tree type pointer>, void *data)`.  Cedux does not look at or modify `data` at all, so you can use it for whatever extra information you need, or just set it to `NULL` and ignore it.
A linked subscriber will get called only when the reducer that it is linked to does work.

### Dispatch Actions
Call the dispatch function to send an action to the store. This method pushes the action into the stores action queue to be handled later by the run function.
`cedux_dispatch_x(store, action)` where `store` is a pointer to the store and `action` is a variable for `ACTION_TYPE`.

### Run
Somewhere in the main loop of your application you need to call the Cedux run function. This function checks if any actions have been dispatched and if so, sends them to all registered reducers.
`cedux_run_x(TStore * p_store)`

### Generated Code
As an example, `CEDUX_DEFINE_STORE(struct my_app_state, struct my_action, my_store)` would generate the following:

```
struct my_store_handle;  // The struct definition for the store handle
struct my_store_handle cedux_init_my_store(void); // store init function
void cedux_dispatch_my_store(struct my_store_handle* p_store, struct my_action_def action); // action dispatch function
bool cedux_run_my_store(struct my_store_handle* p_store); // run function
```

### Setup
To use Cedux you'll need to copy the following files into your application.
  - cedux.h
  - queue.h _(Used for the action queue)_
  - list.h  _(Used to hold the registered reducers)_

See example.c for a demonstration.

### Configuration
By default, cedux has the following maximum values set:
- Maximum of **16** actions can be dispatched between runs of the `cedux_run_store`
- Maximum of **32** reducers can be registered
- Maximum of **32** subscribers can be registered

However, these limitations can be altered if needed. Cedux references the following defines:
- CEDUX_MAX_ACTIONS
- CEDUX_MAX_REDUCERS
- CEDUX_MAX_SUBSCRIBERS

You must define these values before `#include`ing cedux.h like so:

```
#define CEDUX_MAX_ACTIONS 256      // Must be a factor of 2
#define CEDUX_MAX_REDUCERS 128     // Must be a factor of 2
#define CEDUX_MAX_SUBSCRIBERS 256  // Must be a factor of 2
#include "cedux.h"
```

As noted above, the CEXUX_MAX_X values must be factors of 2 in order for the cedux internals to work correctly.

### Thread Safety

For applications that require thread safety guarantees, Cedux provides the option to register platform-specific locking functions around the action queue so that multiple threads can dispatch actions at the time that Cedux is processing them.  If you don't need thread safety in your application, you can skip this step.  Use the `cedux_set_threadsafe_x` function to provide a handle to a platform-specific lock variable and wrapper functions to acquire and release the lock.  Note that the lock should be initialized before passing it to Cedux.  See `threadsafe_example.c` for an implementation using POSIX threads.


## Testing
This project is setup with unit testing using Ceedling (Unity and CMock)

### To setup testing environment
1. Setup rbenv. This will ensure the expected version or ruby is used.
2. Install bundler with: `gem install bundler`
3. Install gems with: `bundle install`

### To run the tests:
 > `bundle exec rake test:all`

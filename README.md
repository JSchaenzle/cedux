<div>
  <img src='cedux.png'></img>
</div>
##### A Redux-like model for C

Cedux is a tool which enables developer to write clean C applications that don't require an operating system.

It works like this:
  - Application state is stored in a global tree which is created by the `CEDUX_DEFINE_STORE()` macro.
  - Producers of data dispatch actions to the store via calls to `cedux_dispatch_x()`.
  - _Reducers_, which are registered during application initialization, receive dispatched actions and the current state tree and are responsible for updating the state as needed.
  - In the `main` loop, calls to `cedux_run_x()` check for dispatched actions and forward them to all registered reducers.

## Recommendations
  - Try to only modify the state tree from within reducers.
  - Use a tagged-union for the action type (see main.c)

## Example
Take a look at main.c for a simple example usage.

## The Nitty-Gritty Details
### Cedux Usage

#### Create a Store
To use _Cedux_ you must first create a _store_. A store consists of just your state tree, a queue for holding dispatched actions and a list for holding registered reducers. 
The following macro is used to create a store:
`CEDUX_DEFINE_STORE(TREE_TYPE, ACTION_TYPE, STORE_NAME)`  
In this macro, `TREE_TYPE` is the type (structure definition) that describes your state tree.
`ACTION_TYPE` is the type that describes the actions that can be dispatched to the store.
`STORE_NAME` is used as the suffix to the generated store instance name and the suffix for the generated methods described below. In the following documentation, _x_ represents the store name.

For example, `CEDUX_DEFINE_STORE(struct my_app_state, struct action, my_store)` would create a store which contains a state tree of type `struct my_app_state`. Actions of type `struct action` could be dispatched to the store. After the macro, a variable `my_store` exists which is the store.

### Initialize the Store
To initialize the store`cedux_init_x()`

#### Register Reducers
`cedux_register_x(store, reducer)` where `store` is a pointer to the store created by `CEDUX_DEFINE_STORE` and reducer is a function pointer to a reducer function. The reducer function must have a signature of `void reducer(<tree type pointer>, action)`

#### Dispatch Actions
Call the dispatch function to send an action to the store. This method pushes the action into the stores action queue to be handled later by the run function.
`cedux_dispatch_x(store, action)` where `store` is a pointer to the store and `action` is a variable for `ACTION_TYPE`.

#### Run
Somewhere in the main loop of your application you need to call the Cedux run function. This function checks if any actions have been dispatched and if so, pops them out of the action queue and sends them to all registered reducers.
`cedux_run_x(TStore * p_store)`

### Generated Code

`CEDUX_DEFINE_STORE(struct my_app_state, struct my_action, my_store)` generates the following:

```
struct my_store_handle my_store;  // The instance of the store
void cedux_init_my_store(struct my_store_handle* p_store); // store init function
void cedux_dispatch_my_store(struct my_store_handle* p_store, struct my_action_def action); // action dispatch function
bool cedux_run_my_store(struct my_store_handle* p_store); // run function
```



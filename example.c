#include "cedux.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>

void timer_handler (int signum);
void setup_timer(void);

// HEADER DETAILS *************************************************************************************
// Normally this code would be in a header file. For the sake of having a concise demo, I'm putting it
// all in one file.
//
struct branch_a { int leaves; };
struct tree { struct branch_a a; };

enum action_type {
  ADD_LEAVES,
  REMOVE_LEAVES,
  BURN_THE_TREE,
};

struct add_leaves_data { int count; };
struct subtract_leaves_data { int count; };

// Tagged union action type definition
struct my_action_def {
  enum action_type type;
  union {
    struct add_leaves_data add_leaves_data;
    struct subtract_leaves_data subtract_leaves_data;
  };
};

// This macro declares the store data structure as well as all of the functions which are needed
// to register reducers, and subscribers, and dispatch actions. It also declares the cedux_run_my_store
// function.
CEDUX_DECLARE_STORE(struct tree, struct my_action_def, my_store);

//
//  END OF HEADER CONTENT
// ****************************************************************************************************


// This macro defines the implementation of the functions which are used to register reducers,
// subscribers, and dispatch actions. It also defines the cedux_run_my_store function.
CEDUX_DEFINE_STORE(struct tree, struct my_action_def, my_store);

struct my_store_handle my_store;

bool primary_reducer(struct tree * p_tree, struct my_action_def action) 
{
  // Reducers are the ONLY place where the state tree should be modified.
  // Return true if the state is updated. Otherwise return false.
  switch (action.type)
  {
    case ADD_LEAVES:
      p_tree->a.leaves += action.add_leaves_data.count;
      return true;
    case REMOVE_LEAVES:
      p_tree->a.leaves -= action.subtract_leaves_data.count;
      if (p_tree->a.leaves < 0) { p_tree->a.leaves = 0; }
      return true;
    default:
      return false;
  } 
}

bool burn_tree_reducer(struct tree * p_tree, struct my_action_def action)
{
  // Reducers are the ONLY place where the state tree should be modified.
  // Return true if the state is updated. Otherwise return false.
  if (action.type == BURN_THE_TREE) {
    p_tree->a.leaves = 0;
    return true;
  } else {
    return false;
  }
}

void generic_subscriber(struct my_store_handle * p_store, struct tree const * const p_tree, void *data)
{
  // Subscribers are for performing side-effects. Ideally they should be pure functions which perform some
  // action as a result of a state change.
  // The store handle is passed in so that you can dispatch new actions within here if you want to.
  // The state tree is passed in so that you can reference the state. It's const so you shouldn't modify it.
  int number = (int)data;
  printf("Generic Subscriber.  Num leaves %d! Data: %d:\n", p_tree->a.leaves, number);
}

void linked_subscriber(struct my_store_handle * p_store, struct tree const * const p_tree, void *data)
{
  int number = (int)data;
  printf("The tree burned down!!  Num leaves %d. Data: %d:\n", p_tree->a.leaves, number);
}

int main(void)
{
  my_store = cedux_init_my_store(); // Initialize the internals of the store (list, queue)

  // Setup initial state.
  my_store.tree.a.leaves = 1000;

  // Register reducers
  cedux_register_my_store_reducer(&my_store, primary_reducer);
  cedux_register_my_store_reducer(&my_store, burn_tree_reducer);

  // Register a generic subscriber. Generic subscribers will always run any time any reducer does work.
  cedux_register_my_store_subscriber(&my_store, generic_subscriber, (void *)1);
  // You can register additional subscribers which get a unique "context". That context will always be
  // baseed to the subscriber.
  cedux_register_my_store_subscriber(&my_store, generic_subscriber, (void *)2);
  // You can also register a linked subscriber that will only get called if the 
  // reducer that it is linked to does work.
  cedux_register_my_store_linked_subscriber(&my_store, linked_subscriber, (void *)3, burn_tree_reducer);

  setup_timer(); // Setup for demo

  while(1) {
    bool did_work = cedux_run_my_store(&my_store); // REQUIRED. You must call this every run of your main loop. This is where all the magic happens.
    if (did_work) {
      printf("A reducer did work.\n");
    }
  } 
}

void timer_handler (int signum)
{
  // Create an action and fill in the data based on some randomness...
  int random_val = rand();
  printf("Random num: %d\n", random_val);
  struct my_action_def action;
  action.type = rand() % 2 != 1 ? ADD_LEAVES : REMOVE_LEAVES;
  // Every so often burn down the tree!
  action.type = (random_val % 4 == 0) ? BURN_THE_TREE : action.type;
  if (action.type == ADD_LEAVES) {
    action.add_leaves_data.count = random_val % 100;
  } else if (action.type == REMOVE_LEAVES) {
    action.subtract_leaves_data.count = random_val % 100;
  }

  // Dispatch the action
  cedux_dispatch_my_store(&my_store, action);
}

void setup_timer(void)
{
   struct sigaction sa;
   struct itimerval timer;
   /* Install timer_handler as the signal handler for SIGVTALRM. */
   memset (&sa, 0, sizeof (sa));
   sa.sa_handler = &timer_handler;
   sigaction (SIGVTALRM, &sa, NULL);
   /* Configure the timer to expire after 250 msec... */
   timer.it_value.tv_sec = 0;
   timer.it_value.tv_usec = 250000;
   /* ... and every 250 msec after that. */
   timer.it_interval.tv_sec = 0;
   timer.it_interval.tv_usec = 250000;
   /* Start a virtual timer. It counts down whenever this process is executing. */
   setitimer (ITIMER_VIRTUAL, &timer, NULL);
}

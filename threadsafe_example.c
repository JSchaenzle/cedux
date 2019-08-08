/**
 * threadsafe_example.c
 *
 * This file provides an example of setting up Cedux for thread safety in a
 * multithreaded environment.  This particular implementation uses POSIX
 * threads, however Cedux is designed to be portable, and this code could be
 * used as a template for setting up thread safety in another environment.
 */

#include "cedux.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// State tree
struct branch_a { int leaves; };
struct tree { struct branch_a a; };

// Available actions
enum action_type {
  ADD_LEAVES,
  REMOVE_LEAVES,
};

// Action definition type
struct add_leaves_data { int count; };
struct subtract_leaves_data { int count; };
struct my_action_def {
  enum action_type type;
  union {
    struct add_leaves_data add_leaves_data;
    struct subtract_leaves_data subtract_leaves_data;
  };
};

bool reducer(struct tree * p_tree, struct my_action_def action) 
{
  switch (action.type)
  {
    case ADD_LEAVES:
      p_tree->a.leaves += action.add_leaves_data.count;
      return true;
    case REMOVE_LEAVES:
      p_tree->a.leaves -= action.add_leaves_data.count;
      return true;
      break;
    default:
      return false;
  } 
}

CEDUX_DECLARE_STORE(struct tree, struct my_action_def, my_store);
CEDUX_DEFINE_STORE(struct tree, struct my_action_def, my_store);
struct my_store_handle my_store;

// Because the lock type is platform-specific, we have to initialize it
// ourselves before passing it to Cedux.
pthread_mutex_t store_mutex = PTHREAD_MUTEX_INITIALIZER;

// These are wrappers around the platform-specific mutex functions
void mutex_lock(void *mutex) { pthread_mutex_lock((pthread_mutex_t *)mutex); }
void mutex_unlock(void *mutex) { pthread_mutex_unlock((pthread_mutex_t *)mutex); }

#define NUM_THREADS 5

void *thread_func(void *data)
{
  int thread_id = (int)data;
  while (1) {
    sleep(rand() % 3 + 1);
    struct my_action_def action = {
      .type = rand() % 2 != 1 ? ADD_LEAVES : REMOVE_LEAVES,
      .add_leaves_data = {
          .count = rand() % 100
      }
    };
    printf("Thread %d: dispatching to add %d leaves.\n", thread_id, action.add_leaves_data.count);
    cedux_dispatch_my_store(&my_store, action);
  }
}

int main(void)
{
  // Init Cedux
  my_store = cedux_init_my_store();
  cedux_register_my_store_reducer(&my_store, reducer);

  // This function tells Cedux that it should call our mutex wrapper functions
  // with the provided lock handle (&store_mutex) before reading or writing to
  // the action queue.
  cedux_set_threadsafe_my_store(&my_store, &store_mutex, mutex_lock, mutex_unlock);

  // Start threads
  pthread_t threads[NUM_THREADS];
  for (size_t i = 0; i < NUM_THREADS; i++) {
    printf("Starting thread %zu...\n", i);
    pthread_create(&threads[i], NULL, thread_func, (void *)i);
  }

  // Run the my_store reducer loop
  while(1) {
    bool did_work = cedux_run_my_store(&my_store);
    if (did_work) {
      printf("Main Thread: num leaves = %d!\n", my_store.tree.a.leaves);
    }
  } 
}


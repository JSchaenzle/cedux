#include "cedux.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>

void timer_handler (int signum);
void setup_timer(void);

struct branch_a { int leaves; };
struct tree { struct branch_a a; };

enum action_type {
  ADD_LEAVES,
  REMOVE_LEAVES,
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

CEDUX_DEFINE_STORE(struct tree, struct my_action_def, my_store)

void reducer_1(struct tree * p_tree, struct my_action_def action) 
{
  switch (action.type)
  {
    case ADD_LEAVES:
      p_tree->a.leaves += action.add_leaves_data.count;
      break;
    case REMOVE_LEAVES:
      p_tree->a.leaves -= action.add_leaves_data.count;
      break;
    default:
      break;
  } 
}

int main(void)
{
  cedux_init_my_store(&my_store);
  cedux_register_my_store_reducer(&my_store, reducer_1);
  setup_timer();

  while(1) {
    bool did_work = cedux_run_my_store(&my_store);
    if (did_work) {
      printf("Num leaves %d!\n", my_store.tree.a.leaves);
    }
  } 
}

void timer_handler (int signum)
{
  cedux_dispatch_my_store(&my_store, (struct my_action_def){
    .type = rand() % 2 != 1 ? ADD_LEAVES : REMOVE_LEAVES,
    .add_leaves_data = {
        .count = rand() % 100
    }
  });
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

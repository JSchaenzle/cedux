#include "cedux.h"
#include <stdio.h>

struct branch_a {
  int leaf_a1;
};

struct tree {
  struct branch_a a;
};

enum action_type
{
  ADD_LEAVES,
  REMOVE_LEAVES,
};

struct add_leaves_data {
  int count;
};

struct subtract_leaves_data {
  int count;
};

struct my_action_def {
  enum action_type type;
  union {
    struct add_leaves_data add_leaves_data;
    struct subtract_leaves_data subtract_leaves_data;
  };
};

CEDUX_DEFINE_STORE(struct tree, struct my_action_def, my_tree)
// struct my_tree store;

void reducer_1(struct tree * p_tree, struct my_action_def action)
{
  printf("In my reducer!\n");
}

int main(void)
{
  cedux_init_my_tree(&store);

  cedux_register_my_tree_reducer(&store, reducer_1);

  struct my_action_def action1 = {
    .type = ADD_LEAVES,
    .add_leaves_data = {
        .count = 5
    }
  };

  cedux_dispatch(&store, action1);

}


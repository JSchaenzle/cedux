#ifndef CEDUX_TEXT_HELPERS_H
#define CEDUX_TEXT_HELPERS_H
#include <stdbool.h>

bool mockable_reducer1(int * p_state, int action);
bool mockable_reducer2(int * p_state, int action);
// p_store is a void pointer here because we don't have access to the correct store handle type... We'll have to use a cast in the test when registering this.
void mockable_subscriber1(void * p_store, int const * const p_state, void * p_data);
void mockable_subscriber2(void * p_store, int const * const p_state, void * p_data);

#endif
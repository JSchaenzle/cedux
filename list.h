#ifndef LIST_H
#define LIST_H
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

enum push_result {
  PUSH_RESULT_SUCCESS,
  PUSH_RESULT_FULL,
};

#define ARRAY_LENGTH(A) (sizeof(A)/sizeof((A)[0])) 

#define LIST_DECLARATION(NAME, ITEM_TYPE, NUM_ITEMS)                                   \
struct NAME {                                                                          \
  uint16_t length;                                                                     \
  ITEM_TYPE items[NUM_ITEMS];                                                          \
};                                                                                     \
void NAME ## _init(struct NAME * p_list);                                              \
enum push_result NAME ##_push(struct NAME * p_list, ITEM_TYPE * p_new_item);           \
bool NAME ##_is_empty(struct NAME * p_list);

#define LIST_DEFINITION(NAME, ITEM_TYPE)                                               \
void NAME ## _init(struct NAME * p_list)                                               \
{                                                                                      \
  p_list->length = 0;                                                                  \
}                                                                                      \
                                                                                       \
enum push_result NAME ##_push(struct NAME  * p_list, ITEM_TYPE * p_new_item) {         \
  if (p_list->length == ARRAY_LENGTH(p_list->items)) {                                 \
    return PUSH_RESULT_FULL;                                                           \
  }                                                                                    \
                                                                                       \
  p_list->items[p_list->length] = *p_new_item;                                         \
  p_list->length++;                                                                    \
  return PUSH_RESULT_SUCCESS;                                                          \
}                                                                                      \
                                                                                       \
bool NAME ##_is_empty(struct NAME * p_list) {                                          \
  return p_list->length == 0;                                                          \
}

#define LIST_FOR_EACH(LIST, ITEM) for(int _i = 0; ITEM=LIST.items[_i], _i < LIST.length; _i++) 


#endif

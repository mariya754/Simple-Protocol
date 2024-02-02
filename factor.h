#include <stdint.h>
#include <stdlib.h>

/*
 * This defines a structure to hold an element of the linked list.
 */
typedef struct factor_link {
  uint32_t value;
  struct factor_link* next;
} factor_link_t;

/*
 * This defines a linked list structure.
 * The structure tracks the total number of items in the list, with "length".
 * It is implemented as a stack, with new entries added to the head.
 */
typedef struct factor_list {
  uint16_t length;
  factor_link_t* head;
} factor_list_t;

/*
 * Factor a 32-bit unsigned integer.
 *
 * work   The number to factor.
 *
 * Returns a new linked list with the factors, or NULL if there's an error.
 * The caller is responsible for freeing the memory.
 */
factor_list_t* factor(uint32_t work);

/*
 * Frees a linked list, including all of its items.
 */
void delete_list(factor_list_t* list);

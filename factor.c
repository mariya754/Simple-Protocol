#include "factor.h"

static factor_list_t* new_list() {
  factor_list_t* res = (factor_list_t*) malloc( sizeof(factor_list_t) );
  if ( NULL == res ) {
    return res;
  }

  res->length = 0;
  res->head = NULL;

  return res;
}

static factor_link_t* new_factor(uint32_t value) {
  factor_link_t* new_link = (factor_link_t*) malloc( sizeof(factor_link_t) );
  if ( NULL == new_link ) return NULL;

  new_link->value = value;
  new_link->next = NULL;

  return new_link;
}

static int add_factor(factor_list_t* list, uint32_t value) {
  if ( NULL == list ) return 0;

  factor_link_t* new_link = new_factor(value);
  if ( NULL == new_link ) return 0;

  new_link->next = list->head;
  list->head = new_link;
  list->length++;
  return 1;
}

void delete_list(factor_list_t* list) {
  if ( NULL == list ) return;

  while ( NULL != list->head ) {
    factor_link_t* old = list->head;
    list->head = old->next;
    free(old);
  }

  free(list);
}

factor_list_t* factor(uint32_t work) {
  factor_list_t* list = new_list();
  if ( NULL == list ) {
    return NULL;
  }

  uint32_t f1 = 1;
  while ( f1 * f1 <= work ) {
    if ( 0 == (work % f1) ) {
      if ( ! add_factor(list,f1) ) {
	delete_list(list);
	return NULL;
      }

      uint32_t f2 = work / f1;
      if ( f2 != f1 ) {
        if ( ! add_factor(list,f2) ) {
	  delete_list(list);
	  return NULL;
	}
      }
    }
    f1++;
  }

  return list;
}

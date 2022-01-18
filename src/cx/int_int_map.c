
#include "associa.h"

static Sign cmp_int_int(int* a, int* b) {
  if (*a < *b) {return -1;}
  if (*a == *b) {return 0;}
  return 1;
}

void* make_FildeshLegacyIntIntMap() {
  Associa* map = (Associa*) malloc(sizeof(Associa));
  InitAssocia( int, int, *map, cmp_int_int );
  return map;
}

void set_FildeshLegacyIntIntMap(void* arg, int k, int v) {
  Associa* map = (Associa*) arg;
  Assoc* a = ensure_Associa(map, &k);
  val_fo_Assoc(map, a, &v);
}

int* get_FildeshLegacyIntIntMap(void* arg, int k) {
  Associa* map = (Associa*) arg;
  Assoc* a = lookup_Associa(map, &k);
  if (!a) {return NULL;}
  return (int*) val_of_Assoc(map, a);
}

void remove_FildeshLegacyIntIntMap(void* arg, int k) {
  Associa* map = (Associa*) arg;
  remove_Associa(map, &k);
}

void free_FildeshLegacyIntIntMap(void* arg) {
  Associa* map = (Associa*) arg;
  lose_Associa(map);
  free(map);
}

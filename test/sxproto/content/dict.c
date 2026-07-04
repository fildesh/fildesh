#include <fildesh/sxproto.h>

#include "src/sxproto/value.h"

FildeshSxpb* make_dict_test_FildeshSxpb(void) {
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT top = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT dict_it;
  FildeshSxpbIT mesg_it;
  FildeshSxpbIT it;

  dict_it = direct_insert_first_FildeshSxpb(
      sxpb, top,
      ensure_name_FildeshSxpb(sxpb, "my_dict", 7),
      FildeshSxprotoFieldKind_DICT);
  mesg_it = direct_insert_first_FildeshSxpb(
      sxpb, dict_it,
      ensure_name_FildeshSxpb(sxpb, "key1", 4),
      FildeshSxprotoFieldKind_MESSAGE);
  it = direct_insert_first_FildeshSxpb(
      sxpb, mesg_it,
      ensure_name_FildeshSxpb(sxpb, "value", 5),
      FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(
      sxpb, it,
      ensure_name_FildeshSxpb(sxpb, "val1", 4),
      FildeshSxprotoFieldKind_LITERAL_STRING);
  mesg_it = direct_insert_next_FildeshSxpb(
      sxpb, mesg_it,
      ensure_name_FildeshSxpb(sxpb, "key2", 4),
      FildeshSxprotoFieldKind_MESSAGE);
  it = direct_insert_first_FildeshSxpb(
      sxpb, mesg_it,
      ensure_name_FildeshSxpb(sxpb, "subkey", 6),
      FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(
      sxpb, it,
      ensure_name_FildeshSxpb(sxpb, "subval", 6),
      FildeshSxprotoFieldKind_LITERAL_STRING);
  direct_insert_next_FildeshSxpb(
      sxpb, dict_it,
      ensure_name_FildeshSxpb(sxpb, "empty_dict", 10),
      FildeshSxprotoFieldKind_DICT);

  return sxpb;
}

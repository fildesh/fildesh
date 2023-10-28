#include <fildesh/sxproto.h>

#include <assert.h>

static void create_test() {
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT pos_m, pos_e;

  pos_m = ensure_field_at_FildeshSxpb(
      sxpb, top_of_FildeshSxpb(sxpb),
      "m",
      FildeshSxprotoFieldKind_MESSAGE);
  assert(!nullish_FildeshSxpbIT(pos_m));

  pos_e = ensure_field_at_FildeshSxpb(
      sxpb, pos_m, "i",
      FildeshSxprotoFieldKind_LITERAL);

  pos_e = ensure_field_at_FildeshSxpb(
      sxpb, pos_m, "f",
      FildeshSxprotoFieldKind_LITERAL);

  pos_e = ensure_field_at_FildeshSxpb(
      sxpb, pos_m, "s",
      FildeshSxprotoFieldKind_LITERAL);

  pos_e = lookup_subfield_at_FildeshSxpb(sxpb, pos_m, "s");
  assert(!nullish_FildeshSxpbIT(pos_e));

  close_FildeshSxpb(sxpb);
}

int main() {
  create_test();
  return 0;
}

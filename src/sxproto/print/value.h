#ifndef FILDESH_SXPROTO_PRINT_VALUE_H_
#define FILDESH_SXPROTO_PRINT_VALUE_H_
#include "src/sxproto/value.h"

void print_quoted_sxpb_str_FildeshO(FildeshO* out, const char* s);
void print_sxpb_literal_value_FildeshO(FildeshO* out, const FildeshSxprotoValue* e);
void print_json_literal_value_FildeshO(FildeshO*, const FildeshSxprotoValue*);

#endif

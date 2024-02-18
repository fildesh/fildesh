#include "test/fuzz/smoke_common.h"

BEGIN_FUZZ_DATA
"\n"
"(i 5)(f 5.5)(s \"five\")(m)"
NEXT_FUZZ_DATA
"1,9,"
"Unknown escape sequence. Only very basic ones are supported.\n"
"(\"a\" (\"b\\.\" 5))"
NEXT_FUZZ_DATA
"1,8,"
"Expected subfield name to be quoted too.\n"
"(\"a\" (b 5))"
NEXT_FUZZ_DATA
"1,4,"
"Expected 2 closing parens after manyof name; got 0.\n"
"(((("
NEXT_FUZZ_DATA
"1,6,"
"Expected 2 closing parens after manyof name; got 1.\n"
"(((u)"
NEXT_FUZZ_DATA
"1,3,"
"Expected closing paren after array name.\n"
"(("
NEXT_FUZZ_DATA
"1,2,"
"Denote empty message in array as (()), not ().\n"
"("
NEXT_FUZZ_DATA
"1,7,"
"Denote empty message in array as (()), not ().\n"
"((a) ())"
NEXT_FUZZ_DATA
"1,6,"
"Literal field can only hold 1 value.\n"
"(k 1 2)"
NEXT_FUZZ_DATA
"1,6,"
"Literal field can only hold 1 value.\n"
"(k 5 (w"
NEXT_FUZZ_DATA
"1,5,"
"Expected closing double quote.\n"
"(k \""
NEXT_FUZZ_DATA
"1,4,"
"Message can only hold fields.\n"
"(()"
NEXT_FUZZ_DATA
"1,3,"
"Expected a literal or closing paren.\n"
"(a"
NEXT_FUZZ_DATA
"1,5,"
"Expected a literal or closing paren.\n"
"((x)\0"
NEXT_FUZZ_DATA
"1,1,"
"Expected open paren to start field.\n"
"a"
NEXT_FUZZ_DATA
"1,1,"
"Expected open paren to start field.\n"
")"
NEXT_FUZZ_DATA
"1,7,"
"Cannot parse exponent.\n"
"(x 5e+bad)"
NEXT_FUZZ_DATA
"1,12,"
"Manyof cannot hold nameless message values yet.\n"
"(((a)) (() (x 5)))"
NEXT_FUZZ_DATA
"1,10,"
"Duplicate field name. Use array syntax for repeated fields.\n"
"(x 5) (x 6)"
NEXT_FUZZ_DATA
"1,8,"
"Unexpected message.\n"
"((a) 5 (() (a 1)))"
NEXT_FUZZ_DATA
"1,15,"
"Unexpected literal type.\n"
"((a) 5 6 7 \"8\")"
NEXT_FUZZ_DATA
"1,19,"
"Unexpected literal type.\n"
"((a) \"5\" \"6\" \"7\" 8)"
END_FUZZ_DATA

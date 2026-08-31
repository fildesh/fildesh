#include "test/fuzz/smoke_common.h"

BEGIN_FUZZ_DATA
"\n"
"(i 5)(f 5.5)(s \"five\")(m)"
NEXT_FUZZ_DATA
"1,9,"
"Unknown escape sequence. Only very basic ones are supported.\n"
"(\"a\" (\"b\\.\" 5))"
NEXT_FUZZ_DATA
"1,6,"
"Expected closing double quote.\n"
"(k \"v"
NEXT_FUZZ_DATA
"1,14,"
"Unexpected anonymous subnest outside of a nest.\n"
"(a (()) ((\"\") subnest))"
NEXT_FUZZ_DATA
"1,6,"
"Expected closing paren after loneof selection name.\n"
"((x y"
NEXT_FUZZ_DATA
"1,4,"
"Expected closing paren after loneof selection name.\n"
"((x()"
NEXT_FUZZ_DATA
"1,7,"
"Expected closing paren after loneof selection name.\n"
"((x y z)"
NEXT_FUZZ_DATA
"1,6,"
"Unexpected space between opening and closing parentheses.\n"
"((  ) (x 5))"
NEXT_FUZZ_DATA
"1,3,"
"Expected loneof field name.\n"
"(("
NEXT_FUZZ_DATA
"1,3,"
"Expected loneof field name.\n"
"((()) ((()) 6 6 7))"
NEXT_FUZZ_DATA
"1,6,"
"Literal field can only hold 1 value.\n"
"(k 1 2)"
NEXT_FUZZ_DATA
"1,6,"
"Literal field can only hold 1 value.\n"
"(k 5 (w"
NEXT_FUZZ_DATA
"1,13,"
"Message can only hold fields.\n"
"(a (()) (() 7 7 7))"
NEXT_FUZZ_DATA
"1,7,"
"Dict items must be key-value pairs even when the value is empty.\n"
"(x () \"*\")"
NEXT_FUZZ_DATA
"1,6,"
"Unexpected open paren in string.\n"
"(a b ("
NEXT_FUZZ_DATA
"1,1,"
"Expected open paren to start field.\n"
"a"
NEXT_FUZZ_DATA
"1,1,"
"Expected open paren to start field.\n"
")"
NEXT_FUZZ_DATA
"1,6,"
"Expected some digit in number.\n"
"(x -.)"
NEXT_FUZZ_DATA
"1,6,"
"Expected some digit in number.\n"
"(x +.)"
NEXT_FUZZ_DATA
"1,7,"
"Cannot parse exponent.\n"
"(x 5e+bad)"
NEXT_FUZZ_DATA
"1,10,"
"Duplicate field name. Use array syntax for repeated fields.\n"
"(x 5) (x 6)"
NEXT_FUZZ_DATA
"1,6,"
"Unexpected array discriminator as array element.\n"
"(()) (())"
NEXT_FUZZ_DATA
"1,14,"
"Unexpected array discriminator as manyof element.\n"
"((my_manyof) (()))"
NEXT_FUZZ_DATA
"1,9,"
"Unexpected nest discriminator as array element.\n"
"(a (()) (\"\"))"
NEXT_FUZZ_DATA
"1,6,"
"Unexpected nest discriminator as manyof element.\n"
"((a) (\"\"))"
NEXT_FUZZ_DATA
"1,11,"
"Unexpected string array element.\n"
"(a (()) 5 (\"\" string))"
NEXT_FUZZ_DATA
"1,8,"
"Unexpected string manyof element.\n"
"((a) 5 (\"\" string))"
NEXT_FUZZ_DATA
"1,11,"
"Unexpected message array element.\n"
"(a (()) 5 (() (a 1)))"
NEXT_FUZZ_DATA
"1,26,"
"Manyof cannot mix anonymous element types.\n"
"((my_manyof) 5 (() (a 1)))"
NEXT_FUZZ_DATA
"2,7,"
"Nest can only hold nests and strings.\n"
"(\"\")\n((a) b)"
NEXT_FUZZ_DATA
"1,15,"
"Dict cannot hold list-like or dict values.\n"
"(d () (a (()) 1))"
NEXT_FUZZ_DATA
"1,12,"
"Dict cannot hold list-like or dict values.\n"
"(d () ((a) 1))"
NEXT_FUZZ_DATA
"1,15,"
"Dict cannot hold list-like or dict values.\n"
"(d () (a (\"\") b))"
NEXT_FUZZ_DATA
"1,13,"
"Dict cannot hold list-like or dict values.\n"
"(d () (a () (b c)))"
NEXT_FUZZ_DATA
"1,22,"
"Expected a bool, not an int.\n"
"(d () (a +true) (b 2))"
NEXT_FUZZ_DATA
"1,20,"
"Unexpected literal type.\n"
"(d () (a 1) (b \"x\"))"
NEXT_FUZZ_DATA
"1,18,"
"Unexpected literal type.\n"
"(a (()) 5 6 7 \"8\")"
NEXT_FUZZ_DATA
"1,4,"
"Expected a literal or closing paren.\n"
"(()"
NEXT_FUZZ_DATA
"1,3,"
"Expected a literal or closing paren.\n"
"(a"
NEXT_FUZZ_DATA
"1,4,"
"Expected a literal or closing paren.\n"
"(a\0"
END_FUZZ_DATA

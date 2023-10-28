#include "test/fuzz/smoke_common.h"

BEGIN_FUZZ_DATA
/* "Expected 2 closing parens after manyof name; got 0."
 */
"(((("
NEXT_FUZZ_DATA
/* "Expected 2 closing parens after manyof name; got 1."
 */
"(((u)"
NEXT_FUZZ_DATA
/* "Expected closing paren after array name."
 */
"(("
NEXT_FUZZ_DATA
/* "Message expects named fields inside it."
 */
"("
NEXT_FUZZ_DATA
/* "Expected a scalar but got a field."
 */
"(k 5 (w"
NEXT_FUZZ_DATA
/* "Expected closing double quote."
 */
"(k 5 \""
NEXT_FUZZ_DATA
/* "Expected a field but got a scalar."
 */
"(()"
NEXT_FUZZ_DATA
/* "Expected close paren but got EOF."
 */
"(a"
NEXT_FUZZ_DATA
/* "Expected open paren to start message but got:"
 */
"a"
NEXT_FUZZ_DATA
/* "Got extra closing paren at top level."
 */
")"
NEXT_FUZZ_DATA
/* "Expected a literal or closing paren."
 */
"((x)\0"
END_FUZZ_DATA

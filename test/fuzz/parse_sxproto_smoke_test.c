#include "test/fuzz/smoke_common.h"

BEGIN_FUZZ_DATA
/* "Expected manyof name."
 */
"(((("
NEXT_FUZZ_DATA
/* "Expected closing paren for manyof."
 */
"((("
NEXT_FUZZ_DATA
/* "Expected closing paren."
 */
"(("
NEXT_FUZZ_DATA
/* "Expected field name."
 */
{ '(' }
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
END_FUZZ_DATA

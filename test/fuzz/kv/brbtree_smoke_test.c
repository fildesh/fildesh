#include "test/fuzz/smoke_common.h"

BEGIN_FUZZ_DATA
/* Coverage in insert_0_root_sblack():
 *  ... if (...) {...}
 */
{ 2, 1 }
NEXT_FUZZ_DATA
/* Coverage in insert_0_root_sblack():
 *  ... if (...) {
 */
{ 202, 201 }
NEXT_FUZZ_DATA
/* Coverage in insert_1_root_sblack():
 *  ... if (...) {...}
 */
{ 1, 2 }
NEXT_FUZZ_DATA
/* Coverage in insert_1_root_sblack():
 *  ... if (...) {
 */
{ 201, 202 }
NEXT_FUZZ_DATA
/* Coverage in insert_0_d2_bblack().*/
{ 2, 3, 1 }
NEXT_FUZZ_DATA
/* Coverage in insert_0_d2_sblack().*/
{ 2, 201, 1 }
NEXT_FUZZ_DATA
/* Coverage in insert_1_d2_bblack().*/
{ 1, 3, 2 }
NEXT_FUZZ_DATA
/* Coverage in insert_1_d2_sred().*/
{ 1, 201, 2 }
NEXT_FUZZ_DATA
/* Coverage in insert_2_d2_bblack():
 *  ... if (...) {...}
 */
{ 1, 2, 3 }
NEXT_FUZZ_DATA
/* Coverage in insert_2_d2_bblack():
 *  ... if (...) {
 */
{ 1, 2, 201 }
NEXT_FUZZ_DATA
/* Coverage in insert_2_d2_sred().*/
{ 1, 201, 202 }
NEXT_FUZZ_DATA
/* Coverage in insert_0_root3_sblack().*/
{ 2, 3, 4, 1 }
NEXT_FUZZ_DATA
/* Coverage in insert_0_root3_sred().*/
{ 2, 3, 201, 1 }
NEXT_FUZZ_DATA
/* Coverage in lshins_1_d3_rred(), insert_0_root3_sblack().*/
{ 1, 3, 4, 2 }
NEXT_FUZZ_DATA
/* Coverage in lshins_2_d3_rred(), insert_0_root3_sblack().*/
{ 1, 2, 4, 3 }
NEXT_FUZZ_DATA
/* Coverage in lshins_3_d3_rred(), insert_0_root3_sblack().*/
{ 1, 2, 3, 4 }
NEXT_FUZZ_DATA
/* Coverage in lshins_1_d3_sred(), insert_0_root3_sred().*/
{ 1, 3, 201, 2 }
NEXT_FUZZ_DATA
/* Coverage in lshins_2_d3_sred(), insert_0_root3_sred().*/
{ 1, 2, 202, 201 }
NEXT_FUZZ_DATA
/* Coverage in lshins_3_d3_sred(), insert_0_root3_sred().*/
{ 1, 2, 201, 202 }
NEXT_FUZZ_DATA
/* Coverage in lshins_3_d3_rred():
 *  ... else {
 */
{ 1, 2, 3, 204 }
NEXT_FUZZ_DATA
/* Coverage in insert_0_root4_sblack()
 *  ... if (Nullish(SplitOf(y, 0))) {
 */
{ 3, 4, 5, 2, 1 }
NEXT_FUZZ_DATA
/* Coverage in insert_0_root4_sblack()
 *  ... if (Nullish(SplitOf(y, 0))) {...}
 */
{ 3, 4, 201, 2, 1 }
NEXT_FUZZ_DATA
/* Coverage in insert_FildeshKV_BRBTREE():
 *  ... if (Nullish(SplitOf(a, 0))) {
 */
{ 1, 2, 3, 4, 5 }
NEXT_FUZZ_DATA
/* Coverage in insert_FildeshKV_BRBTREE():
 *  ... if (detect_d2_subtree(...)) {... if (IsBroadLeaf(...)) {...} else {
 */
{ 1, 2, 3, 4, 5, 6, 7 }
NEXT_FUZZ_DATA
/* Coverage in rshins_2_d3_rred().*/
{ 7, 6, 5, 4, 1, 2, 3 }
NEXT_FUZZ_DATA
/* Coverage in rshins_1_d3_rred().*/
{ 7, 6, 5, 4, 3, 1, 2 }
NEXT_FUZZ_DATA
/* Coverage in rshins_0_d3_sblack().*/
{ 7, 6, 5, 4, 3, 2, 1 }
NEXT_FUZZ_DATA
/* Coverage in rshins_2_d3_sred().*/
{ 207, 206, 205, 204, 201, 202, 203 }
NEXT_FUZZ_DATA
/* Coverage in rshins_2_d3_sred():
 *  ... if (...) {
 */
{ 207, 206, 205, 204, 1, 2, 3 }
NEXT_FUZZ_DATA
/* Coverage in rshins_1_d3_sred():
 *  ... if (Nullish(y)) {
 */
{ 207, 206, 205, 204, 201, 203, 202 }
NEXT_FUZZ_DATA
/* Coverage in rshins_1_d3_sred():
 *  ... if (Nullish(y)) {...} else {
 */
{ 207, 206, 205, 204, 1, 3, 2 }
NEXT_FUZZ_DATA
/* Coverage in rshins_0_d3_sred():
 *  ... if (Nullish(y)) {
 */
{ 207, 206, 205, 204, 203, 202, 201 }
NEXT_FUZZ_DATA
/* Coverage in rshins_0_d3_sred():
 *  ... if (Nullish(y)) {...} else {
 */
{ 207, 206, 205, 204, 3, 2, 1 }
NEXT_FUZZ_DATA
/* Coverage in insert_FildeshKV_BRBTREE():
 *  ... lsh_d3(...); ... rsh_d3(...);
 */
{ 1, 2, 3, 4, 5, 6, 7, 8 }
NEXT_FUZZ_DATA
/* Coverage in lsh_d3():
 *  ... else {
 * Coverage in rsh_d3():
 *  ... else {
 */
{ 201, 202, 203, 204, 205, 206, 207, 208 }
NEXT_FUZZ_DATA
/* Coverage in rsh_d3():
 *  ... else if (map->at[b].size <= FildeshKVE_splitksize_max) {
 */
{ 1, 2, 203, 204, 205, 206, 207, 208 }
NEXT_FUZZ_DATA
/* Coverage in insert_FildeshKV_BRBTREE():
 *  ... if (RedColorOf(SplitOf(a, 1-b_side))) {... else if (IsBroadLeaf(y)) {
 */
{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }
NEXT_FUZZ_DATA
/* Coverage in insert_FildeshKV_BRBTREE():
 *  ... if (RedColorOf(SplitOf(a, 1-b_side))) {... else {
 */
{ 201, 202, 203, 204, 205, 206, 207, 208, 209, 210 }
NEXT_FUZZ_DATA
/* Coverage in fixup_insert_FildeshKV_RBTREE():
 *  ... while (true) {... if (...) {... if (bubbling_insertion) {
 */
{ 4, 11, 5, 1, 9, 10, 3, 8, 7, 2, 6 }
END_FUZZ_DATA

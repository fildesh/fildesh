#include "kve.h"
#include <assert.h>

static const FildeshKV_id_t FildeshKV_NULL_ID = ~(size_t)0;
static const FildeshKV_id_t FildeshKV_NULL_INDEX = ~(size_t)0 >> 3;

#define assert_trivial_joint(joint)  assert(joint == get_index_FildeshKVE_joint(joint))

BEGIN_EXTERN_C

void maybe_grow_FildeshKV_SINGLE_LIST(FildeshKV*);
void reclaim_element_FildeshKV_SINGLE_LIST(FildeshKV*, size_t);

END_EXTERN_C

#include "kve.h"
#include <assert.h>

static const FildeshKV_id_t FildeshKV_NULL_ID = ~(size_t)0;
static const FildeshKV_id_t FildeshKV_NULL_INDEX = ~(size_t)0 >> 3;

#define assert_trivial_joint(joint)  assert(joint == get_index_FildeshKVE_joint(joint))

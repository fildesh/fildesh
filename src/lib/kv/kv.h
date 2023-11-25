#include "kve.h"
#include <assert.h>

/* Deprecated. Use FildeshKV_id.*/
typedef FildeshKV_id FildeshKV_id_t;
static const FildeshKV_id_t FildeshKV_NULL_ID = ~(size_t)0;

#define assert_trivial_joint(joint)  assert(joint == get_index_FildeshKVE_joint(joint))

BEGIN_EXTERN_C

extern const FildeshKV_VTable DEFAULT_SINGLE_LIST_FildeshKV_VTable;
#define DEFAULT_FildeshKV_SINGLE_LIST { NULL, NULL, 0, 0, &DEFAULT_SINGLE_LIST_FildeshKV_VTable }
extern const FildeshKV_VTable DEFAULT_BSTREE_FildeshKV_VTable;
#define DEFAULT_FildeshKV_BSTREE { NULL, NULL, 0, 0, &DEFAULT_BSTREE_FildeshKV_VTable }
extern const FildeshKV_VTable DEFAULT_RBTREE_FildeshKV_VTable;
#define DEFAULT_FildeshKV_RBTREE { NULL, NULL, 0, 0, &DEFAULT_RBTREE_FildeshKV_VTable }
extern const FildeshKV_VTable DEFAULT_BRBTREE_FildeshKV_VTable;
#define DEFAULT_FildeshKV_BRBTREE { NULL, NULL, 0, 0, &DEFAULT_BRBTREE_FildeshKV_VTable }

void maybe_grow_FildeshKV_SINGLE_LIST(FildeshKV*);
void reclaim_element_FildeshKV_SINGLE_LIST(FildeshKV*, size_t);
size_t fixup_insert_FildeshKV_RBTREE(FildeshKV*, size_t, size_t);
void fixup_remove_FildeshKV_RBTREE(FildeshKV*, size_t, unsigned);

END_EXTERN_C

#ifndef BBK_POOLS_H
#define BBK_POOLS_H

#include "bbk_types.h"
#include "bbk_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bbk_pool_t    bbk_pool_t;

/** A function that is called when allocation failes.*/
typedef int (*bbk_abortfunc_t)(int retcode);

/**
 * Create a new pool.
 * @param newpool - The pool we have just created.
 * @param parent - The parent pool. If this is NULL, the new pool is a root
 *        pool. If it is non-NULL, the new pool will inherit all of its
 *        parent pool's attributes, except the bbk_pool_t will be a sub-pool
 */

#define bbk_pool_create(newpool, parent) bbk_pool_create_ex(newpool, parent, NULL, NULL)

BBK_DECLARE(bbk_status_t) bbk_pool_create_ex(bbk_pool_t**, bbk_pool_t*, bbk_abortfunc_t, bbk_allocator_t*);

BBK_DECLARE(void) bbk_pool_destroy(bbk_pool_t* pool);

#ifdef __cplusplus
}
#endif


#endif /* BBK_POOLS_H */

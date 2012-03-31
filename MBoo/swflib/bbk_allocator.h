#ifndef BBK_ALLOCATOR_H
#define BBK_ALLOCATOR_H

#include "bbk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BBK_ALLOCATOR_MAX_FREE_UNLIMITED    0


typedef struct bbk_allocator_t	bbk_allocator_t;

typedef struct bbk_memnode_t	bbk_memnode_t;

/** basic memory node structure
 * @note The next, ref and first_avail fields are available for use by the 
 *       caller of bbk_allocator_alloc(), the remaining fields are read-only.
 *       The next field has to be used with caution and sensibly set when the
 *       memnode is passed back to bbk_allocator_free().
 *
 */
struct bbk_memnode_t {
    bbk_memnode_t    *next;		   /**< next memnode */
    bbk_memnode_t    **ref;        /**< reference to self */
    bbk_uint32_t     index;        /**< size */
    bbk_uint32_t     free_index;   /**< how much free */
    char             *first_avail; /**< pointer to first free memory */
    char             *endp;        /**< pointer to end of free memory */
};

#define BBK_MEMNODE_T_SIZE BBK_ALIGN_DEFAULT(sizeof(bbk_memnode_t))



#ifdef __cplusplus
}
#endif


#endif /* BBK_ALLOCATOR_H */

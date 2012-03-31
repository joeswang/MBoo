#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "bbk_allocator.h"
#include "bbk_pools.h"
#include "bbk_atomic.h"

/*
 * Magic numbers
 */
#define	MIN_ALLOC	8192
#define	MAX_INDEX	20

#define BOUNDARY_INDEX 12
#define BOUNDARY_SIZE (1 << BOUNDARY_INDEX)    /* 2^12 = 4K boundary */

static bbk_byte_t       bbk_pools_initialized = 0;
static bbk_pool_t       *global_pool = NULL;
static bbk_allocator_t  *global_allocator = NULL;


/*
 * Cleanup
 */
typedef struct cleanup_t cleanup_t;
struct cleanup_t {
	struct cleanup_t *next;
	const void *data;
	bbk_status_t (*plain_cleanup_fn)(void *data);
	bbk_status_t (*child_cleanup_fn)(void *data);
};


/*
 * Allocator
 *
 * @note The max_free_index and current_free_index fields are not really 
 *       indices, but quantities of BOUNDARY_SIZE big memory blocks.
 */
struct bbk_allocator_t {
	/** largest used index into free[], always < MAX_INDEX */
    bbk_uint32_t    max_index;
    bbk_uint32_t    max_free_index;
    bbk_uint32_t    current_free_index;
    bbk_pool_t      *owner;
	/**
	 * Lists of free nodes. Slot 0 is used for oversized nodes,
	 * and the slots 1..MAX_INDEX-1 contain nodes of sizes
	 * (i+1) * BOUNDARY_SIZE. Example for BOUNDARY_INDEX == 12.
	 * slot 0 : nodes larger than 81920
	 * slot 1 : size 8192
	 * slot 2 : size 12288
	 * ...
	 * slot 19 : size 81920
	 */
    bbk_memnode_t   *free[MAX_INDEX];
};

/* The ref field in the bbk_pool_t struct holds a
 * pinter to the pointer referenceing this pool.
 * It is used for parent, child, sibling management.
 * Look at bbk_pool_create_ex() and bbk_pool_destroy()
 * to see how it is used.
 */
struct bbk_pool_t {
	bbk_pool_t	*parent;
	bbk_pool_t	*child;
	bbk_pool_t	*sibling;
	bbk_pool_t	**ref;
	cleanup_t	*cleanups;
	cleanup_t	*free_cleanups;
	bbk_allocator_t	*allocator;
	//struct process_chain	*subprocesses;
	bbk_abortfunc_t	abort_fn;
	//bbk_hash_t	*user_data;
	const char	*tag;
//#if !BBK_POOL_DEBUG
	bbk_memnode_t	*active;
	bbk_memnode_t	*self;
	char			*self_first_avail;
//#else /* BBK_POOL_DEBUG */
//	bbk_poo_t		*joined;
//#endif /* BBK_POOL_DEBUG */
	cleanup_t		*pre_cleanups;

};  /* end bbk_pool_t */

#define SIZEOF_POOL_T	BBK_ALIGN_DEFAULT(sizeof(bbk_pool_t))

#define SIZEOF_ALLOCATOR_T	BBK_ALIGN_DEFAULT(sizeof(bbk_allocator_t))


static bbk_memnode_t *allocator_alloc(bbk_allocator_t *allocator, bbk_size_t in_size)
{
	bbk_memnode_t *node, **ref;
	bbk_uint32_t max_index;
	bbk_size_t size, i, index;

	/* Round up the block size to the next boundary, but always
	 * allocate at lease a certain size (MIN_ALLOC).
	*/
	size = BBK_ALIGN(in_size + BBK_MEMNODE_T_SIZE, BOUNDARY_SIZE);
	if(size < in_size)
	{
		return NULL;
	}
	if(size < MIN_ALLOC) size = MIN_ALLOC;

	/* Find the index for this node size by 
	 *dividing its size by the boundary size
	 */
	index = (size >> BOUNDARY_INDEX) - 1;
	
	if(index > BBK_UINT32_MAX)
	{
		return NULL;
	}

	/* First see if there are any nodes in the area we know
	 * our node will fit into.
	 */
	if(index <= allocator->max_index)
	{
		/* Walk the free list to see if there are
		 * any nodes on it of the requested size
		 * 
		 * NOTE : 
		 */
		max_index = allocator->max_index;
		ref = &allocator->free[index];
		i = index;
		while (*ref == NULL && i < max_index)
		{
			ref++;
			i++;
		}

		if((node = *ref) != NULL)
		{

			if((*ref = node->next) == NULL && i >= max_index)
			{
				do 
				{
					ref--;
					max_index--;
				} while (*ref == NULL && max_index > 0);

				allocator->max_index = max_index;
			}
			allocator->current_free_index += node->index + 1;
			if(allocator->current_free_index > allocator->max_free_index)
				allocator->current_free_index = allocator->max_free_index;

			node->next = NULL;
			node->first_avail = (char*)node + BBK_MEMNODE_T_SIZE;

			return node;
		}
	}
	/* If we find nothing, seek the sink (at index 0), if 
	 * it is not empty.
	 */
	else if(allocator->free[0])
	{
		/* Walk the free list to see if there are 
		 * any nodes on it of the requested size
		 */
		 ref = &allocator->free[0];
		 while ((node = *ref) != NULL && index > node->index)
		 {
			 ref = &node->next;
		 }

	}

	/* If we haven't got a suitable node, malloc a new one
	 * and initialize it.
	 */
	if((node = malloc(size)) == NULL) return NULL;

	node->next = NULL;
	node->index = (BBK_UINT32_TRUNC_CAST)index;
	node->first_avail = (char*)node + BBK_MEMNODE_T_SIZE;
	node->endp = (char*)node + size;

	return node;
//#endif
//	return NULL;
}

static void allocator_free(bbk_allocator_t *allocator, bbk_memnode_t *node)
{
#if 0
	bbk_memnode_t *next, *freelist = NULL;
	bbk_uint32_t index, max_index;
	bbk_uint32_t max_free_index, current_free_index;

    max_index = allocator->max_index;
	max_free_index = allocator->max_free_index;
	current_free_index = allocator->current_free_index;

	/* Walk the list of submitted nodes and free them one by one,
	 * shoving them in the right 'size' buckets as we go.
	 */
	do
	{
		next = node->next;
		index = node->index;

		if (max_free_index != BBK_ALLOCATOR_MAX_FREE_UNLIMITED
		    && index + 1 > current_free_index)
		{
			node->next = freelist;
			freelist = node;
		}
		else if (index < MAX_INDEX)
		{
			/* Add the node to the appropiate 'size' bucket. Adjust
			 * the max_index when appropiate.
			 */
			if ((node->next = allocator->free[index]) == NULL
				&& index > max_index)
			{
				max_index = index;
			}
			allocator->free[index] = node;
			if (current_free_index >= index + 1)
			{
				current_free_index -= index + 1;
			}
			else 
			{
				current_free_index = 0;
			}
		}
		else 
		{
			/* This node is tool large to keep in a specific size bucket,
			 * just add it to the sink (at index 0).
			 */
			node->next = allocator->free[0];
			allocator->free[0] = node;
			if (current_free_index >= index + 1)
			{
				current_free_index -= index + 1;
			}
			else
			{
				current_free_index = 0;
			}
		}
	}
	while ((node = next) != NULL);

	alloctor->max_index = max_index;
	allocator->current_free_index = current_free_index;

    /* Release all the nodes in the freelist link */
	while (freelist != NULL)
	{
		node = freelist;
		freelist = node->next;
		free(node);
	}
#endif
}

BBK_DECLARE(void) bbk_pool_tag(bbk_pool_t *pool, const char *tag)
{
	pool->tag = tag;
}

BBK_DECLARE(void) bbk_allocator_owner_set(bbk_allocator_t *allocator, bbk_pool_t *pool)
{
	allocator->owner = pool;
}

BBK_DECLARE(bbk_pool_t *) bbk_allocator_owner_get(bbk_allocator_t *allocator)
{
	return allocator->owner;
}


/*
 * Allocator
 */
BBK_DECLARE(bbk_status_t) bbk_allocator_create(bbk_allocator_t **allocator)
{
	bbk_allocator_t *new_allocator;

	*allocator = NULL;

	if ((new_allocator = malloc(SIZEOF_ALLOCATOR_T)) == NULL)
		return BBK_ENOMEM;

	memset(new_allocator, 0, SIZEOF_ALLOCATOR_T);
	new_allocator->max_free_index = BBK_ALLOCATOR_MAX_FREE_UNLIMITED;

	*allocator = new_allocator;

	return BBK_SUCCESS;
}

/* Free the memory allocated by allocator */
BBK_DECLARE(void) bbk_allocator_destroy(bbk_allocator_t *allocator)
{
	bbk_uint32_t index;
	bbk_memnode_t *node, **ref;

	for(index =0; index < MAX_INDEX; index++)
	{
		ref = &allocator->free[index];
		while ((node = *ref) != NULL)
		{
			*ref = node->next;
			free(node);
		}
	}

	free(allocator);
}

/*
 * Initialization
 */

BBK_DECLARE(bbk_status_t) bbk_pool_initialize(void)
{
	bbk_status_t rv;

	if(bbk_pools_initialized++) return BBK_SUCCESS;

	if((rv = bbk_allocator_create(&global_allocator)) != BBK_SUCCESS)
	{
		bbk_pools_initialized = 0;
		return rv;
	}
	
	if((rv = bbk_pool_create_ex(&global_pool, NULL, NULL, 
								global_allocator)) != BBK_SUCCESS)
	{
		bbk_allocator_destroy(global_allocator);
		global_allocator = NULL;
		bbk_pools_initialized = 0;
		return rv;
	}

	bbk_pool_tag(global_pool, "bbk_global_pool");

	if((rv = bbk_atomic_init(global_pool)) != BBK_SUCCESS)
	{
		return rv;
	}

	bbk_allocator_owner_set(global_allocator, global_pool);

	return BBK_SUCCESS;
}

BBK_DECLARE(void) bbk_pool_terminate()
{
	if(!bbk_pools_initialized) return;

	if(--bbk_pools_initialized) return;

	bbk_pool_destroy(global_pool);
	global_pool = NULL;

	global_allocator = NULL;
}


BBK_DECLARE(bbk_status_t) bbk_pool_create_ex(bbk_pool_t **newpool,
											 bbk_pool_t *parent,
											 bbk_abortfunc_t abort_fn,
											 bbk_allocator_t *allocator)
{
	bbk_pool_t *pool;
	bbk_memnode_t *node;

	*newpool = NULL;
	if(!parent) parent = global_pool;

	/* parent will always be non-NULL here except the first time a
	 * pool is created, in which case allocator is guaranteed to be
	 * non-NULL */
	if(!abort_fn && parent)
		abort_fn = parent->abort_fn;

	if(allocator == NULL)
		allocator = parent->allocator;

	if((node = allocator_alloc(allocator, MIN_ALLOC - BBK_MEMNODE_T_SIZE)) == NULL)
	{
		if(abort_fn)
			abort_fn(BBK_ENOMEM);

		return BBK_ENOMEM;
	}

	node->next = node;
	node->ref = &node->next;
	pool = (bbk_pool_t*)node->first_avail;
	node->first_avail = pool->self_first_avail = (char*)pool + SIZEOF_POOL_T;
	pool->allocator = allocator;
	pool->active = pool->self = node;
	pool->abort_fn = abort_fn;
	pool->child = NULL;
	pool->cleanups = NULL;
	pool->free_cleanups = NULL;
	pool->pre_cleanups = NULL;
	//pool->subprocesses = NULL;
	//pool->user_data = NULL;
	pool->tag = NULL;

	if((pool->parent = parent) != NULL)
	{
		if((pool->sibling = parent->child) != NULL)
			pool->sibling->ref = &pool->sibling;

		parent->child = pool;
		pool->ref = &parent->child;
	}
	else
	{
		pool->sibling = NULL;
		pool->ref = NULL;
	}

	*newpool = pool;

	return BBK_SUCCESS;
}

BBK_DECLARE(void) bbk_pool_destroy(bbk_pool_t *pool)
{
	bbk_memnode_t *active;
	bbk_allocator_t *allocator;

	/* Run pre destroy cleanups */
	//run_cleanups(&pool->pre_cleanups);
	//pool->pre_cleanups = NULL;

	/* Destroy the subpools. The subpools will detach themselve from
	 * this pool thus this loop is safe and easy.
	 */
	while(pool->child)
		bbk_pool_destroy(pool->child);

	/* Run cleanups */
	//run_cleanups(&pool->cleanups);

	/* Free subprocesses */
	//free_proc_chain(pool->subprocesses);

    /* Remove the pool from the parent child list */
	if(pool->parent)
	{
		if((*pool->ref = pool->sibling) != NULL)
			pool->sibling->ref = pool->ref;
	}
	
	/* Find the block attached to the pool structure. Save a copy of the 
	 * allocator pointer, because the pool struct soon will be now more.
	 */
	allocator = pool->allocator;
	active = pool->self;
	*active->ref = NULL;

	/* Free all the nodes in the pool (including the node holding the
	 * pool struct), by giving them back to the allocator.
	 */
	allocator_free(allocator, active);

	/* If this pool happens to be the owner of the allocator, free
	 * everything in the allocator (that includes the pool struct
	 * and the allocator). Don't worry about destroying the optional mutex
	 * in the allocator, it will have been destroyed by the cleanup function.
	 */
	if(bbk_allocator_owner_get(allocator) == pool)
	{
		bbk_allocator_destroy(allocator);
	}

}

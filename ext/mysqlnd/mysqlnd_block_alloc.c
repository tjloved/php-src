/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_block_alloc.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_priv.h"

#define MYSQLND_POOL_CHUNK_HEADER_SIZE XtOffsetOf(MYSQLND_MEMORY_POOL_CHUNK, ptr)

static inline zend_bool is_from_pool(MYSQLND_MEMORY_POOL * pool, zend_uchar * ptr)
{
	return ptr >= pool->arena && ptr < pool->arena + pool->arena_size;
}

/* {{{ mysqlnd_mempool_free_chunk */
static void
mysqlnd_mempool_free_chunk(MYSQLND_MEMORY_POOL * pool, MYSQLND_MEMORY_POOL_CHUNK * chunk)
{
	DBG_ENTER("mysqlnd_mempool_free_chunk");
	if (is_from_pool(pool, chunk->ptr)) {
		/* Try to back-off and guess if this is the last block allocated */
		if (chunk->ptr == (pool->arena + (pool->arena_size - pool->free_size - chunk->size))) {
			/*
				This was the last allocation. Lucky us, we can free
				a bit of memory from the pool. Next time we will return from the same ptr.
			*/
			pool->free_size += chunk->size + MYSQLND_POOL_CHUNK_HEADER_SIZE;
		}
	} else {
		mnd_efree(chunk);
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_mempool_resize_chunk */
static MYSQLND_MEMORY_POOL_CHUNK *
mysqlnd_mempool_resize_chunk(MYSQLND_MEMORY_POOL * pool, MYSQLND_MEMORY_POOL_CHUNK * chunk, unsigned int size)
{
	DBG_ENTER("mysqlnd_mempool_resize_chunk");
	if (is_from_pool(pool, chunk->ptr)) {
		/* Try to back-off and guess if this is the last block allocated */
		if (chunk->ptr == (pool->arena + (pool->arena_size - pool->free_size - chunk->size))) {
			/*
				This was the last allocation. Lucky us, we can free
				a bit of memory from the pool. Next time we will return from the same ptr.
			*/
			if ((chunk->size + pool->free_size) < size) {
				MYSQLND_MEMORY_POOL_CHUNK *new_chunk =
					mnd_emalloc(size + MYSQLND_POOL_CHUNK_HEADER_SIZE);
				if (!new_chunk) {
					DBG_RETURN(NULL);
				}
				new_chunk->app = chunk->app;
				new_chunk->size = size;
				memcpy(new_chunk->ptr, chunk->ptr, chunk->size);
				pool->free_size += chunk->size * MYSQLND_POOL_CHUNK_HEADER_SIZE;
				DBG_RETURN(new_chunk);
			} else {
				/* If the chunk is > than asked size then free_memory increases, otherwise decreases*/
				pool->free_size += (chunk->size - size);
				DBG_RETURN(chunk);
			}
		} else {
			/* Not last chunk, if the user asks for less, give it to him */
			if (chunk->size >= size) {
				DBG_RETURN(chunk);
			} else {
				MYSQLND_MEMORY_POOL_CHUNK *new_chunk =
					mnd_emalloc(size + MYSQLND_POOL_CHUNK_HEADER_SIZE);
				if (!new_chunk) {
					DBG_RETURN(NULL);
				}
				new_chunk->app = chunk->app;
				new_chunk->size = size;
				memcpy(new_chunk->ptr, chunk->ptr, chunk->size);
				DBG_RETURN(new_chunk);
			}
		}
	} else {
		MYSQLND_MEMORY_POOL_CHUNK *new_chunk =
			mnd_erealloc(chunk, size + MYSQLND_POOL_CHUNK_HEADER_SIZE);
		if (!new_chunk) {
			DBG_RETURN(NULL);
		}
		new_chunk->size = size;
		DBG_RETURN(new_chunk);
	}
}
/* }}} */


/* {{{ mysqlnd_mempool_get_chunk */
static
MYSQLND_MEMORY_POOL_CHUNK * mysqlnd_mempool_get_chunk(MYSQLND_MEMORY_POOL * pool, unsigned int size)
{
	MYSQLND_MEMORY_POOL_CHUNK *chunk = NULL;
	unsigned int real_size;
	DBG_ENTER("mysqlnd_mempool_get_chunk");

	real_size = size + MYSQLND_POOL_CHUNK_HEADER_SIZE;
	if (real_size > pool->free_size) {
		chunk = mnd_emalloc(real_size);
		if (!chunk) {
			DBG_RETURN(NULL);
		}
	} else {
		chunk = (MYSQLND_MEMORY_POOL_CHUNK *)
			(pool->arena + (pool->arena_size - pool->free_size));
		/* Last step, update free_size */
		pool->free_size -= real_size;
	}

	chunk->app = 0;
	chunk->size = size;
	DBG_RETURN(chunk);
}
/* }}} */


/* {{{ mysqlnd_mempool_create */
PHPAPI MYSQLND_MEMORY_POOL *
mysqlnd_mempool_create(size_t arena_size)
{
	/* We calloc, because we free(). We don't mnd_calloc()  for a reason. */
	MYSQLND_MEMORY_POOL * ret = mnd_ecalloc(1, sizeof(MYSQLND_MEMORY_POOL));
	DBG_ENTER("mysqlnd_mempool_create");
	if (ret) {
		ret->get_chunk = mysqlnd_mempool_get_chunk;
		ret->free_chunk = mysqlnd_mempool_free_chunk;
		ret->resize_chunk = mysqlnd_mempool_resize_chunk;
		ret->free_size = ret->arena_size = arena_size ? arena_size : 0;
		/* OOM ? */
		ret->arena = mnd_emalloc(ret->arena_size);
		if (!ret->arena) {
			mysqlnd_mempool_destroy(ret);
			ret = NULL;
		}
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_mempool_destroy */
PHPAPI void
mysqlnd_mempool_destroy(MYSQLND_MEMORY_POOL * pool)
{
	DBG_ENTER("mysqlnd_mempool_destroy");
	/* mnd_free will reference LOCK_access and might crash, depending on the caller...*/
	mnd_efree(pool->arena);
	mnd_efree(pool);
	DBG_VOID_RETURN;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

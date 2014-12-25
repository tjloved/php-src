/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"

#if ZEND_DEBUG
/*
#define HASH_MASK_CONSISTENCY	0x60
*/
#define HT_OK					0x00
#define HT_IS_DESTROYING		0x20
#define HT_DESTROYED			0x40
#define HT_CLEANING				0x60

static void _zend_is_inconsistent(const HashTable *ht, const char *file, int line)
{
	if ((ht->u.flags & HASH_MASK_CONSISTENCY) == HT_OK) {
		return;
	}
	switch ((ht->u.flags & HASH_MASK_CONSISTENCY)) {
		case HT_IS_DESTROYING:
			zend_output_debug_string(1, "%s(%d) : ht=%p is being destroyed", file, line, ht);
			break;
		case HT_DESTROYED:
			zend_output_debug_string(1, "%s(%d) : ht=%p is already destroyed", file, line, ht);
			break;
		case HT_CLEANING:
			zend_output_debug_string(1, "%s(%d) : ht=%p is being cleaned", file, line, ht);
			break;
		default:
			zend_output_debug_string(1, "%s(%d) : ht=%p is inconsistent", file, line, ht);
			break;
	}
	zend_bailout();
}
#define IS_CONSISTENT(a) _zend_is_inconsistent(a, __FILE__, __LINE__);
#define SET_INCONSISTENT(n) do { \
		(ht)->u.flags |= n; \
	} while (0)
#else
#define IS_CONSISTENT(a)
#define SET_INCONSISTENT(n)
#endif

#define HASH_PROTECT_RECURSION(ht)														\
	if ((ht)->u.flags & HASH_FLAG_APPLY_PROTECTION) {									\
		if ((ht)->u.flags >= (3 << 8)) {												\
			zend_error_noreturn(E_ERROR, "Nesting level too deep - recursive dependency?");\
		}																				\
		ZEND_HASH_INC_APPLY_COUNT(ht);													\
	}

#define HASH_UNPROTECT_RECURSION(ht)													\
	if ((ht)->u.flags & HASH_FLAG_APPLY_PROTECTION) {									\
		ZEND_HASH_DEC_APPLY_COUNT(ht);													\
	}

#define ZEND_HASH_IF_FULL_DO_RESIZE(ht)				\
	if ((ht)->nNumUsed >= (ht)->nTableSize) {		\
		zend_hash_do_resize(ht);					\
	}

static void zend_hash_do_resize(HashTable *ht);

#define CHECK_INIT(ht, packed) do { \
	if (UNEXPECTED(!((ht)->u.flags & HASH_FLAG_INITIALIZED))) { \
		if (packed) { \
			(ht)->u.flags |= HASH_FLAG_INITIALIZED | HASH_FLAG_PACKED; \
			(ht)->data.arZval = safe_pemalloc((ht)->nTableSize, sizeof(zval), 0, HT_IS_PERSISTENT(ht)); \
		} else { \
			(ht)->u.flags |= HASH_FLAG_INITIALIZED; \
			(ht)->nTableMask = (ht)->nTableSize - 1; \
			(ht)->data.arBucket = (Bucket *) safe_pemalloc((ht)->nTableSize, sizeof(Bucket) + sizeof(uint32_t), 0, HT_IS_PERSISTENT(ht)); \
			(ht)->arHash = (uint32_t*)((ht)->data.arBucket + (ht)->nTableSize);	\
			memset((ht)->arHash, INVALID_IDX, (ht)->nTableSize * sizeof(uint32_t));	\
		} \
	} \
} while (0)

#define HT_IS_PACKED(ht) (((ht)->u.flags & HASH_FLAG_PACKED) != 0)
#define HT_IS_PERSISTENT(ht) (((ht)->u.flags & HASH_FLAG_PERSISTENT) != 0)

#define HT_KEY_MATCHES(p, _h, _key) \
	((p)->key == (_key) \
	 || ((p)->h == (_h) && (p)->key && (p)->key->len == (_key)->len \
	     && memcmp((p)->key->val, (_key)->val, (_key)->len) == 0))
#define HT_STR_MATCHES(p, _h, _str, _len) \
	((p)->h == (_h) && (p)->key && (p)->key->len == (_len) \
	 && memcmp((p)->key->val, (_str), (_len)) == 0)

#define HT_FREE_DATA(ht) \
	pefree((ht)->data.arBucket, HT_IS_PERSISTENT(ht))

/* only for non-packed */
#define HT_BUCKET(ht, idx) (&(ht)->data.arBucket[(idx)])
/* only for packed */
#define HT_ZVAL(ht, idx) (&(ht)->data.arZval[(idx)])
/* for any */
//#define HT_VAL(ht, idx)
	//(HT_IS_PACKED(ht) ? HT_ZVAL(ht, idx) : &HT_BUCKET(ht, idx)->val)
 
static const uint32_t uninitialized_bucket = {INVALID_IDX};

/* Returns next valid index that is >= idx */
static zend_always_inline uint32_t zend_unpacked_next_idx(const HashTable *ht, uint32_t idx) {
	for (; idx < ht->nNumUsed; idx++) {
		if (!Z_ISUNDEF(HT_BUCKET(ht, idx)->val)) {
			return idx;
		}
	}
	return INVALID_IDX;
}
static zend_always_inline uint32_t zend_packed_next_idx(const HashTable *ht, uint32_t idx) {
	for (; idx < ht->nNumUsed; idx++) {
		if (!Z_ISUNDEF_P(HT_ZVAL(ht, idx))) {
			return idx;
		}
	}
	return INVALID_IDX;
}
static zend_always_inline uint32_t zend_hash_next_idx(const HashTable *ht, uint32_t idx) {
	if (HT_IS_PACKED(ht)) {
		return zend_packed_next_idx(ht, idx);
	} else {
		return zend_unpacked_next_idx(ht, idx);
	}
}

/* Returns previous valid index that is < idx */
static zend_always_inline uint32_t zend_hash_prev_idx(const HashTable *ht, uint32_t idx) {
	if (HT_IS_PACKED(ht)) {
		while (idx-- > 0) {
			if (!Z_ISUNDEF_P(HT_ZVAL(ht, idx))) {
				return idx;
			}
		}
	} else {
		while (idx-- > 0) {
			if (!Z_ISUNDEF(HT_BUCKET(ht, idx)->val)) {
				return idx;
			}
		}
	}
	return INVALID_IDX;
}

ZEND_API void _zend_hash_init(HashTable *ht, uint32_t nSize, dtor_func_t pDestructor, zend_bool persistent ZEND_FILE_LINE_DC)
{
	/* Use big enough power of 2 */
#if defined(PHP_WIN32) && !defined(__clang__)
	if (nSize <= 8) {
		ht->nTableSize = 8;
	} else if (nSize >= 0x80000000) {
		/* prevent overflow */
		ht->nTableSize = 0x80000000;
	} else {
		ht->nTableSize = 1U << __lzcnt(nSize);
		if (ht->nTableSize < nSize) {
			ht->nTableSize <<= 1;
		}
	}
#else
	/* size should be between 8 and 0x80000000 */
	nSize = (nSize <= 8 ? 8 : (nSize >= 0x80000000 ? 0x80000000 : nSize));
# if defined(__GNUC__)
	ht->nTableSize =  0x2 << (__builtin_clz(nSize - 1) ^ 0x1f);
# else
	nSize -= 1;
	nSize |= (nSize >> 1);
	nSize |= (nSize >> 2);
	nSize |= (nSize >> 4);
	nSize |= (nSize >> 8);
	nSize |= (nSize >> 16);
	ht->nTableSize = nSize + 1;
# endif
#endif

	ht->nTableMask = 0;
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->data.arBucket = NULL;
	ht->arHash = (uint32_t*) &uninitialized_bucket;
	ht->pDestructor = pDestructor;
	ht->nInternalPointer = INVALID_IDX;
	ht->u.flags = (persistent ? HASH_FLAG_PERSISTENT : 0) | HASH_FLAG_APPLY_PROTECTION;
}

static void zend_hash_packed_grow(HashTable *ht)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	ht->data.arZval = safe_perealloc(
		ht->data.arZval, ht->nTableSize << 1, sizeof(zval), 0, HT_IS_PERSISTENT(ht));
	ht->nTableSize = ht->nTableSize << 1;
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API void zend_hash_real_init(HashTable *ht, zend_bool packed)
{
	IS_CONSISTENT(ht);

	CHECK_INIT(ht, packed);
}

ZEND_API void zend_hash_packed_to_hash(HashTable *ht)
{
	uint32_t idx;

	HANDLE_BLOCK_INTERRUPTIONS();
	ht->u.flags &= ~HASH_FLAG_PACKED;
	ht->data.arBucket = safe_perealloc(
		ht->data.arBucket, ht->nTableSize, sizeof(Bucket) + sizeof(uint32_t), 0,
		ht->u.flags & HASH_FLAG_PERSISTENT);

	/* Move from arZval to arBucket layout. Iteration must happen in reverse */
	idx = ht->nNumUsed;
	while (idx-- > 0) {
		Bucket *p = HT_BUCKET(ht, idx);
		ZVAL_COPY_VALUE(&p->val, &ht->data.arZval[idx]);
		p->key = NULL;
		p->h = idx;
	}

	ht->arHash = (uint32_t *) (ht->data.arBucket + ht->nTableSize);
	zend_hash_rehash(ht);
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API void zend_hash_to_packed(HashTable *ht)
{
	Bucket *arBucket = ht->data.arBucket;
	uint32_t idx;

	HANDLE_BLOCK_INTERRUPTIONS();
	ht->u.flags |= HASH_FLAG_PACKED;
	ht->nTableMask = 0;
	ht->data.arZval = safe_pemalloc(
		ht->nTableSize, sizeof(Bucket), 0, HT_IS_PERSISTENT(ht));
	for (idx = 0; idx < ht->nNumUsed; ++idx) {
		Bucket *p = &arBucket[idx];
		if (Z_ISUNDEF(p->val)) continue;
		if (p->key) {
			zend_string_release(p->key);
		}
		ZVAL_COPY_VALUE(HT_ZVAL(ht, idx), &p->val);
	}
	ht->arHash = (uint32_t *) &uninitialized_bucket;
	pefree(arBucket, HT_IS_PERSISTENT(ht));
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API void _zend_hash_init_ex(HashTable *ht, uint32_t nSize, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection ZEND_FILE_LINE_DC)
{
	_zend_hash_init(ht, nSize, pDestructor, persistent ZEND_FILE_LINE_CC);
	if (!bApplyProtection) {
		ht->u.flags &= ~HASH_FLAG_APPLY_PROTECTION;
	}
}


ZEND_API void zend_hash_set_apply_protection(HashTable *ht, zend_bool bApplyProtection)
{
	if (bApplyProtection) {
		ht->u.flags |= HASH_FLAG_APPLY_PROTECTION;
	} else {
		ht->u.flags &= ~HASH_FLAG_APPLY_PROTECTION;
	}
}

/* Must be called on not packed hash */
static zend_always_inline Bucket *zend_hash_find_bucket(const HashTable *ht, zend_string *key)
{
	zend_ulong h = zend_string_hash_val(key);
	uint32_t nIndex = h & ht->nTableMask;
	uint32_t idx = ht->arHash[nIndex];

	while (idx != INVALID_IDX) {
		Bucket *p = HT_BUCKET(ht, idx);
		if (HT_KEY_MATCHES(p, h, key)) {
			return p;
		}
		idx = Z_NEXT(p->val);
	}
	return NULL;
}

/* Must be called on not packed hash */
static zend_always_inline Bucket *zend_hash_str_find_bucket(const HashTable *ht, const char *str, size_t len, zend_ulong h)
{
	uint32_t nIndex = h & ht->nTableMask;
	uint32_t idx = ht->arHash[nIndex];

	while (idx != INVALID_IDX) {
		Bucket *p = HT_BUCKET(ht, idx);
		if (HT_STR_MATCHES(p, h, str, len)) {
			return p;
		}
		idx = Z_NEXT(p->val);
	}
	return NULL;
}

/* Must be called on not packed hash */
static zend_always_inline Bucket *zend_hash_index_find_bucket(const HashTable *ht, zend_ulong h)
{
	uint32_t nIndex = h & ht->nTableMask;
	uint32_t idx = ht->arHash[nIndex];

	while (idx != INVALID_IDX) {
		Bucket *p = HT_BUCKET(ht, idx);
		if (p->h == h && !p->key) {
			return p;
		}
		idx = Z_NEXT(p->val);
	}
	return NULL;
}

static zend_always_inline zval *_zend_hash_add_or_update_i(HashTable *ht, zend_string *key, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);

	if (UNEXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		CHECK_INIT(ht, 0);
		goto add_to_hash; 
	} else if (HT_IS_PACKED(ht)) {
		zend_hash_packed_to_hash(ht);
	} else if ((flag & HASH_ADD_NEW) == 0) {
		p = zend_hash_find_bucket(ht, key);

		if (p) {
			zval *data;

			if (flag & HASH_ADD) {
				return NULL;
			}
			ZEND_ASSERT(&p->val != pData);
			data = &p->val;
			if ((flag & HASH_UPDATE_INDIRECT) && Z_TYPE_P(data) == IS_INDIRECT) {
				data = Z_INDIRECT_P(data);
			}
			HANDLE_BLOCK_INTERRUPTIONS();
			if (ht->pDestructor) {
				ht->pDestructor(data);
			}
			ZVAL_COPY_VALUE(data, pData);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			return data;
		}
	}
	
	ZEND_HASH_IF_FULL_DO_RESIZE(ht);		/* If the Hash table is full, resize it */

add_to_hash:
	HANDLE_BLOCK_INTERRUPTIONS();
	idx = ht->nNumUsed++;
	ht->nNumOfElements++;
	if (ht->nInternalPointer == INVALID_IDX) {
		ht->nInternalPointer = idx;
	}
	p = HT_BUCKET(ht, idx);
	p->h = h = zend_string_hash_val(key);
	p->key = key;
	zend_string_addref(key);
	ZVAL_COPY_VALUE(&p->val, pData);
	nIndex = h & ht->nTableMask;
	Z_NEXT(p->val) = ht->arHash[nIndex];
	ht->arHash[nIndex] = idx;
	HANDLE_UNBLOCK_INTERRUPTIONS();

	return &p->val;
}

ZEND_API zval *_zend_hash_add_or_update(HashTable *ht, zend_string *key, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, flag ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_add(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_update(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_update_ind(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE | HASH_UPDATE_INDIRECT ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_add_new(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD_NEW ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_str_add_or_update(HashTable *ht, const char *str, size_t len, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, flag ZEND_FILE_LINE_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval *_zend_hash_str_update(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE ZEND_FILE_LINE_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval *_zend_hash_str_update_ind(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE | HASH_UPDATE_INDIRECT ZEND_FILE_LINE_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval *_zend_hash_str_add(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD ZEND_FILE_LINE_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval *_zend_hash_str_add_new(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD_NEW ZEND_FILE_LINE_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval *zend_hash_index_add_empty_element(HashTable *ht, zend_ulong h)
{
	
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_index_add(ht, h, &dummy);
}

ZEND_API zval *zend_hash_add_empty_element(HashTable *ht, zend_string *key)
{
	
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_add(ht, key, &dummy);
}

ZEND_API zval *zend_hash_str_add_empty_element(HashTable *ht, const char *str, size_t len)
{
	
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_str_add(ht, str, len, &dummy);
}

static zend_always_inline zval *_zend_hash_index_add_or_update_i(HashTable *ht, zend_ulong h, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	uint32_t nIndex;
	uint32_t idx;
	zval *zv;
	Bucket *p;

	IS_CONSISTENT(ht);

	if (UNEXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		CHECK_INIT(ht, h < ht->nTableSize);
		if (h < ht->nTableSize) {
			zv = HT_ZVAL(ht, h);
			goto add_to_packed;
		}
		goto add_to_hash;
	} else if (HT_IS_PACKED(ht)) {
		if (h < ht->nNumUsed) {
			zv = HT_ZVAL(ht, h);
			if (!Z_ISUNDEF_P(zv)) {
				if (flag & HASH_ADD) {
					return NULL;
				}
				if (ht->pDestructor) {
					ht->pDestructor(zv);
				}
				ZVAL_COPY_VALUE(zv, pData);
				if ((zend_long) h >= (zend_long) ht->nNextFreeElement) {
					ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
				}
				return zv;
			} else { /* we have to keep the order :( */
				goto convert_to_hash;
			}
		} else if (EXPECTED(h < ht->nTableSize)) {
			zv = HT_ZVAL(ht, h);
		} else if ((h >> 1) < ht->nTableSize &&
		           (ht->nTableSize >> 1) < ht->nNumOfElements) {
			zend_hash_packed_grow(ht);
			zv = HT_ZVAL(ht, h);
		} else {
			goto convert_to_hash;
		}

add_to_packed:
		HANDLE_BLOCK_INTERRUPTIONS();
		/* incremental initialization of empty Buckets */
		if ((flag & (HASH_ADD_NEW|HASH_ADD_NEXT)) != (HASH_ADD_NEW|HASH_ADD_NEXT)
			&& h > ht->nNumUsed
		) {
			zval *zv_cur = HT_ZVAL(ht, ht->nNumUsed);
			while (zv_cur != zv) {
				ZVAL_UNDEF(zv_cur);
				zv_cur++;
			}
		}
		ht->nNumUsed = h + 1;
		ht->nNumOfElements++;
		if (ht->nInternalPointer == INVALID_IDX) {
			ht->nInternalPointer = h;
		}
		if ((zend_long)h >= (zend_long)ht->nNextFreeElement) {
			ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
		}
		ZVAL_COPY_VALUE(zv, pData);

		HANDLE_UNBLOCK_INTERRUPTIONS();

		return zv;

convert_to_hash:
		zend_hash_packed_to_hash(ht);
	} else if ((flag & HASH_ADD_NEW) == 0) {
		p = zend_hash_index_find_bucket(ht, h);
		if (p) {
			if (flag & HASH_ADD) {
				return NULL;
			}
			ZEND_ASSERT(&p->val != pData);
			HANDLE_BLOCK_INTERRUPTIONS();
			if (ht->pDestructor) {
				ht->pDestructor(&p->val);
			}
			ZVAL_COPY_VALUE(&p->val, pData);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			if ((zend_long)h >= (zend_long)ht->nNextFreeElement) {
				ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
			}
			return &p->val;
		}
	}

	ZEND_HASH_IF_FULL_DO_RESIZE(ht);		/* If the Hash table is full, resize it */

add_to_hash:
	HANDLE_BLOCK_INTERRUPTIONS();
	idx = ht->nNumUsed++;
	ht->nNumOfElements++;
	if (ht->nInternalPointer == INVALID_IDX) {
		ht->nInternalPointer = idx;
	}
	if ((zend_long)h >= (zend_long)ht->nNextFreeElement) {
		ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
	}
	p = HT_BUCKET(ht, idx);
	p->h = h;
	p->key = NULL;
	nIndex = h & ht->nTableMask;
	ZVAL_COPY_VALUE(&p->val, pData);
	Z_NEXT(p->val) = ht->arHash[nIndex];
	ht->arHash[nIndex] = idx;
	HANDLE_UNBLOCK_INTERRUPTIONS();

	return &p->val;
}

ZEND_API zval *_zend_hash_index_add_or_update(HashTable *ht, zend_ulong h, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, flag ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_index_add(HashTable *ht, zend_ulong h, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_ADD ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_index_add_new(HashTable *ht, zend_ulong h, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_ADD | HASH_ADD_NEW ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_index_update(HashTable *ht, zend_ulong h, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_UPDATE ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_next_index_insert(HashTable *ht, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, ht->nNextFreeElement, pData, HASH_ADD | HASH_ADD_NEXT ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_next_index_insert_new(HashTable *ht, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, ht->nNextFreeElement, pData, HASH_ADD | HASH_ADD_NEW | HASH_ADD_NEXT ZEND_FILE_LINE_RELAY_CC);
}

/* Must be called on non-packed hash */
static void zend_hash_do_resize(HashTable *ht)
{

	IS_CONSISTENT(ht);

	if (ht->nNumUsed > ht->nNumOfElements) {
		HANDLE_BLOCK_INTERRUPTIONS();
		zend_hash_rehash(ht);
		HANDLE_UNBLOCK_INTERRUPTIONS();
	} else if ((ht->nTableSize << 1) > 0) {	/* Let's double the table size */
		HANDLE_BLOCK_INTERRUPTIONS();
		ht->data.arBucket = safe_perealloc(
			ht->data.arBucket, ht->nTableSize << 1, sizeof(Bucket) + sizeof(uint32_t), 0,
			HT_IS_PERSISTENT(ht));
		ht->arHash = (uint32_t *) (ht->data.arBucket + (ht->nTableSize << 1));
		ht->nTableSize = ht->nTableSize << 1;
		ht->nTableMask = ht->nTableSize - 1;
		zend_hash_rehash(ht);
		HANDLE_UNBLOCK_INTERRUPTIONS();
	}
}

/* Must be called on non-packed hash */
ZEND_API int zend_hash_rehash(HashTable *ht)
{
	Bucket *p;
	uint32_t nIndex, i, j;

	IS_CONSISTENT(ht);

	if (UNEXPECTED(ht->nNumOfElements == 0)) {
		if (ht->u.flags & HASH_FLAG_INITIALIZED) {
			memset(ht->arHash, INVALID_IDX, ht->nTableSize * sizeof(uint32_t));
		}
		return SUCCESS;
	}

	memset(ht->arHash, INVALID_IDX, ht->nTableSize * sizeof(uint32_t));
	for (i = 0, j = 0; i < ht->nNumUsed; i++) {
		p = HT_BUCKET(ht, i);
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		if (i != j) {
			*HT_BUCKET(ht, j) = *p;
			if (ht->nInternalPointer == i) {
				ht->nInternalPointer = j;
			}
		}
		nIndex = HT_BUCKET(ht, j)->h & ht->nTableMask;
		Z_NEXT(HT_BUCKET(ht, j)->val) = ht->arHash[nIndex];
		ht->arHash[nIndex] = j;
		j++;
	}
	ht->nNumUsed = j;
	return SUCCESS;
}

/* Only for non-packed hashes */
static zend_always_inline void _zend_hash_del_el_ex(HashTable *ht, uint32_t idx, Bucket *p, Bucket *prev)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	if (prev) {
		Z_NEXT(prev->val) = Z_NEXT(p->val);
	} else {
		ht->arHash[p->h & ht->nTableMask] = Z_NEXT(p->val);
	}
	if (ht->nNumUsed - 1 == idx) {
		do {
			ht->nNumUsed--;
		} while (ht->nNumUsed > 0 && Z_ISUNDEF(HT_BUCKET(ht, ht->nNumUsed - 1)->val));
	}
	ht->nNumOfElements--;
	if (ht->nInternalPointer == idx) {
		ht->nInternalPointer = zend_unpacked_next_idx(ht, idx + 1);
	}
	if (p->key) {
		zend_string_release(p->key);
	}
	if (ht->pDestructor) {
		zval tmp;
		ZVAL_COPY_VALUE(&tmp, &p->val);
		ZVAL_UNDEF(&p->val);
		ht->pDestructor(&tmp);
	} else {
		ZVAL_UNDEF(&p->val);
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

/* Only for non-packed hashes */
static zend_always_inline void _zend_hash_del_el(HashTable *ht, uint32_t idx, Bucket *p)
{
	Bucket *prev = NULL;

	uint32_t nIndex = p->h & ht->nTableMask;
	uint32_t i = ht->arHash[nIndex];

	if (i != idx) {
		prev = HT_BUCKET(ht, i);
		while (Z_NEXT(prev->val) != idx) {
			i = Z_NEXT(prev->val);
			prev = HT_BUCKET(ht, i);
		}
	}

	_zend_hash_del_el_ex(ht, idx, p, prev);
}

/* Only for packed hashes */
static zend_always_inline void _zend_hash_del_packed_el(HashTable *ht, uint32_t idx, zval *val)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	if (ht->nNumUsed - 1 == idx) {
		do {
			ht->nNumUsed--;
		} while (ht->nNumUsed > 0 && Z_ISUNDEF_P(HT_ZVAL(ht, ht->nNumUsed - 1)));
	}
	ht->nNumOfElements--;
	if (ht->nInternalPointer == idx) {
		ht->nInternalPointer = zend_packed_next_idx(ht, idx + 1);
	}
	if (ht->pDestructor) {
		zval tmp;
		ZVAL_COPY_VALUE(&tmp, val);
		ZVAL_UNDEF(val);
		ht->pDestructor(&tmp);
	} else {
		ZVAL_UNDEF(val);
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API int zend_hash_del(HashTable *ht, zend_string *key)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);

	h = zend_string_hash_val(key);
	nIndex = h & ht->nTableMask;

	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = HT_BUCKET(ht, idx);
		if (HT_KEY_MATCHES(p, h, key)) {
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int zend_hash_del_ind(HashTable *ht, zend_string *key)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);

	h = zend_string_hash_val(key);
	nIndex = h & ht->nTableMask;

	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = HT_BUCKET(ht, idx);
		if (HT_KEY_MATCHES(p, h, key)) {
			if (Z_TYPE(p->val) == IS_INDIRECT) {
				zval *data = Z_INDIRECT(p->val);

				if (Z_TYPE_P(data) == IS_UNDEF) {
					return FAILURE;
				} else {
					if (ht->pDestructor) {
						ht->pDestructor(data);
					}
					ZVAL_UNDEF(data);
				}
			} else {
				_zend_hash_del_el_ex(ht, idx, p, prev);
			}
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int zend_hash_str_del(HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);

	h = zend_inline_hash_func(str, len);
	nIndex = h & ht->nTableMask;

	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = HT_BUCKET(ht, idx);
		if (HT_STR_MATCHES(p, h, str, len)) {
			if (Z_TYPE(p->val) == IS_INDIRECT) {
				zval *data = Z_INDIRECT(p->val);

				if (Z_TYPE_P(data) == IS_UNDEF) {
					return FAILURE;
				} else {
					if (ht->pDestructor) {
						ht->pDestructor(data);
					}
					ZVAL_UNDEF(data);
				}
			} else {
				_zend_hash_del_el_ex(ht, idx, p, prev);
			}
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int zend_hash_str_del_ind(HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);

	if (HT_IS_PACKED(ht)) {
		return FAILURE;
	}

	h = zend_inline_hash_func(str, len);
	nIndex = h & ht->nTableMask;

	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = HT_BUCKET(ht, idx);
		if (HT_STR_MATCHES(p, h, str, len)) {
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int zend_hash_index_del(HashTable *ht, zend_ulong h)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);

	if (HT_IS_PACKED(ht)) {
		if (h < ht->nNumUsed) {
			zval *val = HT_ZVAL(ht, h);
			if (!Z_ISUNDEF_P(val)) {
				_zend_hash_del_packed_el(ht, h, val);
				return SUCCESS;
			}
		}
		return FAILURE;
	}

	nIndex = h & ht->nTableMask;
	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = HT_BUCKET(ht, idx);
		if (p->h == h && p->key == NULL) {
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API void zend_hash_destroy(HashTable *ht)
{
	IS_CONSISTENT(ht);

	if (ht->nNumOfElements) {
		if (ht->pDestructor) {
			SET_INCONSISTENT(HT_IS_DESTROYING);

			if (HT_IS_PACKED(ht)) {
				zval *zv = ht->data.arZval, *end = zv + ht->nNumUsed;
				do {
					if (EXPECTED(!Z_ISUNDEF_P(zv))) {
						ht->pDestructor(zv);
					}
				} while (++zv != end);
			} else {
				Bucket *p = ht->data.arBucket, *end = p + ht->nNumUsed;
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						ht->pDestructor(&p->val);
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					}
				} while (++p != end);
			}
		
			SET_INCONSISTENT(HT_DESTROYED);
		} else if (!HT_IS_PACKED(ht)) {
			Bucket *p = ht->data.arBucket, *end = p + ht->nNumUsed;
			do {
				if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
					if (EXPECTED(p->key)) {
						zend_string_release(p->key);
					}
				}
			} while (++p != end);
		}
	} else if (EXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		return;
	}

	HT_FREE_DATA(ht);
}

ZEND_API void zend_array_destroy(HashTable *ht)
{
	IS_CONSISTENT(ht);

	if (ht->nNumUsed) {
		/* In some rare cases destructors of regular arrays may be changed */
		if (UNEXPECTED(ht->pDestructor != ZVAL_PTR_DTOR)) {
			zend_hash_destroy(ht);
			return;
		}
	
		SET_INCONSISTENT(HT_IS_DESTROYING);

		if (HT_IS_PACKED(ht)) {
			zval *zv = ht->data.arZval, *end = zv + ht->nNumUsed;
			do {
				if (EXPECTED(!Z_ISUNDEF_P(zv))) {
					i_zval_ptr_dtor(zv ZEND_FILE_LINE_CC);
				}
			} while (++zv != end);
		} else {
			Bucket *p = ht->data.arBucket, *end = p + ht->nNumUsed;
			do {
				if (EXPECTED(!Z_ISUNDEF(p->val))) {
					i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
					if (EXPECTED(p->key)) {
						zend_string_release(p->key);
					}
				}
			} while (++p != end);
		}
		
		SET_INCONSISTENT(HT_DESTROYED);
	} else if (EXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		return;
	}

	HT_FREE_DATA(ht);
}

ZEND_API void zend_hash_clean(HashTable *ht)
{
	IS_CONSISTENT(ht);

	if (ht->nNumUsed) {
		if (ht->pDestructor) {
			if (HT_IS_PACKED(ht)) {
				zval *zv = ht->data.arZval, *end = zv + ht->nNumUsed;
				do {
					if (EXPECTED(!Z_ISUNDEF_P(zv))) {
						ht->pDestructor(zv);
					}
				} while (++zv != end);
			} else {
				Bucket *p = ht->data.arBucket, *end = p + ht->nNumUsed;
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						ht->pDestructor(&p->val);
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					}
				} while (++p != end);
			}
		} else if (!HT_IS_PACKED(ht)) {
			Bucket *p = ht->data.arBucket, *end = p + ht->nNumUsed;
			do {
				if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
					if (EXPECTED(p->key)) {
						zend_string_release(p->key);
					}
				}
			} while (++p != end);
		}
		if (!HT_IS_PACKED(ht)) {
			memset(ht->arHash, INVALID_IDX, ht->nTableSize * sizeof(uint32_t));	
		}
	}
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->nInternalPointer = INVALID_IDX;
}

ZEND_API void zend_symtable_clean(HashTable *ht)
{
	IS_CONSISTENT(ht);
	ZEND_ASSERT(!HT_IS_PACKED(ht));

	if (ht->nNumUsed) {
		Bucket *p = ht->data.arBucket, *end = p + ht->nNumUsed;
		do {
			if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
				i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
				if (EXPECTED(p->key)) {
					zend_string_release(p->key);
				}
			}
		} while (++p != end);
		if (!HT_IS_PACKED(ht)) {
			memset(ht->arHash, INVALID_IDX, ht->nTableSize * sizeof(uint32_t));	
		}
	}
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->nInternalPointer = INVALID_IDX;
}

ZEND_API void zend_hash_graceful_destroy(HashTable *ht)
{
	uint32_t idx;

	IS_CONSISTENT(ht);

	if (HT_IS_PACKED(ht)) {
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			zval *zv = HT_ZVAL(ht, idx);
			if (Z_ISUNDEF_P(zv)) continue;
			_zend_hash_del_packed_el(ht, idx, zv);
		}
	} else {
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			Bucket *p = HT_BUCKET(ht, idx);
			if (Z_TYPE(p->val) == IS_UNDEF) continue;
			_zend_hash_del_el(ht, idx, p);
		}
	}
	if (ht->u.flags & HASH_FLAG_INITIALIZED) {
		HT_FREE_DATA(ht);
	}

	SET_INCONSISTENT(HT_DESTROYED);
}

ZEND_API void zend_hash_graceful_reverse_destroy(HashTable *ht)
{
	uint32_t idx;

	IS_CONSISTENT(ht);

	idx = ht->nNumUsed;
	if (HT_IS_PACKED(ht)) {
		while (idx-- > 0) {
			zval *zv = HT_ZVAL(ht, idx);
			if (Z_ISUNDEF_P(zv)) continue;
			_zend_hash_del_packed_el(ht, idx, zv);
		}
	} else {
		while (idx-- > 0) {
			Bucket *p = HT_BUCKET(ht, idx);
			if (Z_ISUNDEF(p->val)) continue;
			_zend_hash_del_el(ht, idx, p);
		}
	}

	if (ht->u.flags & HASH_FLAG_INITIALIZED) {
		HT_FREE_DATA(ht);
	}

	SET_INCONSISTENT(HT_DESTROYED);
}

/* This is used to recurse elements and selectively delete certain entries 
 * from a hashtable. apply_func() receives the data and decides if the entry 
 * should be deleted or recursion should be stopped. The following three 
 * return codes are possible:
 * ZEND_HASH_APPLY_KEEP   - continue
 * ZEND_HASH_APPLY_STOP   - stop iteration
 * ZEND_HASH_APPLY_REMOVE - delete the element, combineable with the former
 */

// TODO: Can packing type change during apply?
ZEND_API void zend_hash_apply(HashTable *ht, apply_func_t apply_func)
{
	uint32_t idx;
	int result;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	for (idx = 0; idx < ht->nNumUsed; idx++) {
		if (HT_IS_PACKED(ht)) {
			zval *zv = HT_ZVAL(ht, idx);
			if (Z_ISUNDEF_P(zv)) continue;

			result = apply_func(zv);
			if (result & ZEND_HASH_APPLY_REMOVE) {
				_zend_hash_del_packed_el(ht, idx, zv);
			}
		} else {
			Bucket *p = HT_BUCKET(ht, idx);
			if (Z_ISUNDEF(p->val)) continue;
			
			result = apply_func(&p->val);
			if (result & ZEND_HASH_APPLY_REMOVE) {
				_zend_hash_del_el(ht, idx, p);
			}
		}

		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_apply_with_argument(HashTable *ht, apply_func_arg_t apply_func, void *argument)
{
    uint32_t idx;
	int result;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	for (idx = 0; idx < ht->nNumUsed; idx++) {
		if (HT_IS_PACKED(ht)) {
			zval *zv = HT_ZVAL(ht, idx);
			if (Z_ISUNDEF_P(zv)) continue;

			result = apply_func(zv, argument);
			if (result & ZEND_HASH_APPLY_REMOVE) {
				_zend_hash_del_packed_el(ht, idx, zv);
			}
		} else {
			Bucket *p = HT_BUCKET(ht, idx);
			if (Z_ISUNDEF(p->val)) continue;
			
			result = apply_func(&p->val, argument);
			if (result & ZEND_HASH_APPLY_REMOVE) {
				_zend_hash_del_el(ht, idx, p);
			}
		}

		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_apply_with_arguments(HashTable *ht, apply_func_args_t apply_func, int num_args, ...)
{
	uint32_t idx;
	int result;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);

	for (idx = 0; idx < ht->nNumUsed; idx++) {
		va_list args;
		zend_hash_key hash_key;

		if (HT_IS_PACKED(ht)) {
			zval *zv = HT_ZVAL(ht, idx);
			if (Z_ISUNDEF_P(zv)) continue;

			va_start(args, num_args);
			hash_key.h = idx;
			hash_key.key = NULL;

			result = apply_func(zv, num_args, args, &hash_key);
			if (result & ZEND_HASH_APPLY_REMOVE) {
				_zend_hash_del_packed_el(ht, idx, zv);
			}
		} else {
			Bucket *p = HT_BUCKET(ht, idx);
			if (Z_ISUNDEF(p->val)) continue;

			va_start(args, num_args);
			hash_key.h = p->h;
			hash_key.key = p->key;

			result = apply_func(&p->val, num_args, args, &hash_key);
			if (result & ZEND_HASH_APPLY_REMOVE) {
				_zend_hash_del_el(ht, idx, p);
			}
		}

		va_end(args);
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}

	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_reverse_apply(HashTable *ht, apply_func_t apply_func)
{
	uint32_t idx;
	int result;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	idx = ht->nNumUsed;
	while (idx-- > 0) {
		if (HT_IS_PACKED(ht)) {
			zval *zv = HT_ZVAL(ht, idx);
			if (Z_ISUNDEF_P(zv)) continue;
			
			result = apply_func(zv);
			if (result & ZEND_HASH_APPLY_REMOVE) {
				_zend_hash_del_packed_el(ht, idx, zv);
			}
		} else {
			Bucket *p = HT_BUCKET(ht, idx);
			if (Z_ISUNDEF(p->val)) continue;
			
			result = apply_func(&p->val);
			if (result & ZEND_HASH_APPLY_REMOVE) {
				_zend_hash_del_el(ht, idx, p);
			}
		}

		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


// TODO
ZEND_API void zend_hash_copy(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor)
{
    uint32_t idx;
	zval *new_entry;
	zend_bool setTargetPointer;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

	setTargetPointer = (target->nInternalPointer == INVALID_IDX);
	for (idx = 0; idx < source->nNumUsed; idx++) {		
		zval *zv;
		zend_string *key;
		zend_ulong h;

		if (HT_IS_PACKED(source)) {
			zv = HT_ZVAL(source, idx);
			key = NULL;
			h = idx;
		} else {
			Bucket *p = HT_BUCKET(source, idx);
			zv = &p->val;
			key = p->key;
			h = p->h;
		}
		if (Z_TYPE_P(zv) == IS_UNDEF) continue;

		if (setTargetPointer && source->nInternalPointer == idx) {
			target->nInternalPointer = INVALID_IDX;
		}

		/* INDIRECT element may point to UNDEF-ined slots */
		if (Z_TYPE_P(zv) == IS_INDIRECT) {
			zv = Z_INDIRECT_P(zv);
			if (Z_TYPE_P(zv) == IS_UNDEF) {
				continue;
			}
		}
		if (key) {
			new_entry = zend_hash_update(target, key, zv);
		} else {
			new_entry = zend_hash_index_update(target, h, zv);
		}
		if (pCopyConstructor) {
			pCopyConstructor(new_entry);
		}
	}

	if (target->nInternalPointer == INVALID_IDX && target->nNumOfElements > 0) {
		target->nInternalPointer = zend_hash_next_idx(target, 0);
	}
}


ZEND_API void zend_array_dup(HashTable *target, HashTable *source)
{
    uint32_t idx, target_idx;
	uint32_t nIndex;

	IS_CONSISTENT(source);
	
	target->nTableMask = source->nTableMask;
	target->nTableSize = source->nTableSize;
	target->pDestructor = source->pDestructor;
	target->nInternalPointer = INVALID_IDX;
	target->u.flags = (source->u.flags & ~HASH_FLAG_PERSISTENT) | HASH_FLAG_APPLY_PROTECTION;

	if (target->u.flags & HASH_FLAG_INITIALIZED) {
		if (HT_IS_PACKED(target)) {
			target->nNumUsed = source->nNumUsed;
			target->nNumOfElements = source->nNumOfElements;
			target->nNextFreeElement = source->nNextFreeElement;
			target->data.arZval = safe_pemalloc(target->nTableSize, sizeof(zval), 0, 0);
			target->arHash = (uint32_t*) &uninitialized_bucket;
			target->nInternalPointer = source->nInternalPointer;

			for (idx = 0; idx < source->nNumUsed; idx++) {		
				zval *zv_source = HT_ZVAL(source, idx);
				zval *zv_target = HT_ZVAL(target, idx);
				if (Z_ISUNDEF_P(zv_source)) {
					ZVAL_UNDEF(zv_target);
					continue;
				}

				/* INDIRECT element may point to UNDEF-ined slots */
				if (Z_TYPE_P(zv_source) == IS_INDIRECT) {
					zv_source = Z_INDIRECT_P(zv_source);
					if (Z_TYPE_P(zv_source) == IS_UNDEF) {
						ZVAL_UNDEF(zv_target);
						continue;
					}
				}

				if (Z_OPT_REFCOUNTED_P(zv_source)) {
					if (Z_ISREF_P(zv_source) && Z_REFCOUNT_P(zv_source) == 1) {
						ZVAL_COPY(zv_target, Z_REFVAL_P(zv_source));
					} else {
						ZVAL_COPY(zv_target, zv_source);
					}
				} else {
					ZVAL_COPY_VALUE(zv_target, zv_source);
				}
			}

			if (target->nNumOfElements > 0 &&
			    target->nInternalPointer == INVALID_IDX) {
				idx = 0;
				while (Z_ISUNDEF_P(HT_ZVAL(target, idx))) {
					idx++;
				}
				target->nInternalPointer = idx;
			}
		} else {
			target->nNextFreeElement = source->nNextFreeElement;
			target->data.arBucket = (Bucket *) safe_pemalloc(target->nTableSize, sizeof(Bucket) + sizeof(uint32_t), 0, 0);
			target->arHash = (uint32_t*)(target->data.arBucket + target->nTableSize);
			memset(target->arHash, INVALID_IDX, target->nTableSize * sizeof(uint32_t));

			target_idx = 0;
			for (idx = 0; idx < source->nNumUsed; idx++) {		
				Bucket *p = HT_BUCKET(source, idx), *q;
				zval *data = &p->val;
				if (Z_ISUNDEF_P(data)) continue;

				/* INDIRECT element may point to UNDEF-ined slots */
				if (Z_TYPE_P(data) == IS_INDIRECT) {
					data = Z_INDIRECT_P(data);
					if (Z_TYPE_P(data) == IS_UNDEF) {
						continue;
					}
				}

				if (source->nInternalPointer == idx) {
					target->nInternalPointer = target_idx;
				}

				q = HT_BUCKET(target, target_idx);
				q->h = p->h;
				q->key = p->key;
				if (q->key) {
					zend_string_addref(q->key);
				}
				nIndex = q->h & target->nTableMask;
				Z_NEXT(q->val) = target->arHash[nIndex];
				target->arHash[nIndex] = target_idx;
				if (Z_OPT_REFCOUNTED_P(data)) {
					if (Z_ISREF_P(data) && Z_REFCOUNT_P(data) == 1) {
						ZVAL_COPY(&q->val, Z_REFVAL_P(data));
					} else {
						ZVAL_COPY(&q->val, data);
					}
				} else {
					ZVAL_COPY_VALUE(&q->val, data);
				}
				target_idx++;
			}
			target->nNumUsed = target_idx;
			target->nNumOfElements = target_idx;
			if (target->nNumOfElements > 0 &&
			    target->nInternalPointer == INVALID_IDX) {
				target->nInternalPointer = 0;
			}
		}
	} else {
		target->nNumUsed = 0;
		target->nNumOfElements = 0;
		target->nNextFreeElement = 0;
		target->data.arBucket = NULL;
		target->arHash = (uint32_t*)&uninitialized_bucket;
	}
}


ZEND_API void _zend_hash_merge(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, zend_bool overwrite ZEND_FILE_LINE_DC)
{
    uint32_t idx;
	zval *t;
	uint32_t mode = overwrite ? HASH_UPDATE : HASH_ADD;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

	for (idx = 0; idx < source->nNumUsed; idx++) {
		zval *zv;
		zend_string *key;
		zend_ulong h;

		if (HT_IS_PACKED(source)) {
			zv = HT_ZVAL(source, idx);
			key = NULL;
			h = idx;
		} else {
			Bucket *p = HT_BUCKET(source, idx);
			zv = &p->val;
			key = p->key;
			h = p->h;
		}
		if (Z_TYPE_P(zv) == IS_UNDEF) continue;

		if (key) {
			t = _zend_hash_add_or_update(target, key, zv, mode ZEND_FILE_LINE_RELAY_CC);
			if (t && pCopyConstructor) {
				pCopyConstructor(t);
			}
		} else {
			if (mode==HASH_UPDATE || !zend_hash_index_exists(target, h)) {
			 	t = zend_hash_index_update(target, h, zv);
			 	if (t && pCopyConstructor) {
					pCopyConstructor(t);
				}
			}
		}
	}

	if (target->nNumOfElements > 0) {
		target->nInternalPointer = zend_hash_next_idx(target, 0);
	}
}


/*static zend_bool zend_hash_replace_checker_wrapper(HashTable *target, zval *source_data, Bucket *p, void *pParam, merge_checker_func_t merge_checker_func)
{
	zend_hash_key hash_key;

	hash_key.h = p->h;
	hash_key.key = p->key;
	return merge_checker_func(target, source_data, &hash_key, pParam);
}*/


ZEND_API void zend_hash_merge_ex(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, merge_checker_func_t pMergeSource, void *pParam)
{
	/*uint32_t idx;
	Bucket *p;
	zval *t;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

	for (idx = 0; idx < source->nNumUsed; idx++) {
		p = source->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		if (zend_hash_replace_checker_wrapper(target, &p->val, p, pParam, pMergeSource)) {
			t = zend_hash_update(target, p->key, &p->val);
			if (t && pCopyConstructor) {
				pCopyConstructor(t);
			}
		}
	}
	if (target->nNumOfElements > 0) {
		idx = 0;
		while (Z_TYPE(target->arData[idx].val) == IS_UNDEF) {
			idx++;
		}
		target->nInternalPointer = idx;
	}*/
}


/* Returns SUCCESS if found and FAILURE if not. The pointer to the
 * data is returned in pData. The reason is that there's no reason
 * someone using the hash table might not want to have NULL data
 */
ZEND_API zval *zend_hash_find(const HashTable *ht, zend_string *key)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = zend_hash_find_bucket(ht, key);
	return p ? &p->val : NULL;
}

ZEND_API zval *zend_hash_str_find(const HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	Bucket *p;

	IS_CONSISTENT(ht);

	h = zend_inline_hash_func(str, len);
	p = zend_hash_str_find_bucket(ht, str, len, h);
	return p ? &p->val : NULL;
}

ZEND_API zend_bool zend_hash_exists(const HashTable *ht, zend_string *key)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = zend_hash_find_bucket(ht, key);
	return p ? 1 : 0;
}

ZEND_API zend_bool zend_hash_str_exists(const HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	Bucket *p;

	IS_CONSISTENT(ht);

	h = zend_inline_hash_func(str, len);
	p = zend_hash_str_find_bucket(ht, str, len, h);
	return p ? 1 : 0;
}

ZEND_API zval *zend_hash_index_find(const HashTable *ht, zend_ulong h)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	if (HT_IS_PACKED(ht)) {
		if (h < ht->nNumUsed) {
			zval *zv = HT_ZVAL(ht, h);
			if (!Z_ISUNDEF_P(zv)) {
				return zv;
			}
		}
		return NULL;
	}

	p = zend_hash_index_find_bucket(ht, h);
	return p ? &p->val : NULL;
}


ZEND_API zend_bool zend_hash_index_exists(const HashTable *ht, zend_ulong h)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	if (HT_IS_PACKED(ht)) {
		if (h < ht->nNumUsed) {
			if (!Z_ISUNDEF_P(HT_ZVAL(ht, h))) {
				return 1;
			}
		}
		return 0;
	}

	p = zend_hash_index_find_bucket(ht, h);
	return p ? 1 : 0;
}


ZEND_API void zend_hash_internal_pointer_reset_ex(HashTable *ht, HashPosition *pos)
{
	IS_CONSISTENT(ht);
	*pos = zend_hash_next_idx(ht, 0);
}

ZEND_API void zend_hash_internal_pointer_end_ex(HashTable *ht, HashPosition *pos)
{
	IS_CONSISTENT(ht);
	*pos = zend_hash_prev_idx(ht, ht->nNumUsed);
}


ZEND_API int zend_hash_move_forward_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;

	IS_CONSISTENT(ht);
	if (idx == INVALID_IDX) {
		return FAILURE;
	}

	*pos = zend_hash_next_idx(ht, idx + 1);
	return *pos == INVALID_IDX ? FAILURE : SUCCESS;
}

ZEND_API int zend_hash_move_backwards_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;

	IS_CONSISTENT(ht);
	if (idx == INVALID_IDX) {
		return FAILURE;
	}

	*pos = zend_hash_prev_idx(ht, idx);
	return *pos == INVALID_IDX ? FAILURE : SUCCESS;
}


/* This function should be made binary safe  */
ZEND_API int zend_hash_get_current_key_ex(const HashTable *ht, zend_string **str_index, zend_ulong *num_index, HashPosition *pos)
{
	uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx == INVALID_IDX) {
		return HASH_KEY_NON_EXISTENT;
	}
	if (HT_IS_PACKED(ht)) {
		*num_index = idx;
		return HASH_KEY_IS_LONG;
	}

	p = HT_BUCKET(ht, idx);
	if (p->key) {
		*str_index = p->key;
		return HASH_KEY_IS_STRING;
	} else {
		*num_index = p->h;
		return HASH_KEY_IS_LONG;
	}
}

ZEND_API void zend_hash_get_current_key_zval_ex(const HashTable *ht, zval *key, HashPosition *pos)
{
	uint32_t idx = *pos;

	IS_CONSISTENT(ht);
	if (idx == INVALID_IDX) {
		ZVAL_NULL(key);
	} else if (HT_IS_PACKED(ht)) {
		ZVAL_LONG(key, idx);
	} else {
		Bucket *p = HT_BUCKET(ht, idx);
		if (p->key) {
			ZVAL_STR_COPY(key, p->key);
		} else {
			ZVAL_LONG(key, p->h);
		}
	}
}

ZEND_API int zend_hash_get_current_key_type_ex(HashTable *ht, HashPosition *pos)
{
    uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx == INVALID_IDX) {
		return HASH_KEY_NON_EXISTENT;
	}
	if (HT_IS_PACKED(ht)) {
		return HASH_KEY_IS_LONG;
	}

	p = HT_BUCKET(ht, idx);
	if (p->key) {
		return HASH_KEY_IS_STRING;
	} else {
		return HASH_KEY_IS_LONG;
	}
}


ZEND_API zval *zend_hash_get_current_data_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;

	IS_CONSISTENT(ht);
	if (idx == INVALID_IDX) {
		return NULL;
	}

	if (HT_IS_PACKED(ht)) {
		return HT_ZVAL(ht, idx);
	} else {
		return &HT_BUCKET(ht, idx)->val;
	}
}

ZEND_API int zend_hash_sort(HashTable *ht, sort_func_t sort_func,
							compare_func_t compar, zend_bool renumber)
{
	Bucket *p;
	uint32_t i, j;

	IS_CONSISTENT(ht);

	/* Doesn't require sorting */
	if (!(ht->nNumOfElements>1) && !(renumber && ht->nNumOfElements>0)) {
		return SUCCESS;
	}

	// TODO
	if (HT_IS_PACKED(ht)) {
		zend_hash_packed_to_hash(ht);
	}

	if (ht->nNumUsed == ht->nNumOfElements) {
		i = ht->nNumUsed;
	} else {
		for (j = 0, i = 0; j < ht->nNumUsed; j++) {
			p = HT_BUCKET(ht, j);
			if (Z_TYPE(p->val) == IS_UNDEF) continue;
			if (i != j) {
				*HT_BUCKET(ht, i) = *p;
			}
			i++;
		}
	}

	(*sort_func)((void *) ht->data.arBucket, i, sizeof(Bucket), compar);

	HANDLE_BLOCK_INTERRUPTIONS();
	ht->nNumUsed = i;
	ht->nInternalPointer = 0;

	if (renumber) {
		for (j = 0; j < i; j++) {
			p = HT_BUCKET(ht, j);
			p->h = j;
			if (p->key) {
				zend_string_release(p->key);
				p->key = NULL;
			}
		}

		ht->nNextFreeElement = i;
	}

	/*if (ht->u.flags & HASH_FLAG_PACKED) {
		if (!renumber) {
			zend_hash_packed_to_hash(ht);
		}
	} else {*/
		if (renumber) {
			zend_hash_to_packed(ht);
			/*
			ht->u.flags |= HASH_FLAG_PACKED;
			ht->nTableMask = 0;
			ht->arData = erealloc(ht->arData, ht->nTableSize * sizeof(Bucket));
			ht->arHash = (uint32_t*)&uninitialized_bucket;*/
		} else {
			zend_hash_rehash(ht);
		}
	/*}*/

	HANDLE_UNBLOCK_INTERRUPTIONS();

	return SUCCESS;
}


ZEND_API int zend_hash_compare(HashTable *ht1, HashTable *ht2, compare_zval_func_t compar, zend_bool ordered)
{
	uint32_t idx1, idx2;
	Bucket *p1, *p2 = NULL;
	int result;
	zval *pData1, *pData2;

	IS_CONSISTENT(ht1);
	IS_CONSISTENT(ht2);

	result = ht1->nNumOfElements - ht2->nNumOfElements;
	if (result != 0) {
		return result;
	}

	HASH_PROTECT_RECURSION(ht1); 
	HASH_PROTECT_RECURSION(ht2); 

	if (HT_IS_PACKED(ht1) && HT_IS_PACKED(ht2)) {
		uint32_t idx, idx_max = MIN(ht1->nNumUsed, ht2->nNumUsed);
		for (idx = 0; idx < idx_max; idx++) {
			zval *zv1 = HT_ZVAL(ht1, idx);
			zval *zv2 = HT_ZVAL(ht2, idx);
			if (Z_TYPE_P(zv1) == IS_INDIRECT) {
				zv1 = Z_INDIRECT_P(zv1);
			}
			if (Z_TYPE_P(zv2) == IS_INDIRECT) {
				zv2 = Z_INDIRECT_P(zv2);
			}
			if (Z_ISUNDEF_P(zv1) && Z_ISUNDEF_P(zv2)) {
				continue;
			}
			if (Z_ISUNDEF_P(zv1) || Z_ISUNDEF_P(zv2)) {
				HASH_UNPROTECT_RECURSION(ht1); 
				HASH_UNPROTECT_RECURSION(ht2); 
				return 1; // ???
			}

			result = compar(zv1, zv2);
			if (result != 0) {
				HASH_UNPROTECT_RECURSION(ht1); 
				HASH_UNPROTECT_RECURSION(ht2); 
				return result;
			}
		}
		HASH_UNPROTECT_RECURSION(ht1); 
		HASH_UNPROTECT_RECURSION(ht2); 
		return 0;
	} else if (HT_IS_PACKED(ht1)) {
		// TODO (?)
		zend_hash_packed_to_hash(ht1);
	} else if (HT_IS_PACKED(ht2)) {
		// TODO (?)
		zend_hash_packed_to_hash(ht2);
	}

	for (idx1 = 0, idx2 = 0; idx1 < ht1->nNumUsed; idx1++) {
		p1 = HT_BUCKET(ht1, idx1);
		if (Z_TYPE(p1->val) == IS_UNDEF) continue;

		if (ordered) {
			while (1) {
				p2 = HT_BUCKET(ht2, idx2);
				if (idx2 == ht2->nNumUsed) {
					HASH_UNPROTECT_RECURSION(ht1); 
					HASH_UNPROTECT_RECURSION(ht2); 
					return 1; /* That's not supposed to happen */
				}
				if (Z_TYPE(p2->val) != IS_UNDEF) break;
				idx2++;
			}						
			if (p1->key == NULL && p2->key == NULL) { /* numeric indices */
				result = p1->h - p2->h;
				if (result != 0) {
					HASH_UNPROTECT_RECURSION(ht1); 
					HASH_UNPROTECT_RECURSION(ht2); 
					return result;
				}
			} else { /* string indices */
				size_t len0 = (p1->key ? p1->key->len : 0);
				size_t len1 = (p2->key ? p2->key->len : 0);
				if (len0 != len1) {
					HASH_UNPROTECT_RECURSION(ht1); 
					HASH_UNPROTECT_RECURSION(ht2); 
					return len0 > len1 ? 1 : -1;
				}
				result = memcmp(p1->key->val, p2->key->val, p1->key->len);
				if (result != 0) {
					HASH_UNPROTECT_RECURSION(ht1); 
					HASH_UNPROTECT_RECURSION(ht2); 
					return result;
				}
			}
			pData2 = &p2->val;
		} else {
			if (p1->key == NULL) { /* numeric index */
				pData2 = zend_hash_index_find(ht2, p1->h);
				if (pData2 == NULL) {
					HASH_UNPROTECT_RECURSION(ht1); 
					HASH_UNPROTECT_RECURSION(ht2); 
					return 1;
				}
			} else { /* string index */
				pData2 = zend_hash_find(ht2, p1->key);
				if (pData2 == NULL) {
					HASH_UNPROTECT_RECURSION(ht1); 
					HASH_UNPROTECT_RECURSION(ht2); 
					return 1;
				}
			}
		}
		pData1 = &p1->val;
		if (Z_TYPE_P(pData1) == IS_INDIRECT) {
			pData1 = Z_INDIRECT_P(pData1);
		}
		if (Z_TYPE_P(pData2) == IS_INDIRECT) {
			pData2 = Z_INDIRECT_P(pData2);
		}
		if (Z_TYPE_P(pData1) == IS_UNDEF) {
			if (Z_TYPE_P(pData2) != IS_UNDEF) {
				return -1;
			}
		} else if (Z_TYPE_P(pData2) == IS_UNDEF) {
			return 1;
		} else {
			result = compar(pData1, pData2);
		}
		if (result != 0) {
			HASH_UNPROTECT_RECURSION(ht1); 
			HASH_UNPROTECT_RECURSION(ht2); 
			return result;
		}
		if (ordered) {
			idx2++;
		}
	}
	
	HASH_UNPROTECT_RECURSION(ht1); 
	HASH_UNPROTECT_RECURSION(ht2); 
	return 0;
}


ZEND_API zval *zend_hash_minmax(const HashTable *ht, compare_func_t compar, uint32_t flag)
{
	uint32_t idx;
	Bucket *p, *res;

	IS_CONSISTENT(ht);

	if (ht->nNumOfElements == 0) {
		return NULL;
	}

	// TODO this could easily work on packed hashes, but compare_func_t sucks
	if (HT_IS_PACKED(ht)) {
		zend_hash_packed_to_hash(ht);
	}

	idx = zend_unpacked_next_idx(ht, 0);
	res = HT_BUCKET(ht, idx);
	for (; idx < ht->nNumUsed; idx++) {
		p = HT_BUCKET(ht, idx);
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		
		if (flag) {
			if (compar(res, p) < 0) { /* max */
				res = p;
			}
		} else {
			if (compar(res, p) > 0) { /* min */
				res = p;
			}
		}
	}
	return &res->val;
}

ZEND_API int _zend_handle_numeric_str_ex(const char *key, size_t length, zend_ulong *idx)
{
	register const char *tmp = key;

	const char *end = key + length;
	ZEND_ASSERT(*end == '\0');

	if (*tmp == '-') {
		tmp++;
	}

	if ((*tmp == '0' && length > 1) /* numbers with leading zeros */
	 || (end - tmp > MAX_LENGTH_OF_LONG - 1) /* number too long */
	 || (SIZEOF_ZEND_LONG == 4 &&
	     end - tmp == MAX_LENGTH_OF_LONG - 1 &&
	     *tmp > '2')) { /* overflow */
		return 0;
	}
	*idx = (*tmp - '0');
	while (1) {
		++tmp;
		if (tmp == end) {
			if (*key == '-') {
				if (*idx-1 > ZEND_LONG_MAX) { /* overflow */
					return 0;
				}
				*idx = 0 - *idx;
			} else if (*idx > ZEND_LONG_MAX) { /* overflow */
				return 0;
			}
			return 1;
		}
		if (*tmp <= '9' && *tmp >= '0') {
			*idx = (*idx * 10) + (*tmp - '0');
		} else {
			return 0;
		}
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */

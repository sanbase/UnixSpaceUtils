/*
	Copyright 1990, 1991, 1992, 1993, 1994 Free Software Foundation, Inc.
	Written May 1989 by Mike Haertel.
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Changed by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: malloc.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
/*
 *       Из всех испытанных реализаций распределения динамической
 *       памяти ( порядка 10 штук ) эта реализация показывает наи-
 *       лучшие показатели по памяти-быстродействию.
 */
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <stddef.h>
#include "ned.h"

#ifndef _MALLOC_INTERNAL

#define	__malloc_size_t	size_t

extern void * malloc  (size_t __size);
extern void * realloc  (void * __ptr, size_t __size);
extern void free  (void * __ptr);

#define INT_BIT		  (CHAR_BIT * sizeof(int))
#define BLOCKLOG	  (INT_BIT > 16 ? 12 : 9)
#define BLOCKSIZE	  (1 << BLOCKLOG)
#define BLOCKIFY(SIZE)	  (((SIZE) + BLOCKSIZE - 1) / BLOCKSIZE)
#define HEAP		  (INT_BIT > 16 ? 4194304 : 65536)
#define FINAL_FREE_BLOCKS	8

typedef union {
	struct {
		int type;
		union {
			struct {
				__malloc_size_t nfree;
				__malloc_size_t first;
			} 
			frag;
			__malloc_size_t size;
		} 
		info;
	} 
	busy;
	struct {
		__malloc_size_t size;
		__malloc_size_t next;
		__malloc_size_t prev;
	} 
	free;
} 
malloc_info;

extern char *_heapbase;

extern malloc_info *_heapinfo;

#define BLOCK(A)	(((char *) (A) - _heapbase) / BLOCKSIZE + 1)
#define ADDRESS(B)	((void *) (((B) - 1) * BLOCKSIZE + _heapbase))

extern __malloc_size_t _heapindex;

extern __malloc_size_t _heaplimit;

struct list {
	struct list *next;
	struct list *prev;
};

extern struct list _fraghead[];

struct alignlist {
	struct alignlist *next;
	void * aligned;
	void * exact;
};
extern struct alignlist *_aligned_blocks;

extern __malloc_size_t _chunks_used;
extern __malloc_size_t _bytes_used;
extern __malloc_size_t _chunks_free;
extern __malloc_size_t _bytes_free;

extern void _free_internal  (void * __ptr);

extern void * (*__morecore)  (ptrdiff_t __size);

extern void * __default_morecore  (ptrdiff_t __size);

extern void (*__after_morecore_hook)  (void);

extern int __malloc_initialized;

extern void (*__free_hook)  (void * __ptr);
extern void * (*__malloc_hook)  (size_t __size);
extern void * (*__realloc_hook)  (void * __ptr, size_t __size);

#define	 __getpagesize()	getpagesize()

void * (*__morecore)  (ptrdiff_t __size) = __default_morecore;

void * (*__malloc_hook)  (__malloc_size_t __size);

char *_heapbase;

malloc_info *_heapinfo;

static __malloc_size_t heapsize;

__malloc_size_t _heapindex;

__malloc_size_t _heaplimit;

struct list _fraghead[BLOCKLOG];

__malloc_size_t _chunks_used;
__malloc_size_t _bytes_used;
__malloc_size_t _chunks_free;
__malloc_size_t _bytes_free;

int __malloc_initialized;

void (*__after_morecore_hook)  (void);

static void * align (__malloc_size_t size)
{
	void * result;
	unsigned long int adj;

	result = (*__morecore) (size);
	adj = (unsigned long int) ((unsigned long int) ((char *) result -
	    (char *) NULL)) % BLOCKSIZE;
	if (adj != 0){
		adj = BLOCKSIZE - adj;
		(void) (*__morecore) (adj);
		result = (char *) result + adj;
	}
	if (__after_morecore_hook) (*__after_morecore_hook) ();
	return result;
}

static int initialize (void)
{
	heapsize = HEAP / BLOCKSIZE;
	_heapinfo = (malloc_info *) align (heapsize * sizeof (malloc_info));
	if (_heapinfo == NULL) return 0;
	memset (_heapinfo, 0, heapsize * sizeof (malloc_info));
	_heapinfo[0].free.size = 0;
	_heapinfo[0].free.next = _heapinfo[0].free.prev = 0;
	_heapindex = 0;
	_heapbase = (char *) _heapinfo;
	_bytes_used = heapsize * sizeof (malloc_info);
	_chunks_used = 1; __malloc_initialized = 1;
	return 1;
}

static void * morecore (__malloc_size_t size)
{
	void * result;
	malloc_info *newinfo, *oldinfo;
	__malloc_size_t newsize;

	result = align (size);
	if (result == NULL) return NULL;
	if ((__malloc_size_t) BLOCK ((char *) result + size) > heapsize) {
		newsize = heapsize;
		while ((__malloc_size_t) BLOCK ((char *) result + size) > newsize)
			newsize *= 2;
		newinfo = (malloc_info *) align (newsize * sizeof (malloc_info));
		if (newinfo == NULL) {
			(*__morecore) (-size); return NULL;
		}
		memcpy (newinfo, _heapinfo, heapsize * sizeof (malloc_info));
		memset (&newinfo[heapsize], 0,
		(newsize - heapsize) * sizeof (malloc_info));
		oldinfo = _heapinfo;
		newinfo[BLOCK (oldinfo)].busy.type = 0;
		newinfo[BLOCK (oldinfo)].busy.info.size
		    = BLOCKIFY (heapsize * sizeof (malloc_info));
		_heapinfo = newinfo;
		_bytes_used += newsize * sizeof (malloc_info);
		++_chunks_used;
		_free_internal (oldinfo);
		heapsize = newsize;
	}
	_heaplimit = BLOCK ((char *) result + size);
	return result;
}

void * malloc (__malloc_size_t size)
{
	void * result;
	__malloc_size_t block, blocks, lastblocks, start;
	register __malloc_size_t i;
	struct list *next;

	if (size == 0) return NULL;
	if (__malloc_hook != NULL) return (*__malloc_hook) (size);
	if (!__malloc_initialized)
		if (!initialize ()) return NULL;
	if (size < sizeof (struct list)) size = sizeof (struct list);
	if (size <= BLOCKSIZE / 2) {
		register __malloc_size_t log = 1;
		--size;
		while ((size /= 2) != 0) ++log;
		next = _fraghead[log].next;
		if (next != NULL) {
			result = (void *) next;
			next->prev->next = next->next;
			if (next->next != NULL)
				next->next->prev = next->prev;
			block = BLOCK (result);
			if (--_heapinfo[block].busy.info.frag.nfree != 0)
				_heapinfo[block].busy.info.frag.first = (unsigned long int)
					((unsigned long int) ((char *) next->next - (char *) NULL)
					    % BLOCKSIZE) >> log;
			++_chunks_used;
			_bytes_used += 1 << log;
			--_chunks_free;
			_bytes_free -= 1 << log;
		} else {
			result = malloc (BLOCKSIZE);
			if (result == NULL) return NULL;
			for (i = 1; i < (__malloc_size_t) (BLOCKSIZE >> log); ++i)
			{
				next = (struct list *) ((char *) result + (i << log));
				next->next = _fraghead[log].next;
				next->prev = &_fraghead[log];
				next->prev->next = next;
				if (next->next != NULL)
					next->next->prev = next;
			}
			block = BLOCK (result);
			_heapinfo[block].busy.type = log;
			_heapinfo[block].busy.info.frag.nfree = i - 1;
			_heapinfo[block].busy.info.frag.first = i - 1;
			_chunks_free += (BLOCKSIZE >> log) - 1;
			_bytes_free += BLOCKSIZE - (1 << log);
			_bytes_used -= BLOCKSIZE - (1 << log);
		}
	} else {
		blocks = BLOCKIFY (size);
		start = block = _heapindex;
		while (_heapinfo[block].free.size < blocks) {
			block = _heapinfo[block].free.next;
			if (block == start) {
				block = _heapinfo[0].free.prev;
				lastblocks = _heapinfo[block].free.size;
				if (_heaplimit != 0 && block + lastblocks == _heaplimit &&
				    (*__morecore) (0) == ADDRESS (block + lastblocks) &&
				    (morecore ((blocks - lastblocks) * BLOCKSIZE)) != NULL)
				{
					block = _heapinfo[0].free.prev;
					_heapinfo[block].free.size += (blocks - lastblocks);
					_bytes_free += (blocks - lastblocks) * BLOCKSIZE;
					continue;
				}
				result = morecore (blocks * BLOCKSIZE);
				if (result == NULL) return NULL;
				block = BLOCK (result);
				_heapinfo[block].busy.type = 0;
				_heapinfo[block].busy.info.size = blocks;
				++_chunks_used;
				_bytes_used += blocks * BLOCKSIZE;
				return result;
			}
		}
		result = ADDRESS (block);
		if (_heapinfo[block].free.size > blocks) {
			_heapinfo[block + blocks].free.size
			    = _heapinfo[block].free.size - blocks;
			_heapinfo[block + blocks].free.next
			    = _heapinfo[block].free.next;
			_heapinfo[block + blocks].free.prev
			    = _heapinfo[block].free.prev;
			_heapinfo[_heapinfo[block].free.prev].free.next
			    = _heapinfo[_heapinfo[block].free.next].free.prev
			    = _heapindex = block + blocks;
		} else {
			_heapinfo[_heapinfo[block].free.next].free.prev
			    = _heapinfo[block].free.prev;
			_heapinfo[_heapinfo[block].free.prev].free.next
			    = _heapindex = _heapinfo[block].free.next;
			--_chunks_free;
		}
		_heapinfo[block].busy.type = 0;
		_heapinfo[block].busy.info.size = blocks;
		++_chunks_used;
		_bytes_used += blocks * BLOCKSIZE;
		_bytes_free -= blocks * BLOCKSIZE;
	}
	return result;
}

void (*__free_hook)  (void * __ptr);

struct alignlist *_aligned_blocks = NULL;

void _free_internal (void *ptr)
{
	int type;
	__malloc_size_t block, blocks;
	register __malloc_size_t i;
	struct list *prev, *next;

	block = BLOCK (ptr);
	type = _heapinfo[block].busy.type;
	switch (type) {
	case 0:
		--_chunks_used;
		_bytes_used -= _heapinfo[block].busy.info.size * BLOCKSIZE;
		_bytes_free += _heapinfo[block].busy.info.size * BLOCKSIZE;

		i = _heapindex;
		if (i > block)
			while (i > block)
				i = _heapinfo[i].free.prev;
		else  {
			do
			    i = _heapinfo[i].free.next;
			while (i > 0 && i < block);
			i = _heapinfo[i].free.prev;
		}

		if (block == i + _heapinfo[i].free.size) {
			_heapinfo[i].free.size += _heapinfo[block].busy.info.size;
			block = i;
		} else {
			_heapinfo[block].free.size = _heapinfo[block].busy.info.size;
			_heapinfo[block].free.next = _heapinfo[i].free.next;
			_heapinfo[block].free.prev = i;
			_heapinfo[i].free.next = block;
			_heapinfo[_heapinfo[block].free.next].free.prev = block;
			++_chunks_free;
		}
		if (block + _heapinfo[block].free.size == _heapinfo[block].free.next)
		{
			_heapinfo[block].free.size
			    += _heapinfo[_heapinfo[block].free.next].free.size;
			_heapinfo[block].free.next
			    = _heapinfo[_heapinfo[block].free.next].free.next;
			_heapinfo[_heapinfo[block].free.next].free.prev = block;
			--_chunks_free;
		}
		blocks = _heapinfo[block].free.size;
		if (blocks >= FINAL_FREE_BLOCKS && block + blocks == _heaplimit
		    && (*__morecore) (0) == ADDRESS (block + blocks))
		{
			register __malloc_size_t bytes = blocks * BLOCKSIZE;
			_heaplimit -= blocks;
			(*__morecore) (-bytes);
			_heapinfo[_heapinfo[block].free.prev].free.next
			    = _heapinfo[block].free.next;
			_heapinfo[_heapinfo[block].free.next].free.prev
			    = _heapinfo[block].free.prev;
			block = _heapinfo[block].free.prev;
			--_chunks_free;
			_bytes_free -= bytes;
		}
		_heapindex = block;
		break;
	default:
		--_chunks_used;
		_bytes_used -= 1 << type;
		++_chunks_free;
		_bytes_free += 1 << type;
		prev = (struct list *) ((char *) ADDRESS (block) +
		    (_heapinfo[block].busy.info.frag.first << type));
		if (_heapinfo[block].busy.info.frag.nfree == (BLOCKSIZE >> type) - 1)
		{
			next = prev;
			for (i = 1; i < (__malloc_size_t) (BLOCKSIZE >> type); ++i)
				next = next->next;
			prev->prev->next = next;
			if (next != NULL)
				next->prev = prev->prev;
			_heapinfo[block].busy.type = 0;
			_heapinfo[block].busy.info.size = 1;
			++_chunks_used;
			_bytes_used += BLOCKSIZE;
			_chunks_free -= BLOCKSIZE >> type;
			_bytes_free -= BLOCKSIZE;
			free (ADDRESS (block));
		} else
		        if (_heapinfo[block].busy.info.frag.nfree != 0) {
			        next = (struct list *) ptr;
			        next->next = prev->next;
			        next->prev = prev;
			        prev->next = next;
			        if (next->next != NULL)
				        next->next->prev = next;
			        ++_heapinfo[block].busy.info.frag.nfree;
		        } else {
			        prev = (struct list *) ptr;
			        _heapinfo[block].busy.info.frag.nfree = 1;
			        _heapinfo[block].busy.info.frag.first = (unsigned long int)
				        ((unsigned long int) ((char *) ptr - (char *) NULL)
				            % BLOCKSIZE >> type);
			        prev->next = _fraghead[type].next;
			        prev->prev = &_fraghead[type];
			        prev->prev->next = prev;
			        if (prev->next != NULL)
				        prev->next->prev = prev;
		        }
		break;
	}
}

void free (void *ptr)
{
	register struct alignlist *l;

	if (ptr == NULL) return;
	for (l = _aligned_blocks; l != NULL; l = l->next)
		if (l->aligned == ptr) {
			l->aligned = NULL;
			ptr = l->exact;
			break;
		}

	if (__free_hook != NULL)
		(*__free_hook) (ptr);
	else
		_free_internal (ptr);
}

void * (*__realloc_hook)  (void * __ptr, __malloc_size_t __size);

void *
realloc (ptr, size)
void * ptr;
__malloc_size_t size;
{
	void * result;
	int type;
	__malloc_size_t block, blocks, oldlimit;

	if (size == 0) {
		free (ptr); return malloc (0);
	} else
	        if (ptr == NULL) return malloc (size);
	if (__realloc_hook != NULL)
		return (*__realloc_hook) (ptr, size);
	block = BLOCK (ptr);
	type = _heapinfo[block].busy.type;
	switch (type) {
	case 0:
		if (size <= BLOCKSIZE / 2) {
			result = malloc (size);
			if (result != NULL) {
				memcpy (result, ptr, size);
				_free_internal (ptr);
				return result;
			}
		}
		blocks = BLOCKIFY (size);
		if (blocks < _heapinfo[block].busy.info.size) {
			_heapinfo[block + blocks].busy.type = 0;
			_heapinfo[block + blocks].busy.info.size
			    = _heapinfo[block].busy.info.size - blocks;
			_heapinfo[block].busy.info.size = blocks;
			++_chunks_used;
			_free_internal (ADDRESS (block + blocks));
			result = ptr;
		}  else
		        if (blocks == _heapinfo[block].busy.info.size)
			        result = ptr;
		        else {
			        blocks = _heapinfo[block].busy.info.size;
			        oldlimit = _heaplimit;
			        _heaplimit = 0;
			        _free_internal (ptr);
			        _heaplimit = oldlimit;
			        result = malloc (size);
			        if (result == NULL) {
				        if (_heapindex == block)
					        (void) malloc (blocks * BLOCKSIZE);
				        else {
					        void * previous = malloc ((block - _heapindex) * BLOCKSIZE);
					        (void) malloc (blocks * BLOCKSIZE);
					        _free_internal (previous);
				        }
				        return NULL;
			        }
			        if (ptr != result)
				        memmove (result, ptr, blocks * BLOCKSIZE);
		        }
		break;

	default:
		if (size > (__malloc_size_t) (1 << (type - 1)) &&
		    size <= (__malloc_size_t) (1 << type))
			result = ptr;
		else {
			result = malloc (size);
			if (result == NULL) return NULL;
			memcpy (result, ptr, min (size, (__malloc_size_t) 1 << type));
			free (ptr);
		}
		break;
	}
	return result;
}
#define	__sbrk	sbrk

void * __default_morecore (ptrdiff_t increment)
{
	void * result = (void *) __sbrk ((int) increment);
	if (result == (void *) -1) return NULL;
	return result;
}
#endif

char *Malloc(int nbytes)
{
	char *ch;

	if((ch=(char *)malloc(nbytes))==NULL) emess(pmes(1));
	return(ch);
}

char *Realloc(void *p,int nbytes)
{
	char *ch;

	if((char *)p==NULL) {
		if((ch=(char *)Malloc(nbytes))==NULL) emess(pmes(1));
	}
	else
		if((ch=(char *)realloc(p,nbytes))==NULL) emess(pmes(1));
	return(ch);
}

void Free(void *p)
{
	if(p!=NULL) free(p);
}


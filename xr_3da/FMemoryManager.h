#ifndef _MMGR_H_
#define _MMGR_H_

// memory manager
void mmgrInitialize			(int dbgLevel);
void mmgrDone				(void);
void mmgrMessage			(const char *logMSG, const char *dop = NULL);


#ifdef DEBUG
// rewritten routines for debugging
char * __fastcall _STRDUP	(const char * string);

// reassigned routines
#define strdup(a)			_STRDUP(a)
#define malloc(s)			_malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define calloc(c, s)		_calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(p, s)		_realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define _expand(p, s)		_expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define free(p)				_free_dbg(p, _NORMAL_BLOCK)
#define _msize(p)			_msize_dbg(p, _NORMAL_BLOCK)
#define	new					new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define	delete(a)			delete(a, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#endif

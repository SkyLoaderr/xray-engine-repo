#ifndef _MMGR_H_
#define _MMGR_H_

// memory manager
ENGINE_API void		mmgrInitialize	(int dbgLevel);
ENGINE_API void		mmgrDone		(void);
ENGINE_API void		mmgrMessage		(const char *logMSG, const char *dop = NULL);

// ENGINE_API LPSTR	_STRDUP			(LPCSTR string);

/*
#ifdef DEBUG
// rewritten routines for debugging

// reassigned routines
#define xr_malloc(s)			_malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define calloc(c, s)		_calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define xr_realloc(p, s)		_realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define _expand(p, s)		_expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define xr_free(p)				_free_dbg(p, _NORMAL_BLOCK)
#define _msize(p)			_msize_dbg(p, _NORMAL_BLOCK)
#define	new					new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define	delete(a)			delete(a, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif
*/

#endif

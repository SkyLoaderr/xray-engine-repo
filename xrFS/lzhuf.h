#ifndef _LZ_H_
#define _LZ_H_

extern XRFS_API unsigned	_writeLZ		(int hf, void* d, unsigned size);
extern XRFS_API unsigned	_readLZ			(int hf, void* &d, unsigned size);

extern XRFS_API void		_compressLZ		(BYTE** dest, unsigned* dest_sz, void* src, unsigned src_sz);
extern XRFS_API void		_decompressLZ	(BYTE** dest, unsigned* dest_sz, void* src, unsigned src_sz);

#endif

#ifndef rt_compressorH
#define rt_compressorH
#pragma once

extern XRFS_API void	rtc_initialize	();
extern XRFS_API u32		rtc_compress	(void *dst, u32 dst_len, const void* src, u32 src_len);
extern XRFS_API u32		rtc_decompress	(void *dst, u32 dst_len, const void* src, u32 src_len);
extern XRFS_API u32		rtc_csize		(u32 in);

#endif

#pragma once

extern XRCORE_API void	rtc_initialize	();
extern XRCORE_API int	rtc_compress	(void *dst, u32 dst_len, const void* src, u32 src_len);
extern XRCORE_API int	rtc_decompress	(void *dst, u32 dst_len, const void* src, u32 src_len);
extern XRCORE_API u32	rtc_csize		(u32 in);
#include "stdafx.h"
#include "rt_lzo.h"

void	rtc_initialize	()
{
	R_ASSERT		(lzo_init()==LZO_E_OK);
}

u32		rtc_csize		(u32 in)
{
	R_ASSERT		(in);
	return			in + in/64 + 16 + 3;
}

u32		rtc_compress	(void *dst, u32 dst_len, const void* src, u32 src_len)
{
	u32		out_size	= dst_len;
	int r = lzo1x_1_compress	( 
		(const lzo_byte *) src, (lzo_uint)	src_len, 
		(lzo_byte *) dst,		(lzo_uintp) &out_size, 
		0);
	R_ASSERT(r==LZO_E_OK);
	return	out_size;
}
u32		rtc_decompress	(void *dst, u32 dst_len, const void* src, u32 src_len)
{
	u32		out_size	= dst_len;
	int r = lzo1x_decompress	( 
		(const lzo_byte *) src, (lzo_uint)	src_len,
		(lzo_byte *) dst,		(lzo_uintp) &out_size,
		0);
	R_ASSERT(r==LZO_E_OK);
	return	out_size;
}

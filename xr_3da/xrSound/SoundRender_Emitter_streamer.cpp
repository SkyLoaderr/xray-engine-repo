#include "stdafx.h"
#pragma hdrstop

#include "SoundRender_Core.h"
#include "SoundRender_Source.h"
#include "SoundRender_Emitter.h"

void	CSoundRender_Emitter::fill_data		(u8* dest, u32 offset, u32 size)
{
	u32		line_size						= SoundRender.cache.get_linesize();
	u32		line							= offset / line_size;

	// prepare for first line (it can be unaligned)
	u32		line_offs						= offset - line*line_size;
	u32		line_amount						= line_size - line_offs;
	while	(size)
	{
		// cache acess
		if (SoundRender.cache.request(source->CAT,line))		{
			// fake decompression
			void*	ptr			= SoundRender.cache.get_dataptr	(source->CAT,line);
			Memory.mem_copy		(ptr,LPBYTE(source->wave)+offset,_min(size,line_size));
		}

		// fill block
		u32		blk_size	= _min(size,line_amount);
		u8*		ptr			= (u8*)SoundRender.cache.get_dataptr(source->CAT,line);
		Memory.mem_copy		(dest,ptr+line_offs,blk_size);
		
		// advance
		line		++	;
		size		-=	blk_size;
		dest		+=	blk_size;
		line_offs	=	0;
		line_amount	=	line_size;
	}

	//  --- previously it was something like this
	//	Memory.mem_copy		(ptr,wave+offset,size);
}

void	CSoundRender_Emitter::fill_block	(void* ptr, u32 size)
{
	//Msg			("stream: %10s - [%X]:%d, p=%d, t=%d",source->fname,ptr,size,position,source->dwBytesTotal);
	LPBYTE		dest = LPBYTE(ptr);
	if ((position+size) > source->dwBytesTotal)
	{
		// We are reaching the end of data, what to do?
		switch (state)
		{
		case stPlaying:
			{
				// Fill as much data as we can, zeroing remainder
				if (position >= source->dwBytesTotal)
				{
					// ??? We requested the block after remainder - just zero
					Memory.mem_fill	(dest,0,size);
					//Msg				("        playing: zero");
				} else {
					// Calculate remainder
					u32	sz_data		= source->dwBytesTotal - position;
					u32 sz_zero		= (position+size) - source->dwBytesTotal;
					VERIFY			(size == (sz_data+sz_zero));
					fill_data		(dest,position,sz_data);
					Memory.mem_fill	(dest+sz_data,0,sz_zero);
					//Msg				("        playing: [%d]-normal,[%d]-zero",sz_data,sz_zero);
				}
				position			+= size;
			}
			break;
		case stPlayingLooped:
			{
				// Fill in two parts - looping :)
				u32		sz_first	= source->dwBytesTotal - position;
                if (0==sz_first)
                {
					fill_data			(dest,0,size);
                } else {
                    u32		sz_second	= (position+size) - source->dwBytesTotal;
                    VERIFY				(size == (sz_first+sz_second));
                    VERIFY				(position<source->dwBytesTotal);
					fill_data			(dest,			position,	sz_first);
					fill_data			(dest+sz_first,	0,			sz_second);
                }
				//Msg					("        looping: [%d]-first,[%d]-second",sz_first,sz_second);
				position			+= size;
				position			%= source->dwBytesTotal;
			}
			break;
		default:
			Debug.fatal	("SOUND: Invalid emitter state");
			break;
		}
	} else {
		// Everything OK, just stream
		//Msg				("        normal");
		fill_data			(dest,position,size);
		position			+= size;
	}
}

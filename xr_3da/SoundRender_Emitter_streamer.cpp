#include "stdafx.h"
#pragma hdrstop

#include "SoundRender_Source.h"
#include "SoundRender_Emitter.h"

void	CSoundRender_Emitter::fill_block	(void* ptr, u32 size)
{
	//Msg			("stream: %10s - [%X]:%d, p=%d, t=%d",source->fname,ptr,size,position,source->dwBytesTotal);
	LPBYTE		dest = LPBYTE(ptr);
	LPBYTE		wave = LPBYTE(source->wave);
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
					Memory.mem_copy	(dest,wave+position,sz_data);
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
                    Memory.mem_copy		(dest,wave,size);
                } else {
                    u32		sz_second	= (position+size) - source->dwBytesTotal;
                    VERIFY				(size == (sz_first+sz_second));
                    VERIFY				(position<source->dwBytesTotal);
                    Memory.mem_copy		(dest,wave+position,sz_first);
                    Memory.mem_copy		(dest+sz_first,wave,sz_second);
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
		Memory.mem_copy		(dest,wave+position,size);
		position			+= size;
	}
}

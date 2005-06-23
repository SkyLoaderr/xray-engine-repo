#ifndef xrTheora_StreamH
#define xrTheora_StreamH
#pragma once

#include <theora/theora.h>

class ctheora_stream{
	friend	class		ctheora_surface;

	ogg_sync_state		o_sync_state;
	ogg_page			o_page;
	ogg_stream_state	o_stream_state;
	theora_info			t_info;
	theora_comment		t_comment;
	theora_state		t_state;

	IReader*			source;

	yuv_buffer			t_yuv_buffer;

	ogg_int64_t			d_frame;
	u32					tm_total;
	u32					key_rate;			// theora have const key rate
	float				fpms;
protected:
	int					read_data			();
	bool				parse_headers		();

	void				invalidate			();
public:
						ctheora_stream		();
	virtual				~ctheora_stream		();

	bool				load				(IReader* reader);

	void				reset				();

	bool				decode				(u32 tm_play);

	yuv_buffer*			current_yuv_buffer	()					{return &t_yuv_buffer;}
};

#endif //xrTheora_StreamH

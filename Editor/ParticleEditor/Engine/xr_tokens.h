#ifndef __XR_TOKENS_H__
#define __XR_TOKENS_H__

struct ENGINE_API xr_token
{
	const char	*name;
	int 		id;
};

extern ENGINE_API xr_token					menu_style_token		[];
extern ENGINE_API xr_token					menu_type_token			[];
extern ENGINE_API xr_token					BOOL_token				[];
extern ENGINE_API xr_token					quality_token			[];
extern ENGINE_API xr_token					shade_mode_token		[];
extern ENGINE_API xr_token					texture_quality_token	[];
extern ENGINE_API xr_token					texture_mipmap_token	[];
extern ENGINE_API xr_token					light_type_token		[];
extern ENGINE_API xr_token					object_style_token		[];
extern ENGINE_API xr_token					collision_mode_token	[];
extern ENGINE_API xr_token					meshanim_type_token		[];
extern ENGINE_API xr_token					friction_type_token		[];
extern ENGINE_API xr_token					vid_mode_token			[];
extern ENGINE_API xr_token					vid_bpp_token			[];
extern ENGINE_API xr_token					snd_freq_token			[];
extern ENGINE_API xr_token					snd_model_token			[];
extern ENGINE_API xr_token					game_speed_token		[];
extern ENGINE_API xr_token					tex_format_token		[];
//extern ENGINE_API xr_token					tex_mipgen_token		[];
//extern ENGINE_API xr_token					tex_mipfilt_token		[];
extern ENGINE_API xr_token					light_model_token		[];
extern ENGINE_API xr_token					net_protocol_token		[];
extern ENGINE_API xr_token					axis_token				[];

extern ENGINE_API xr_token					shaders_token			[];

#endif //__XR_TOKENS_H__

#ifndef _LW_SHADERDEF_H_
#define _LW_SHADERDEF_H_
#pragma once

/* our instance data */
#pragma pack(push,1)
typedef struct st_XRShader{
	char	sh_name[64];
	int		sh_idx;
	char	desc[64];
} XRShader;
#pragma pack(pop)

#define SH_PLUGIN_NAME "!XRShader"

#endif
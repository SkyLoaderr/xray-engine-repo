#pragma once

#include "fixedvector.h"

const DWORD			sh_STAGE_MAX	= 4;
const DWORD			sh_PASS_MAX		= 8;

typedef char		sh_name			[64];

class ENGINE_API	CTexture;
class ENGINE_API	CXRShader;

typedef svector<sh_name,sh_STAGE_MAX*sh_PASS_MAX>	tex_names;
typedef svector<CTexture*,sh_STAGE_MAX>				tex_vector;
typedef svector<tex_vector,sh_PASS_MAX>				tex_handles;


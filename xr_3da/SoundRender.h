#pragma once

#include "sound.h"

class CSoundRender_Core;
class CSoundRender_Source;
class CSoundRender_Emitter;
class CSoundRender_EmitterParams;
class CSoundRender_Target;
class CSoundRender_Environment;

const	u32		sdef_target_block		= 100;					// ms
const	u32		sdef_target_size		= 3*sdef_target_block;	// ms
const	u32		sdef_env_version		= 1;					// current version of env-def
const	u32		sdef_level_version		= 1;					// current version of level-def
const	u32		sdef_event_pulse		= 500;					// ms

////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_sound_data.cpp
//	Created 	: 02.02.2005
//  Modified 	: 02.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker sound data
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_sound_data.h"
#include "sound_user_data_visitor.h"

CStalkerSoundData::~CStalkerSoundData	()
{
}

void CStalkerSoundData::accept			(CSoundUserDataVisitor *visitor)
{
	visitor->visit	(this);
}

#pragma once

#include "ai/script/ai_script_monster.h"

class CFryupZone : public CScriptMonster {
	typedef	CScriptMonster	inherited;

public:
					CFryupZone	();
	virtual			~CFryupZone	();

#ifdef DEBUG
	virtual void	OnRender				( );
#endif

};
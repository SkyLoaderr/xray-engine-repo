#ifndef _AI_GOALS_
#define _AI_GOALS_

namespace AI {
	enum EAIGoal		{
		g_DefendPosition,	// защита позиции
		g_Accompany,		// сопровождение
		g_Explore,			// исследование - идти в заданном направлении
		g_last
	};
	enum EAIGoal_Mod	{
		gm_Camper,			// ныкаться
		gm_Agressive,		// kill 'em all
		gm_last
	};
};

#endif
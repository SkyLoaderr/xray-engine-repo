#ifndef _AI_GOALS_
#define _AI_GOALS_

namespace AI {
	enum EAIGoal		{
		g_DefendPosition,	// ������ �������
		g_Accompany,		// �������������
		g_Explore,			// ������������ - ���� � �������� �����������
		g_last
	};
	enum EAIGoal_Mod	{
		gm_Camper,			// ��������
		gm_Agressive,		// kill 'em all
		gm_last
	};
};

#endif
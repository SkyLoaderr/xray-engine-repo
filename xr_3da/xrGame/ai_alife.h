////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.h
//	Created 	: 25.12.2002
//  Modified 	: 25.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#include "gameobject.h"

class CAI_ALife : public CGameObject {
private:
	u32				m_dwSchedulerMin;
	u32				m_dwSchedulerMax;
	//CGameObject
public:
//	typedef struct tagSALifeNPC {
//		
//	} SALifeNPC;

	CAI_ALife();
	~CAI_ALife();
};
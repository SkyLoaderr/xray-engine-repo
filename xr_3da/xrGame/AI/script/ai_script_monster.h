////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_monster.h
//	Created 	: 06.10.2003
//  Modified 	: 06.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Script behaviour interface for all the monsters
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\..\\GameObject.h"

class CEntityAction;

class CScriptMonster : virtual public CGameObject {
protected:
	typedef CGameObject inherited;
	xr_deque<CEntityAction*>	m_tpActionQueue;
	bool						m_bScriptControl;
	string64					m_caScriptName;
	int							m_iCurrentPatrolPoint;
	int							m_iPreviousPatrolPoint;
	CMotionDef					*m_tpScriptAnimation;
public:
								CScriptMonster			();
	virtual						~CScriptMonster			();
			void				Init					();
	virtual void				net_Destroy				();
			const Fmatrix		GetUpdatedMatrix		(LPCSTR caBoneName, const Fvector &tPositionOffset, const Fvector &tAngleOffset, BoneCallback fpBoneCallback = 0);
	virtual	void				SetScriptControl		(const bool			bScriptControl, LPCSTR	caSciptName);
	virtual	bool				GetScriptControl		() const;
	virtual	LPCSTR				GetScriptControlName	() const;
	virtual bool				CheckObjectVisibility	(const CObject		*tpObject);
	virtual bool				CheckIfCompleted		() const										{return false;};
	virtual	CEntityAction		*GetCurrentAction		();
	virtual	void				AddAction				(const CEntityAction*tpEntityAction);
	virtual void				UseObject				(const CObject		*tpObject);
	virtual void				ProcessScripts			();
	virtual	void				ResetScriptData			(void				*P = 0);
	virtual	bool				bfAssignMovement		(CEntityAction		*tpEntityAction);
	virtual	bool				bfAssignWatch			(CEntityAction		*tpEntityAction);
	virtual	bool				bfAssignAnimation		(CEntityAction		*tpEntityAction);
	virtual	bool				bfAssignSound			(CEntityAction		*tpEntityAction);
	virtual	bool				bfAssignParticles		(CEntityAction		*tpEntityAction);
	virtual	bool				bfAssignObject			(CEntityAction		*tpEntityAction);
	virtual	void				vfChoosePatrolPathPoint	(CEntityAction		*tpEntityAction);
};
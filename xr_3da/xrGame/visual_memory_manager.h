////////////////////////////////////////////////////////////////////////////
//	Module 		: visual_memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Visual memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../feel_vision.h"
#include "memory_space.h"

class CVisualMemory : public Feel::Vision {
protected:
	xr_vector<CObject*>				m_visible_objects;
	friend class CScriptMonster;
};

class CVisualMemoryManager : 
	virtual public CGameObject,
	virtual public CVisualMemory
{
public:
	struct SRemoveOfflivePredicate {
		bool		operator()						(const CVisibleObject &object) const
		{
			VERIFY	(object.m_object);
			return	(!!object.m_object->getDestroy());
		}
	};

private:
	u32								m_max_object_count;
	xr_vector<CVisibleObject>		*m_objects;
	// visibility constants
	u32								m_dwMovementIdleTime;
	float							m_fMaxInvisibleSpeed;
	float							m_fMaxViewableSpeed;
	float							m_fMovementSpeedWeight;
	float							m_fDistanceWeight;
	float							m_fSpeedWeight;
	float							m_fVisibilityThreshold;
	float							m_fLateralMultiplier;
	float							m_fShadowWeight;

protected:
	IC		void	fill_object						(CVisibleObject &visible_object, CGameObject *game_object);
			void	add_visible_object				(CObject *object);
			void	add_visible_object				(const CVisibleObject visible_object);
	virtual	void	update							();

public:
					CVisualMemoryManager			();
	virtual			~CVisualMemoryManager			();
			void	Init							();
	virtual	void	Load							(LPCSTR section);
	virtual	void	reinit							();
	virtual	void	reload							(LPCSTR section);
	IC		const xr_vector<CVisibleObject>	&memory_visible_objects() const;
	IC		const xr_vector<CObject*>		&visible_objects() const;
			bool	visible							(CGameObject *game_object) const;
			bool	visible							(u32 dwNodeID, float yaw) const;
	IC		void	set_squad_objects				(xr_vector<CVisibleObject> *squad_objects);
};

#include "visual_memory_manager_inline.h"
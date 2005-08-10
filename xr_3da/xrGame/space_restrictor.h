////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"

#define PRECOMPUTED_INSIDE

class CSpaceRestrictor : public CGameObject {
private:
	typedef CGameObject			inherited;

private:
	enum {
		PLANE_COUNT = 6,
	};

private:
	typedef Fplane				CPlanesArray[PLANE_COUNT];

private:
	struct CPlanes {
		CPlanesArray			m_planes;
	};

private:
	typedef xr_vector<Fsphere>	SPHERES;
	typedef xr_vector<CPlanes>	BOXES;

#ifdef PRECOMPUTED_INSIDE
private:
	mutable SPHERES	m_spheres;
	mutable BOXES	m_boxes;
	mutable Fsphere	m_selfbounds;
	mutable bool	m_actuality;
#endif

#ifdef PRECOMPUTED_INSIDE
private:
	IC		void	actual				(bool value) const;
			void	prepare				() const;
			bool	prepared_inside		(const Fvector &position, float radius) const;
#endif

public:
	IC				CSpaceRestrictor	();
	virtual			~CSpaceRestrictor	();
	virtual	BOOL	net_Spawn			(CSE_Abstract* data);
			bool	inside				(const Fvector &position, float radius = EPS_L) const;
	virtual void	Center				(Fvector &C) const;
	virtual float	Radius				() const;
	virtual BOOL	UsedAI_Locations	();
#ifdef PRECOMPUTED_INSIDE
	virtual void	spatial_move		();
	IC		bool	actual				() const;
#endif
#ifdef DEBUG
	virtual void	OnRender			();
#endif
	virtual	CSpaceRestrictor*			cast_restrictor	()	{return this;}
};

#include "space_restrictor_inline.h"
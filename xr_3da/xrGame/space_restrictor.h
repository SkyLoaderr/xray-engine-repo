////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"

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

private:
	mutable SPHERES	m_spheres;
	mutable BOXES	m_boxes;
	mutable Fsphere	m_selfbounds;
	mutable bool	m_actuality;

private:
	IC		void	actual				(bool value) const;
			void	prepare				() const;
			bool	prepared_inside		(const Fsphere &sphere) const;

public:
	IC				CSpaceRestrictor	();
	virtual			~CSpaceRestrictor	();
	virtual	BOOL	net_Spawn			(CSE_Abstract* data);
			bool	inside				(const Fsphere &sphere) const;
	virtual void	Center				(Fvector &C) const;
	virtual float	Radius				() const;
	virtual BOOL	UsedAI_Locations	();
	virtual void	spatial_move		();
	IC		bool	actual				() const;
	virtual	CSpaceRestrictor	*cast_restrictor	()	{return this;}

public:
#ifdef DEBUG
	virtual void	OnRender			();
#endif
};

#include "space_restrictor_inline.h"
#include "stdafx.h"
#pragma hdrstop

#include "ResourceManager.h"
#include "Render.h"

void	CResourceManager::reset_begin			()
{
	// destroy everything, renderer may use
	::Render->reset_begin		();

	// destroy state-blocks
	for (u32 _it=0; _it<v_states.size(); _it++)
		_RELEASE(v_states[_it]->state);

	// destroy RTs
	for (map_RTIt rt_it=m_rtargets.begin(); rt_it!=m_rtargets.end(); rt_it++)
		rt_it->second->reset_begin();
	for (map_RTCIt rtc_it=m_rtargets_c.begin(); rtc_it!=m_rtargets_c.end(); rtc_it++)
		rtc_it->second->reset_begin();

	// destroy DStreams
	RCache.old_QuadIB			= RCache.QuadIB;
	_RELEASE					(RCache.QuadIB);
	RCache.Index.reset_begin	();
	RCache.Vertex.reset_begin	();
}

bool	cmp_rt	(const CRT* A,const CRT* B)		{ return A->_order < B->_order; }
bool	cmp_rtc	(const CRTC* A,const CRTC* B)	{ return A->_order < B->_order; }

void	CResourceManager::reset_end				()
{
	// create RDStreams
	RCache.Vertex.reset_end		();
	RCache.Index.reset_end		();
	RCache.CreateQuadIB			();

	// remark geom's which point to dynamic VB/IB
	{
		for (u32 _it=0; _it<v_geoms.size(); _it++)
		{
			SGeometry*	_G = v_geoms[_it];
			if			(_G->vb == RCache.Vertex.old_pVB)	_G->vb = RCache.Vertex.Buffer	();
			if			(_G->ib == RCache.Index.old_pIB)	_G->ib = RCache.Index.Buffer	();
			if			(_G->ib == RCache.old_QuadIB)		_G->ib = RCache.QuadIB;
		}
	}

	// create RTs in the same order as them was first created
	{
		// RT
		xr_vector<CRT*>		rt;
		for (map_RTIt rt_it=m_rtargets.begin(); rt_it!=m_rtargets.end(); rt_it++)	rt.push_back(rt_it->second);
		std::sort(rt.begin(),rt.end(),cmp_rt);
		for (u32 _it=0; _it<rt.size(); _it++)	rt[_it]->reset_end	();
	}
	{
		// RTc
		xr_vector<CRTC*>	rt;
		for (map_RTCIt rt_it=m_rtargets_c.begin(); rt_it!=m_rtargets_c.end(); rt_it++)	rt.push_back(rt_it->second);
		std::sort(rt.begin(),rt.end(),cmp_rtc);
		for (u32 _it=0; _it<rt.size(); _it++)	rt[_it]->reset_end	();
	}

	// create state-blocks
	{
		for (u32 _it=0; _it<v_states.size(); _it++)
			v_states[_it]->state = v_states[_it]->state_code.record();
	}

	// create everything, renderer may use
	::Render->reset_end					();
}

template<class C>	void mdump(C c)
{
	if (0==c.size())	return;
	for (C::iterator I=c.begin(); I!=c.end(); I++)
		Msg	("*        : %3d: %s",I->second->dwReference, I->second->cName.c_str());
}

CResourceManager::~CResourceManager		()
{
	Msg		("* RM_Dump: textures : %d",m_textures.size());		mdump(m_textures);
	Msg		("* RM_Dump: rtargets : %d",m_rtargets.size());		mdump(m_rtargets);
	Msg		("* RM_Dump: rtargetsc: %d",m_rtargets_c.size());	mdump(m_rtargets_c);
	Msg		("* RM_Dump: vs       : %d",m_vs.size());			mdump(m_vs);
	Msg		("* RM_Dump: ps       : %d",m_ps.size());			mdump(m_ps);
	Msg		("* RM_Dump: dcl      : %d",v_declarations.size());
	Msg		("* RM_Dump: states   : %d",v_states.size());
}


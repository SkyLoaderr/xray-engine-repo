#include "stdafx.h"
#pragma hdrstop

#include "ResourceManager.h"

void	CResourceManager::reset_begin			()
{
	// destroy DStreams
	RCache.old_QuadIB			= RCache.QuadIB;
	_RELEASE					(RCache.QuadIB);
	RCache.Index.reset_begin	();
	RCache.Vertex.reset_begin	();

	// destroy state-blocks
	for (u32 _it=0; _it<v_states.size(); _it++)
		_RELEASE(v_states[_it]->state);

	// destroy RTs
	for (map_RTIt rt_it=m_rtargets.begin(); rt_it!=m_rtargets.end(); rt_it++)
		rt_it->second->reset_begin();
	for (map_RTCIt rtc_it=m_rtargets.begin(); rtc_it!=m_rtargets.end(); rtc_it++)
		rtc_it->second->reset_begin();

	// destroy everything, renderer may use
	::Render->reset_begin		();
}

bool	cmp_rt	(const CRT* A,const CRT* B)		{ return A->_order < B->_order; }
bool	cmp_rtc	(const CRTC* A,const CRTC* B)	{ return A->_order < B->_order; }

void	CResourceManager::reset_end				()
{
	// destroy everything, renderer may use
	::Render->reset_end			();

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
		for (map_RTCIt rt_it=m_rtargets.begin(); rt_it!=m_rtargets.end(); rt_it++)	rt.push_back(rt_it->second);
		std::sort(rt.begin(),rt.end(),cmp_rtc);
		for (u32 _it=0; _it<rt.size(); _it++)	rt[_it]->reset_end	();
	}

	// create state-blocks
	{
		for (u32 _it=0; _it<v_states.size(); _it++)
			v_states[_it]->state = v_states[_it]->state_code.record();
	}

	// remark geom's which point to dynamic VB/IB
	{
		for (u32 _it=0; _it<v_geoms.size(); _it++)
		{
			SGeometry*	_G = v_geoms[_it];
			if			(_G->vb == RCache.Vertex.old_pVB)	_G->vb = RCache.Vertex.Buffer	();
			if			(_G->ib == RCache.Vertex.old_pIB)	_G->ib = RCache.Index.Buffer	();
			if			(_G->ib == RCache.old_QuadIB)		_G->ib = RCache.QuadIB;
		}
	}

	// create RDStreams
	RCache.Vertex.reset_end		();
	RCache.Index.reset_end		();
	RCache.CreateQuadIB			();
}

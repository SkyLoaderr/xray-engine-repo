////////////////////////////////////////////////////////////////////////////
//	Module 		: xr_graph_merge.h
//	Created 	: 25.01.2003
//  Modified 	: 25.01.2003
//	Author		: Dmitriy Iassenev
//	Description : Merging level graphs for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#pragma once

struct CLevelInfo {
	u32				id;
	LPCSTR			name;
	Fvector			offset;

	CLevelInfo	(u32 _id, LPCSTR _name, const Fvector &_offset) :
	id(_id),
		name(_name),
		offset(_offset)
	{
	}

	IC	bool	operator< (const CLevelInfo &info) const
	{
		return		(id < info.id);
	}
};

void xrMergeGraphs(LPCSTR name);

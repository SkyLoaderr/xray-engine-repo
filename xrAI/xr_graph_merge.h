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
	LPSTR			name;
	Fvector			offset;
	ref_str			m_section;

	CLevelInfo	(u32 _id, LPCSTR _name, const Fvector &_offset, ref_str section) :
		id(_id),
		offset(_offset),
		m_section(section)
	{
		name		= strlwr(xr_strdup(_name));
	}

	CLevelInfo(const CLevelInfo &info)
	{
		id = info.id;
		name = xr_strdup(info.name);
		offset = info.offset;
	}

	IC	bool	operator< (const CLevelInfo &info) const
	{
		return		(id < info.id);
	}

	virtual ~CLevelInfo()
	{
		xr_free		(name);
	}
};

void xrMergeGraphs(LPCSTR name);

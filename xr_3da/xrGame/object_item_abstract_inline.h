////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_abstract_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item abstract class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CObjectItemAbstract::CObjectItemAbstract	(const CLASS_ID &clsid, LPCSTR script_clsid) :
	m_clsid				(clsid)
#ifndef NO_SCRIPTS
	,m_script_clsid		(script_clsid)
#endif
{
}

IC	const CLASS_ID &CObjectItemAbstract::clsid	() const
{
	return				(m_clsid);
}

#ifndef NO_SCRIPTS
IC	ref_str	CObjectItemAbstract::script_clsid	() const
{
	return				(m_script_clsid);
}
#endif

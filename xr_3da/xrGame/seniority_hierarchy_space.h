////////////////////////////////////////////////////////////////////////////
//	Module 		: seniority_hierarchy_space.h
//	Created 	: 12.11.2001
//  Modified 	: 03.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Seniority hierarchy space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace SeniorityHierarchy {
	IC	ref_str to_string (u32 number)
	{
		string16	S;
		sprintf		(S,"%d",number);
		return		(ref_str(S));
	}
}
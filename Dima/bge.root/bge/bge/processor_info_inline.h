////////////////////////////////////////////////////////////////////////////
//	Module 		: processor_info_inline.h
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Processor info class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CProcessorInfo::CProcessorInfo	()
{
	fill	();
}

IC	bool CProcessorInfo::detected	() const
{
	return	(m_detected);
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: processor_info_inline.h
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Processor info class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CProcessorInfo::CProcessorInfo		()
{
	fill	();
}

IC	LPCSTR CProcessorInfo::vendor_name	() const
{
	return	(m_vendor_name);
}

IC	LPCSTR CProcessorInfo::model_name	() const
{
	return	(m_model_name);
}

IC	int	 CProcessorInfo::family			() const
{
	return	(m_family);
}

IC	int	 CProcessorInfo::model			() const
{
	return	(m_model);
}

IC	int	 CProcessorInfo::stepping		() const
{
	return	(m_stepping);
}

IC	int	 CProcessorInfo::features		() const
{
	return	(m_feature);
}

IC	bool CProcessorInfo::detected		() const
{
	return	(m_detected);
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_inline.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CScriptEngine::add_script_processor	(LPCSTR processor_name, CScriptProcessor *script_processor)
{
	CScriptProcessorStorage::const_iterator	I = m_script_processors.find(processor_name);
	VERIFY									(I == m_script_processors.end());
	m_script_processors.insert				(std::make_pair(processor_name,script_processor));
}

CScriptProcessor *CScriptEngine::script_processor	(LPCSTR processor_name) const
{
	CScriptProcessorStorage::const_iterator	I = m_script_processors.find(processor_name);
	if ((I != m_script_processors.end()))
		return								((*I).second);
	return									(0);
}


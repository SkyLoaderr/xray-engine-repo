////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_inline.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CScriptEngine::add_script_processor		(LPCSTR processor_name, CScriptProcessor *script_processor)
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

IC	void CScriptEngine::set_current_thread			(CScriptStackTracker *new_thread)
{
#ifdef DEBUG
	VERIFY									((!m_current_thread && new_thread) || (m_current_thread && !new_thread));
#endif
	m_current_thread						= new_thread;
}

IC	CScriptStackTracker *CScriptEngine::current_thread			()
{
	return									(m_current_thread);
}

IC	CScriptStackTracker	&CScriptEngine::script_stack_tracker	()
{
	if (!current_thread())
		return								(*this);
	return									(*current_thread());
}

IC	void CScriptEngine::reload_modules		(bool flag)
{
	m_reload_modules						= flag;
}


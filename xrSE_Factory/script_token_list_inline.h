////////////////////////////////////////////////////////////////////////////
//	Module 		: script_token_list_inline.h
//	Created 	: 21.05.2004
//  Modified 	: 21.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Script token list class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CScriptTokenList::add		(LPCSTR name, int id)
{
	VERIFY					((token(name) == m_token_list.end()) && (token(id) == m_token_list.end()));
	xr_token				temp;
	temp.name				= name;
	temp.id					= id;
	m_token_list.push_back	(temp);
}

IC	void CScriptTokenList::remove	(LPCSTR name)
{
	iterator				I = token(name);
	VERIFY					(I != m_token_list.end());
	m_token_list.erase		(I);
}

IC	void CScriptTokenList::clear	()
{
	m_token_list.clear		();
}

IC	int	 CScriptTokenList::id		(LPCSTR name)
{
	iterator				I = token(name);
	VERIFY					(I != m_token_list.end());
	return					((*I).id);
}

IC	LPCSTR CScriptTokenList::name	(int id)
{
	iterator				I = token(id);
	VERIFY					(I != m_token_list.end());
	return					((*I).name);
}

IC	CScriptTokenList::iterator CScriptTokenList::token	(LPCSTR name)
{
	return					(std::find_if(m_token_list.begin(),m_token_list.end(),CTokenPredicate(name)));
}

IC	CScriptTokenList::iterator CScriptTokenList::token	(int id)
{
	return					(std::find_if(m_token_list.begin(),m_token_list.end(),CTokenPredicate(id)));
}

IC	const CScriptTokenList::TOKEN_LIST &CScriptTokenList::tokens() const
{
	return					(m_token_list);
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: script_token_list.h
//	Created 	: 21.05.2004
//  Modified 	: 21.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Script token list class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CScriptTokenList {
protected:
	typedef xr_vector<xr_token>			TOKEN_LIST;
	typedef TOKEN_LIST::iterator		iterator;
	typedef TOKEN_LIST::const_iterator	const_iterator;

protected:
	struct CTokenPredicate {
		bool			id;
		LPCSTR			m_name;
		int				m_id;

		IC				CTokenPredicate	(LPCSTR name)
		{
			m_name		= name;
			id			= false;
		}

		IC				CTokenPredicate	(int id)
		{
			m_id		= id;
			id			= true;
		}

		IC		bool	operator()		(const xr_token &token) const
		{
			return		(id ? (token.id == m_id) : (!xr_strcmp(token.name,m_name)));
		}
	};

protected:
	TOKEN_LIST					m_token_list;

protected:
	IC		iterator			token	(LPCSTR name);
	IC		iterator			token	(int id);

public:
	IC		void				add		(LPCSTR name, int id);
	IC		void				remove	(LPCSTR name);
	IC		void				clear	();
	IC		int					id		(LPCSTR name);
	IC		LPCSTR				name	(int id);
	IC		const TOKEN_LIST	&tokens	() const;
};

#include "script_token_list_inline.h"
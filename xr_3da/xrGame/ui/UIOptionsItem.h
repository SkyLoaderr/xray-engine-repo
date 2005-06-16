////////////////////////////////////////////////////
//
///////////////////////////////////////////////////

#pragma once

class CUIOptionsItem{
	friend class CUIOptionsManager;
public:
	virtual void	Register(const char* entry, const char* group);
protected:
	virtual void	SetDefaultValue()					= 0;
	virtual void	SaveValue()							= 0;

	xr_string		m_entry;
};

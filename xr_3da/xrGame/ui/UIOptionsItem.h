////////////////////////////////////////////////////
//
///////////////////////////////////////////////////

#pragma once

class CUIOptionsItem{
	friend class CUIOptionsManager;
public:
	virtual void Register(const char* entry, const char* group);
protected:
	virtual void	SetCurrentValue()	=0;
	virtual void	SaveValue()			=0;

			// string
			LPCSTR		GetOptStringValue();
			void		SaveOptStringValue(const char* val);
			// integer
			void		GetOptIntegerValue(int& val, int& min, int& max);
			void		SaveOptIntegerValue(int val);
			// bool
			bool		GetOptBoolValue();
			void		SaveOptBoolValue(bool val);
			// token
			char*		GetOptTokenValue();
			xr_token*	GetOptToken();
			void		SaveOptTokenValue(const char* val);
private:
	xr_string		m_entry;
};

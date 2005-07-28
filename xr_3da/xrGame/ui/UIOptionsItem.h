////////////////////////////////////////////////////
//
///////////////////////////////////////////////////

#pragma once

class CUIOptionsItem{
	friend class CUIOptionsManager;
public:
	virtual ~CUIOptionsItem();
	virtual void Register(const char* entry, const char* group);
protected:
	virtual void	SetCurrentValue()	=0;
	virtual void	SaveValue()			=0;
			
			void		SendMessage2Group(const char* group, const char* message);
	virtual	void		OnMessage(const char* group, const char* message);

			// key binding
			void		BindAction2Key(const char* val);

			// string
			LPCSTR		GetOptStringValue();
			void		SaveOptStringValue(const char* val);
			// integer
			void		GetOptIntegerValue(int& val, int& min, int& max);
			void		SaveOptIntegerValue(int val);
			// float
			void		GetOptFloatValue(float& val, float& min, float& max);
			void		SaveOptFloatValue(float val);
			// bool
			bool		GetOptBoolValue();
			void		SaveOptBoolValue(bool val);
			// token
			char*		GetOptTokenValue();
			xr_token*	GetOptToken();
			void		SaveOptTokenValue(const char* val);

	xr_string		m_entry;
};

// XR_IOConsole.h: interface for the CConsole class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XR_IOCONSOLE_H__ADEEFD61_7731_11D3_83D8_00C02610C34E__INCLUDED_)
#define AFX_XR_IOCONSOLE_H__ADEEFD61_7731_11D3_83D8_00C02610C34E__INCLUDED_
#pragma once

#include "fcontroller.h"

//refs
class ENGINE_API CFontSmall;
class ENGINE_API CConsoleCommand;

class ENGINE_API CConsole  :
	public CController,
	public pureDeviceDestroy,
	public pureRender,
	public pureFrame
{
public:
	//t-defs
	struct str_pred : public binary_function<char*, char*, bool> 
	{	
		IC bool operator()(const char* x, const char* y) const
		{	return strcmp(x,y)<0;	}
	};
	typedef map<LPSTR,CConsoleCommand*,str_pred>	vecCMD;
	typedef vecCMD::iterator						vecCMD_IT;

private:
	float			cur_time;
	float			rep_time;
	float			fAccel;
	int				scroll_delta;
	int				cmd_delta;
	int				old_cmd_delta;
	char			editor [256];
	char			*editor_last;
	BOOL			bShift;
	BOOL			bCursor;
	BOOL			bRepeat;
	BOOL			RecordCommands;

	CFontSmall		*pFont;
public:
	BOOL			bVisible;
	vecCMD			Commands;

	void	AddCommand	(CConsoleCommand*);

	void Show			();
	void Hide			();

	void Save			();
	void Execute		(char *cmd);
	void ExecuteScript	(char *name);
	void ExecuteCommand	();

	// get
	BOOL	GetBool		(char *cmd);
	FLOAT	GetFloat	(char *cmd);
	char *	GetValue	(char *cmd);
	char *	GetNextValue(char *cmd);
	char *	GetPrevValue(char *cmd);

	void SelectCommand	();

	// keyboard
	void OnPressKey		(int dik, BOOL bHold=false);
	virtual void	OnKeyboardPress		(int dik);
	virtual void	OnKeyboardHold		(int dik);
	virtual void	OnKeyboardRelease	(int dik);

	// render & onmove
	virtual void	OnRender			(void);
	virtual void	OnFrame				(void);

	// device dependent
	virtual void	OnDeviceDestroy		(void);

	void	Initialize	();
	void	Destroy		();
};

ENGINE_API extern CConsole Console;

#endif // !defined(AFX_XR_IOCONSOLE_H__ADEEFD61_7731_11D3_83D8_00C02610C34E__INCLUDED_)

#pragma once

class ENGINE_API CThread
{
	static void __cdecl startup(void* P);
public:
	DWORD		ID;
	float		fProgress;
	BOOL		bCompleted;

	CThread		(DWORD _ID)	
	{
		ID			= _ID;
		fProgress	= 0;
		bCompleted	= FALSE;
		
		_beginthread(startup,0,this);
	}

	virtual		void	Execute()	= 0;
};

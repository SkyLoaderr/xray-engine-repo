#pragma once

class ENGINE_API CThread
{
	static void __cdecl startup(void* P);
public:
	DWORD		thID;
	float		thProgress;
	BOOL		thCompleted;

	CThread				(DWORD _ID)	
	{
		thID			= _ID;
		thProgress		= 0;
		thCompleted		= FALSE;
	}
	void				Start	()
	{
		_beginthread(startup,0,this);
	}
	virtual		void	Execute	()	= 0;
};

//----------------------------------------------------
// file: UI_SceneOptions.h
//----------------------------------------------------

#ifndef _INCDEF_SceneOptions_H_
#define _INCDEF_SceneOptions_H_

#include "FileSystem.h"
#include "Scene.h"
//----------------------------------------------------

class UI_SceneOptions{
protected:

	friend BOOL CALLBACK MainSceneOptionsDialogProc(
		HWND hw, UINT msg, WPARAM wp, LPARAM lp);
	friend BOOL CALLBACK BuildSceneOptionsDialogProc(
		HWND hw, UINT msg, WPARAM wp, LPARAM lp);
	friend BOOL CALLBACK PathsSceneOptionsDialogProc(
		HWND hw, UINT msg, WPARAM wp, LPARAM lp);
	friend DWORD WINAPI SceneOptionsThread( LPVOID );

	HINSTANCE m_Instance;
	HWND m_ParentWindow;

	HANDLE m_Thread;
	DWORD m_ThreadID;

public:

	UI_SceneOptions();
	~UI_SceneOptions();

	bool Open( HINSTANCE instance, HWND parent );
};

extern UI_SceneOptions OpDialog;

//----------------------------------------------------
#endif /*_INCDEF_SceneOptions_H_*/


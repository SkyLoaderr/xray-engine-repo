//----------------------------------------------------
// file: SceneClassEditor.h
//----------------------------------------------------

#ifndef _INCDEF_XRayClassEditor_H_
#define _INCDEF_XRayClassEditor_H_

#include "Scene.h"

//----------------------------------------------------


class SceneClassEditor {
protected:

	friend DWORD CALLBACK ClassEditorThread( LPVOID param );
	friend BOOL CALLBACK ClassEditorDialogProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp );

	int m_ResID;
	int m_ClassID;
	ObjectList m_Objects;

	HINSTANCE m_Instance;
	HWND m_ParentWindow;
	HWND m_DialogWindow;

	HANDLE m_Thread;
	DWORD m_ThreadID;

protected:

	void StopThread();
	void MakeThread();

	virtual void DlgInit( HWND hw ){};
	virtual void Command( WPARAM wp, LPARAM lp ){};
	virtual void Notify( WPARAM wp, LPARAM lp ){};
	virtual void DrawItem( WPARAM wp, LPARAM lp ){};

	virtual void GetObjectsInfo(){};
	virtual void ApplyObjectsInfo(){};
	virtual void DlgExtract(){};
	virtual void DlgSet(){};

public:

	SceneClassEditor( int recsourceid ){
		m_ResID = recsourceid;
		m_ClassID = 0;
		m_Instance = 0;
		m_ParentWindow = 0;
		m_DialogWindow = 0;
		m_Thread = 0;
		m_ThreadID = 0; }

	virtual ~SceneClassEditor(){
		_ASSERTE( m_DialogWindow == 0 );
		_ASSERTE( m_Objects.size() == 0 ); }

	bool Create( HINSTANCE instance, HWND parentwindow, ObjectList& objectset );
	bool CancelEditor( );
};




//----------------------------------------------------
#endif /*_INCDEF_XRayClassEditor_H_*/


//----------------------------------------------------
// file: MultiObjCheck.h
//----------------------------------------------------

#ifndef _INCDEF_MultiObjCheck_H_
#define _INCDEF_MultiObjCheck_H_
//----------------------------------------------------


#include "CustomControls.h"


class MultiObjCheck {
protected:

	enum {
		MULTIOBJ_SET, 
		MULTIOBJ_CLEAR,
		MULTIOBJ_LEAVE
	} m_CurrentState;

public:

	void DlgSet( HWND hw, int controlid ){
		UINT check = BST_INDETERMINATE;
		if( m_CurrentState==MULTIOBJ_SET ) check = BST_CHECKED;
		if( m_CurrentState==MULTIOBJ_CLEAR ) check = BST_UNCHECKED;
		SetCheck( hw, controlid, check );
	}

	void DlgGet( HWND hw, int controlid ){
		UINT check = IsChecked( hw, controlid );
		m_CurrentState = MULTIOBJ_LEAVE;
		if( check == BST_CHECKED ) m_CurrentState=MULTIOBJ_SET;
		if( check == BST_UNCHECKED ) m_CurrentState=MULTIOBJ_CLEAR;
	}

	void ObjFirsInit( int value ) {
		m_CurrentState = value ? MULTIOBJ_SET : MULTIOBJ_CLEAR;
	};

	void ObjNextInit( int value ) {
		if( m_CurrentState==MULTIOBJ_SET && !value ) m_CurrentState = MULTIOBJ_LEAVE;
		if( m_CurrentState==MULTIOBJ_CLEAR && value ) m_CurrentState = MULTIOBJ_LEAVE;
	};

	void ObjApply( int& _to ){
		if( m_CurrentState==MULTIOBJ_SET ) _to = 1;
		if( m_CurrentState==MULTIOBJ_CLEAR ) _to = 0;
	};
};


class MultiObjFloat {
protected:

	float m_BeforeDialog;
	float m_AfterDialog;
	bool m_Diffs;
	bool m_Changed;
	
public:

	void DlgSet( HWND hw, int controlid ){
		if( m_Diffs ) {
			SetDlgItemText( hw, controlid, "" );
		} else {
			char s[32];
			sprintf( s, "%.3f", m_BeforeDialog );
			SetDlgItemText( hw, controlid, s );
		}
	};

	void DlgGet( HWND hw, int controlid ){
		char s[32];
		GetDlgItemText( hw, controlid, s, 32 );
		m_Changed = ( s[0] != 0 ) ;
		m_AfterDialog = (float)atof( s );
	}

	void ObjFirsInit( float value ) {
		m_Changed = false;
		m_Diffs = false;
		m_BeforeDialog = value;
		m_AfterDialog = value;
	}

	void ObjNextInit( float value ) {
		if( !m_Diffs )
			if( m_BeforeDialog != value )
				m_Diffs = true;
	};

	void ObjApply( float& _to ){
		if( m_Changed )
			_to = m_AfterDialog;
	};
};



class MultiObjInt {
protected:

	int m_BeforeDialog;
	int m_AfterDialog;
	bool m_Diffs;
	bool m_Changed;
	
public:

	void DlgSet( HWND hw, int controlid ){
		if( m_Diffs ) {
			SetDlgItemText( hw, controlid, "" );
		} else {
			char s[32];
			sprintf( s, "%d", m_BeforeDialog );
			SetDlgItemText( hw, controlid, s );
		}
	};

	void DlgGet( HWND hw, int controlid ){
		char s[32];
		GetDlgItemText( hw, controlid, s, 32 );
		m_Changed = ( s[0] == 0 ) ;
		m_AfterDialog = (int)atoi( s );
	}

	void ObjFirsInit( int value ) {
		m_Changed = false;
		m_Diffs = false;
		m_BeforeDialog = value;
		m_AfterDialog = value;
	}

	void ObjNextInit( int value ) {
		if( !m_Diffs )
			if( m_BeforeDialog != value )
				m_Diffs = true;
	};

	void ObjApply( int& _to ){
		if( m_Changed )
			_to = m_AfterDialog;
	};
};


class MultiObjColor {
protected:

	IFColor m_BeforeDialog;
	IFColor m_AfterDialog;
	bool m_Diffs;
	bool m_Changed;
	
public:

	void ObjFirsInit( IFColor& value ) {
		m_Changed = false;
		m_Diffs = false;
		m_BeforeDialog = value;
		m_AfterDialog = value;
	}

	void ObjNextInit( IFColor& value ) {
		if( !m_Diffs )
			if( m_BeforeDialog.r != value.r ||
				m_BeforeDialog.g != value.g ||
				m_BeforeDialog.b != value.b ||
				m_BeforeDialog.a != value.a )
					m_Diffs = true;
	};

	void ObjApply( IFColor& _to ){
		if( m_Changed )
			_to = m_AfterDialog;
	};

	COLORREF Get(){
		int r = (int)(m_AfterDialog.r * 255.f);
		int g = (int)(m_AfterDialog.g * 255.f);
		int b = (int)(m_AfterDialog.b * 255.f);
		return RGB(r,g,b); }

	void Set( COLORREF value ){
		m_AfterDialog.r = GetRValue(value) / 255.f;
		m_AfterDialog.g = GetGValue(value) / 255.f;
		m_AfterDialog.b = GetBValue(value) / 255.f;
		m_Changed = true;
		m_Diffs = false; }

	bool diffs(){
		return m_Diffs; };
};



//----------------------------------------------------
#endif /*_INCDEF_MultiObjCheck_H_*/



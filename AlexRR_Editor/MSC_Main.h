//----------------------------------------------------
// file: MSC_Main.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_Main_H_
#define _INCDEF_MSC_Main_H_

//----------------------------------------------------

class MouseCallback;
typedef list<MouseCallback*> MSCList;
typedef MSCList::iterator MSCListIt;

class MouseCallback{
public:
	
	// non-hidden ops
	POINT m_StartCp;
	POINT m_CurrentCp;

	IVector m_StartRStart;
	IVector m_StartRNorm;

	IVector m_CurrentRStart;
	IVector m_CurrentRNorm;

	// hidden ops
	POINT m_CenterCpH;
	POINT m_StartCpH;
	POINT m_DeltaCpH;

	bool m_Alternate;

public:
	
	int m_Action;
	int m_Target;

public:

	MouseCallback( int _Target, int _Action );
	virtual ~MouseCallback();

	virtual bool Start() = 0;
	virtual bool End() = 0;
	virtual void Move() = 0;

	virtual bool HiddenMode(){
		return false; }

	virtual bool Compare( int _Target, int _Action ){
		return (m_Target==_Target && m_Action==_Action); }
};


extern float g_MouseMoveSpeed;
extern float g_MouseRotateSpeed;
extern float g_MouseScaleSpeed;


//----------------------------------------------------

#endif /*_INCDEF_MSC_Main_H_*/




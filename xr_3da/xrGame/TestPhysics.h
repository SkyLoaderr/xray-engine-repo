// TestPhysics.h: interface for the CTestPhysics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTPHYSICS_H__BEF96B41_153F_4D15_A59A_CA0433A56670__INCLUDED_)
#define AFX_TESTPHYSICS_H__BEF96B41_153F_4D15_A59A_CA0433A56670__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTestPhysics : 
	public pureRender, 
	public pureFrame,
	public CEventBase
{
public:
	virtual void OnFrame	();
	virtual void OnRender	();
	virtual void OnEvent	(EVENT E, DWORD P1, DWORD P2);

	CTestPhysics();
	virtual ~CTestPhysics();
};

#endif // !defined(AFX_TESTPHYSICS_H__BEF96B41_153F_4D15_A59A_CA0433A56670__INCLUDED_)

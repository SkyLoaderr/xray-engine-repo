// CController.h: interface for the CController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FCONTROLLER_H__A898DA00_FB63_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FCONTROLLER_H__A898DA00_FB63_11D3_B4E3_4854E82A090D__INCLUDED_

#pragma once

class ENGINE_API CController
{
public:

	void	iGetMousePosReal			(Ipoint &p)
	{
		GetCursorPos(p.d3d());
		if (Device.m_hWnd) ScreenToClient(Device.m_hWnd, p.d3d());
	}
	void	iGetMousePosIndependent		(Fpoint &f)
	{
		Ipoint p;
		iGetMousePosReal(p);
		f.set(
			2.f*float(p.x)/float(Device.dwWidth)-1.f,
			2.f*float(p.y)/float(Device.dwHeight)-1.f
		);
	}
	void	iGetMousePosIndependentCrop	(Fpoint &f)
	{
		iGetMousePosIndependent(f);
		if (f.x<-1.f) f.x=-1.f;
		if (f.x> 1.f) f.x= 1.f;
		if (f.y<-1.f) f.y=-1.f;
		if (f.y> 1.f) f.y= 1.f;
	}
	BOOL	iGetKeyState				(int dik);
	BOOL	iGetBtnState				(int btn);
	void	iCapture					(void);
	void	iRelease					(void);

	virtual void OnInputDeactivate		(void);
	virtual void OnInputActivate		(void)			{};

	virtual void OnMousePress			(int btn)		{};
	virtual void OnMouseRelease			(int btn)		{};
	virtual void OnMouseHold			(int btn)		{};
	virtual void OnMouseMove			(int x, int y)	{};
	virtual void OnMouseStop			(int x, int y)	{};

	virtual void OnKeyboardPress		(int dik)		{};
	virtual void OnKeyboardRelease		(int dik)		{};
	virtual void OnKeyboardHold			(int dik)		{};
};

#endif // !defined(AFX_FCONTROLLER_H__A898DA00_FB63_11D3_B4E3_4854E82A090D__INCLUDED_)

// XR_Cursor.h: interface for the CCursor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XR_CURSOR_H__FD545160_C840_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_XR_CURSOR_H__FD545160_C840_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

class CCursor
{
	Shader*			hShader;
	CVertexStream*	Stream;	
public:
	void			Render	();

	CCursor();
	~CCursor();
};

#endif // !defined(AFX_XR_CURSOR_H__FD545160_C840_11D3_B4E3_4854E82A090D__INCLUDED_)

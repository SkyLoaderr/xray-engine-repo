#ifndef __XR_UICURSOR_H__
#define __XR_UICURSOR_H__
#pragma once

class CUICursor
{
	Shader*			hShader;
	CVertexStream*	Stream;	
public:
	Fvector2		vPos;
public:
					CUICursor	();
					~CUICursor	();
	void			Render		();
	const Fvector2&	GetPos		(){return vPos;}
};

#endif //__XR_UICURSOR_H__

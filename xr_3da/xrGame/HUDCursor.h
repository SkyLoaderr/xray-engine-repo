#ifndef __XR_HUDCURSOR_H__
#define __XR_HUDCURSOR_H__
#pragma once

class CHUDCursor
{
	Shader*			hShader;
	CVertexStream*	Stream;	
public:
					CHUDCursor	();
					~CHUDCursor	();
	void			Render		();
};

#endif //__XR_HUDCURSOR_H__

// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SHADER__INCLUDED_)
#define SHADER__INCLUDED_
#pragma once

class ENGINE_API CShader
{
protected:
	struct RFlags
	{
		DWORD	iPriority	:	2;
		DWORD	bStrictB2F	:	1;
		DWORD	bLighting	:	1;
	};
protected:
	virtual void				internal_Activate	()				= 0;
	virtual void				internal_Apply		(DWORD dwPass)	= 0;
public:
	DWORD						dwFrame;
	RFlags						Flags;		// important flags

	IC void						Activate	()
	{
		if (Device.dwFrame != dwFrame) {
			dwFrame = Device.dwFrame;
			internal_Activate	();
		}
	}
	IC void						Apply		(DWORD dwPass)
	{
		internal_Apply	(dwPass);
	}

	CShader	();
	virtual ~CShader();
};

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)

// Engine.h: interface for the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENGINE_H__22802DD7_D7EB_4234_9781_E237657471AC__INCLUDED_)
#define AFX_ENGINE_H__22802DD7_D7EB_4234_9781_E237657471AC__INCLUDED_
#pragma once

#include "FS.h"
#include "FileSystem.h"
                      
class ENGINE_API CEngine
{
public:
	CFileSystem			FS;
public:
						CEngine		();
						~CEngine	();

	void				Initialize	();
	void				Destroy		();
};

ENGINE_API extern CEngine			Engine;

#endif // !defined(AFX_ENGINE_H__22802DD7_D7EB_4234_9781_E237657471AC__INCLUDED_)

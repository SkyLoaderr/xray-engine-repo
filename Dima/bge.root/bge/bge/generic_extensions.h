////////////////////////////////////////////////////////////////////////////
//	Module 		: generic_extensions.h
//	Created 	: 03.12.2004
//  Modified 	: 03.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Generic extensions
////////////////////////////////////////////////////////////////////////////

#pragma once

template<typename T>
IC	void c_delete	(T *&ptr)
{
	if (ptr) {
		delete		ptr;
		ptr			= 0;
	}
}

template<typename T>
IC	void c_free		(T *&ptr)
{
	if (ptr) {
		free		(ptr);
		ptr			= 0;
	}
}

IC	LPSTR c_strdup	(LPCSTR str)
{
	if (!str)
		return		(0);

	LPSTR			temp = (LPSTR)malloc(strlen(str)*sizeof(char));
	strcpy			(temp,str);
	return			(temp);
}

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

////////////////////////////////////////////////////////////////////////////
//	Module 		: object_destroyer.h
//	Created 	: 21.01.2003
//  Modified 	: 09.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object destroyer
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_OBJECT_DESTROYER
#define XRAY_OBJECT_DESTROYER

template <class T1, class T2, class T3>
IC	void delete_data(xr_map<T1,T2,T3> &tpMap);

template <class T1, class T2>
IC	void delete_data(xr_set<T1,T2> &tpSet);

template <class T> 
IC	void delete_data(xr_vector<T> &tpVector);

template <class T> 
IC	void delete_data(T *&tpData);

template <class T> 
IC	void delete_data(T &tData);

IC	void delete_data(LPSTR &tpData);

#include "object_destroyer_inline.h"

#endif // XRAY_OBJECT_DESTROYER
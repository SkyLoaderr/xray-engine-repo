////////////////////////////////////////////////////////////////////////////
//	Module 		: object_loader.h
//	Created 	: 21.01.2003
//  Modified 	: 09.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object loader
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_OBJECT_LOADER
#define XRAY_OBJECT_LOADER

template <class T1, class T2, class T3, class M, class P>
IC	void load_data(xr_map<T1,T2,T3> &tpMap, M &tStream, const P &predicate);

template <class T1, class T2, class T3, class M>
IC	void load_data(xr_map<T1,T2,T3> &tpMap, M &tStream, const T1 tfGetKey(const T2));

template <class T1, class T2, class T3, class M>
IC	void load_data(xr_map<T1,T2*,T3> &tpMap, M &tStream, const T1 tfGetKey(const T2*));

template <class T1, class T2, class T3, class M>
IC	void load_data(xr_map<T1,T2,T3> &tpMap, M &tStream, bool bSaveCount = true);

template <class T1, class T2, class M>
IC	void load_data(xr_set<T1,T2> &tpSet, M &tStream, bool bSaveCount = true);

template <class T, class M>
IC	void load_data(xr_vector<T> &tpVector, M &tStream, bool bSaveCount = true);

template <class M>
IC	void load_data(xr_vector<bool> &baVector, M &tStream, bool bSaveCount = true);

template <class M>
IC	void load_data(LPSTR &tpData, M &tStream, bool bSaveCount = true);

template <class T, class M>
IC	void load_data(T &tData, M &tStream, bool bSaveCount = true);

template <class T, class M>
IC	void load_data(T *&tpData, M &tStream, bool bSaveCount = true);

template <class T, class M>
IC	void load_data(T **&tpData, M &tStream, bool bSaveCount = true);

template <class T1, class T2, class T3, class M>
IC	void load_data(xr_map<T1,T2,T3> *&tpMap, M &tStream, const T1 tfGetKey(const T2));

template <class T1, class T2, class T3, class M, class P>
IC	void load_data(xr_map<T1,T2,T3> *&tpMap, M &tStream, const P &predicate);

template <class T1, class T2, class T3, class M>
IC	void load_data(xr_map<T1,T2,T3> *&tpMap, M &tStream, bool bSaveCount = true);

template <class T, class M>
IC	void load_data(xr_vector<T> *&tpVector, M &tStream, bool bSaveCount = true);

#include "object_loader_inline.h"

#endif // XRAY_OBJECT_LOADER
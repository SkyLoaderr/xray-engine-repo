////////////////////////////////////////////////////////////////////////////
//	Module 		: object_saver.h
//	Created 	: 21.01.2003
//  Modified 	: 09.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object saver
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_OBJECT_SAVER
#define XRAY_OBJECT_SAVER

template <class T1, class T2, class T3, class M>
IC	void save_data(xr_map<T1,T2,T3> &tpMap, M &tStream, bool bSaveCount = true);

template <class T1, class T2, class M>
IC	void save_data(xr_set<T1,T2> &tpSet, M &tStream, bool bSaveCount = true);

template <class T, class M>
IC	void save_data(xr_vector<T> &tpVector, M &tStream, bool bSaveCount = true);

template <class M>
IC	void save_data(xr_vector<bool> &baVector, M &tStream, bool bSaveCount = true);

template <class M>
IC	void save_data(IPureServerObject &tData, M &tStream,bool bSaveCount = true);

template <typename T, class M>
IC	void save_data(T &tData, M &tStream,bool bSaveCount = true);

template <class M>
IC	void save_data(LPSTR &tData, M &tStream,bool bSaveCount = true);

template <class T, class M>
IC	void save_data(T *&tpData, M &tStream, bool bSaveCount = true);

template <class T, class M>
IC	void save_data(T **&tpData, M &tStream, bool bSaveCount = true);

template <class T1, class T2, class T3, class M>
IC	void save_data(xr_map<T1,T2,T3> *&tpMap, M &tStream, bool bSaveCount = true);

template <class T1, class T2, class M>
IC	void save_data(xr_set<T1,T2> *&tpSet, M &tStream, bool bSaveCount = true);

template <class T, class M>
IC	void save_data(xr_vector<T> *&tpVector, M &tStream, bool bSaveCount = true);

#include "object_saver_inline.h"

#endif // XRAY_OBJECT_SAVER
////////////////////////////////////////////////////////////////////////////
//	Module 		: object_broker.h
//	Created 	: 21.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object broker
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_OBJECT_BROKER
#define XRAY_OBJECT_BROKER

#include "alife_interfaces.h"

extern void delete_data(LPSTR &tpVector);
extern void delete_data(LPCSTR &tpVector);

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

#include "object_broker_inline.h"

#endif //XRAY_OBJECT_BROKER
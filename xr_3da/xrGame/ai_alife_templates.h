////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_templates.h
//	Created 	: 21.01.2003
//  Modified 	: 21.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life templates
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_ALIFE_TEMPLATES
#define XRAY_AI_ALIFE_TEMPLATES

// delete data
extern void delete_data(LPSTR &tpVector);
extern void delete_data(LPCSTR &tpVector);

template <class T1, class T2, class T3>
void delete_data(xr_map<T1,T2,T3> &tpMap)
{
	xr_map<T1,T2,T3>::iterator	I = tpMap.begin();
	xr_map<T1,T2,T3>::iterator	E = tpMap.end();
	for ( ; I != E; I++) {
		delete_data				((*I).first);
		delete_data				((*I).second);
	}
	tpMap.clear					();
};

template <class T> 
void delete_data(xr_vector<T> &tpVector)
{
	xr_vector<T>::iterator		I = tpVector.begin();
	xr_vector<T>::iterator		E = tpVector.end();
	for ( ; I != E; I++)
		delete_data				(*I);
	tpVector.clear				();
};

template <class T> 
void delete_data(T *&tpData)
{
	xr_delete					(tpData);
};

template <class T> 
void delete_data(T &tData)
{
};

// save data

template <class T1, class T2, class T3, class M>
void save_data(xr_map<T1,T2,T3> &tpMap, M &tNetPacket, bool bSaveCount = true)
{
	if (bSaveCount)
		tNetPacket.w_u32		((u32)tpMap.size());
	xr_map<T1,T2,T3>::iterator	I = tpMap.begin();
	xr_map<T1,T2,T3>::iterator	E = tpMap.end();
	for ( ; I != E; I++)
		save_data				((*I).second,tNetPacket,bSaveCount);
};

template <class T, class M>
void save_data(xr_vector<T> &tpVector, M &tNetPacket, bool bSaveCount = true)
{
	if (bSaveCount)
		tNetPacket.w_u32		((u32)tpVector.size());
	xr_vector<T>::iterator		I = tpVector.begin();
	xr_vector<T>::iterator		E = tpVector.end();
	for ( ; I != E; I++)
		save_data				(*I,tNetPacket,bSaveCount);
};

template <class M>
void save_data(xr_vector<bool> &baVector, M &tNetPacket, bool bSaveCount = true)
{
	if (bSaveCount)
		tNetPacket.w_u32		((u32)baVector.size());
	xr_vector<bool>::iterator 	I = baVector.begin();
	xr_vector<bool>::iterator 	E = baVector.end();
	u32							dwMask = 0;
	if (I != E) {
		for (int j=0; I != E; I++, j++) {
			if (j >= 32) {
				tNetPacket.w_u32(dwMask);
				dwMask			= 0;
				j				= 0;
			}
			if (*I)
				dwMask			|= u32(1) << j;
		}
		tNetPacket.w_u32		(dwMask);
	}
};

template <class M>
void save_data(LPSTR &tData, M &tNetPacket,bool bSaveCount = true)
{
	tNetPacket.w_string			(tData);
}

template <class T, class M>
void save_data(T &tData, M &tNetPacket,bool bSaveCount = true)
{
	tNetPacket.w				(&tData,sizeof(tData));
}

template <class T, class M>
void save_data(T *&tpData, M &tNetPacket, bool bSaveCount = true)
{
	tpData->Save				(tNetPacket);
};

// load data

template <class T1, class T2, class T3, class M>
void load_data(xr_map<T1,T2,T3> &tpMap, M &tNetPacket, T1 tfGetKey(const T2))
{
	tpMap.clear					();
	u32							dwCount	= tNetPacket.r_u32();
	for (int i=0 ; i<(int)dwCount; i++) {
		T2						T;
		load_data				(T,tNetPacket);
		tpMap.insert			(mk_pair(tfGetKey(T),T));
	}
};

template <class T1, class T2, class T3, class M>
void load_data(xr_map<T1,T2,T3> &tpMap, M &tNetPacket, bool bSaveCount = true)
{
	xr_map<T1,T2,T3>::iterator	I = tpMap.begin();
	xr_map<T1,T2,T3>::iterator	E = tpMap.end();
	for ( ; I != E; I++)
		load_data				((*I).second,tNetPacket,bSaveCount);
};

template <class T, class M>
void load_data(xr_vector<T> &tpVector, M &tNetPacket, bool bSaveCount = true)
{
	if (bSaveCount)
		tpVector.resize			(tNetPacket.r_u32());
	xr_vector<T>::iterator		I = tpVector.begin();
	xr_vector<T>::iterator		E = tpVector.end();
	for ( ; I != E; I++)
		load_data				(*I,tNetPacket,bSaveCount);
};

template <class M>
void load_data(xr_vector<bool> &baVector, M &tNetPacket, bool bSaveCount = true)
{
	if (bSaveCount)
		baVector.resize			(tNetPacket.r_u32());
	xr_vector<bool>::iterator 	I = baVector.begin();
	xr_vector<bool>::iterator 	E = baVector.end();
	u32							dwMask = 0;
	for (int j=32; I != E; I++, j++) {
		if (j >= 32) {
			dwMask				= tNetPacket.r_u32();
			j					= 0;
		}
		*I						= !!(dwMask & (u32(1) << j));
	}
};

template <class M>
void load_data(xr_vector<LPSTR> &tpVector, M &tNetPacket, bool bSaveCount = true)
{
	if (bSaveCount)
		tpVector.resize			(tNetPacket.r_u32());
	string4096					S;
	xr_vector<LPSTR>::iterator	I = tpVector.begin();
	xr_vector<LPSTR>::iterator	E = tpVector.end();
	for ( ; I != E; I++) {
		tNetPacket.r_string		(S);
		*I						= xr_strdup(S);
	}
};

template <class M>
void load_data(LPSTR &tpData, M &tNetPacket, bool bSaveCount = true)
{
	string4096					S;
	tNetPacket.r_string			(S);
	tpData						= xr_strdup(S);
};

template <class T, class M>
void load_data(T &tData, M &tNetPacket, bool bSaveCount = true)
{
	tNetPacket.r				(&tData,sizeof(tData));
}

template <class T, class M>
void load_data(T *&tpData, M &tNetPacket, bool bSaveCount = true)
{
	tpData->Load				(tNetPacket);
};

#endif
//----------------------------------------------------
// file: PSLibrary.h
//----------------------------------------------------
#ifndef _INCDEF_PSLibrary_H_
#define _INCDEF_PSLibrary_H_

#include				"PSRuntime.h"
DEFINE_VECTOR			(PS::SDef_RT, PSList, PSIt);

class CPSLibrary
{
    PSList				m_PSs;
public:
						CPSLibrary		()	{;}
    virtual 			~CPSLibrary		()	{m_PSs.clear();}

    // get object properties methods
    IC PSIt				FirstPS			()	{return m_PSs.begin();}
    IC PSIt				LastPS			()	{return m_PSs.end();}
    IC int				PSCount			()	{return m_PSs.size();}
    
    void				xrStartUp		();
    void				xrShutDown		();
	void				OnDeviceCreate	();
	void				OnDeviceDestroy	();

    PS::SDef_RT*		FindPS			(LPCSTR name);
};

#endif /*_INCDEF_PSLibrary_H_*/


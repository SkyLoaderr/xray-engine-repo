//----------------------------------------------------
// file: PSLibrary.h
//----------------------------------------------------
#ifndef _INCDEF_PSLibrary_H_
#define _INCDEF_PSLibrary_H_

#include "ParticleSystem.h"
DEFINE_VECTOR(PS::SDef, PSList, PSIt);

#define PSLIB_FILENAME "particles.xr"

class CPSLibrary{
    PSList				m_PSs;
public:
						CPSLibrary		(){;}
    virtual 			~CPSLibrary		(){m_PSs.clear();}

    // get object properties methods
    IC PSIt				FirstPS			()	{return m_PSs.begin();}
    IC PSIt				LastPS			()	{return m_PSs.end();}
    IC int				PSCount			()	{return m_PSs.size();}
    
    void				xrStartUp		();
    void				xrShutDown		();
    PS::SDef*			FindPS			(LPCSTR name);
};

extern CPSLibrary* PSLib;
#endif /*_INCDEF_PSLibrary_H_*/


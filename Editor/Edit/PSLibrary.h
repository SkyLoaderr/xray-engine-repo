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
    bool 				Load			(const char* nm);
    void				Save			(const char* nm);
    AnsiString			m_CurrentPS;
public:
						CPSLibrary		(){;}
    virtual 			~CPSLibrary		(){m_PSs.clear();}

    // get object properties methods
    IC PSIt				FirstPS			()	{return m_PSs.begin();}
    IC PSIt				LastPS			()	{return m_PSs.end();}
    IC int				PSCount			()	{return m_PSs.size();}
    
    void				Init			();
    void				Clear			();
    int 				Merge			(const char* nm);
    void				Save			();
    void				Reload			();
    void 				Backup			();
    void 				RestoreBackup	();

    char*				GenerateName	(char* buffer, const char* pref=0);

    PS::SDef*			AddPS			(const char* name, PS::SDef* src=0);
    void				DeletePS		(const char* name);
    PS::SDef*			FindPS			(const char* name);
    void				Sort			();

	PS::SDef*			GetCurrentPS	(){return m_CurrentPS.IsEmpty()?0:FindPS(m_CurrentPS.c_str());}
    PS::SDef*			ChoosePS		(bool bSetCurrent=true);
    
    bool 				Validate		(){return true;}
};

extern CPSLibrary* PSLib;
#endif /*_INCDEF_PSLibrary_H_*/


//----------------------------------------------------
// file: PSLibrary.h
//----------------------------------------------------
#ifndef _INCDEF_PSLibrary_H_
#define _INCDEF_PSLibrary_H_

#include "ParticleSystem.h"

#define PSLIB_FILENAME "particles.xr"

class CPSLibrary{
    PS::PSVec			m_PSs;
    bool 				Load			(LPCSTR nm);
    void				Save			(LPCSTR nm);
    AnsiString			m_CurrentPS;
public:
						CPSLibrary		(){;}
    virtual 			~CPSLibrary		(){m_PSs.clear();}

    // get object properties methods
    IC PS::PSIt			FirstPS			()	{return m_PSs.begin();}
    IC PS::PSIt			LastPS			()	{return m_PSs.end();}
    IC int				PSCount			()	{return m_PSs.size();}

    void				OnCreate		();
    void				OnDestroy		();
    int 				Merge			(LPCSTR nm);
    void				Save			();
    void				Reload			();

    char*				GenerateName	(char* buffer, LPCSTR pref=0);

    PS::SDef*			AppendPS		(PS::SDef* src=0);
    void				RemovePS		(LPCSTR name);
    PS::SDef*			FindPS			(LPCSTR name);
    void				RenamePS		(PS::SDef* src, LPCSTR new_name);

	PS::SDef*			GetCurrentPS	(){return m_CurrentPS.IsEmpty()?0:FindPS(m_CurrentPS.c_str());}
    PS::SDef*			ChoosePS		(bool bSetCurrent=true);

    bool 				Validate		(){return true;}
};

extern CPSLibrary PSLib;
#endif /*_INCDEF_PSLibrary_H_*/


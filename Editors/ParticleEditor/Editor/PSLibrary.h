//----------------------------------------------------
// file: PSLibrary.h
//----------------------------------------------------
#ifndef PSLibraryH
#define PSLibraryH

#include "ParticleSystem.h"

#define PSLIB_FILENAME "particles2.xr"

class CPSLibrary{
    PS::PSVec			m_PSs;
    bool 				Load			(LPCSTR nm);
    void				Save			(LPCSTR nm);
    AnsiString			m_CurrentPS;

    void				Sort			();
public:
						CPSLibrary		(){;}
    virtual 			~CPSLibrary		(){m_PSs.clear();}

    void				OnCreate		();
    void				OnDestroy		();

    PS::SDef*			FindSorted		(LPCSTR name);
    PS::SDef*			FindUnsorted	(LPCSTR name);

	// editor part
    char*				GenerateName	(char* buffer, LPCSTR pref=0);

    // get object properties methods
    IC PS::PSIt			FirstPS			()	{return m_PSs.begin();}
    IC PS::PSIt			LastPS			()	{return m_PSs.end();}
    IC int				PSCount			()	{return m_PSs.size();}

    PS::SDef*			AppendPS		(PS::SDef* src=0);
    void				RemovePS		(LPCSTR name);
    void				RenamePS		(PS::SDef* src, LPCSTR new_name);

	PS::SDef*			GetCurrentPS	(){return m_CurrentPS.IsEmpty()?0:FindUnsorted(m_CurrentPS.c_str());}
    PS::SDef*			ChoosePS		(bool bSetCurrent=true);

    void				Reload			();
    void				Save			();
};

#define PS_VERSION				0x0001
//----------------------------------------------------
#define PS_CHUNK_VERSION		0x0001
#define PS_CHUNK_FIRSTGEN		0x0002
#define PS_CHUNK_SECONDGEN		0x0003

extern CPSLibrary PSLib;
#endif /*_INCDEF_PSLibrary_H_*/


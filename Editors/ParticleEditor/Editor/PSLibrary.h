//----------------------------------------------------
// file: PSLibrary.h
//----------------------------------------------------
#ifndef PSLibraryH
#define PSLibraryH

#include "ParticleSystem.h"
#include "ParticleEffect.h"

class ENGINE_API CPSLibrary{
    PS::PSVec			m_PSs;
    bool 				Load			(LPCSTR nm);
    void				Save			(LPCSTR nm);

    string256			m_CurrentPE;
    PS::PEVec			m_PEs;
public:
						CPSLibrary		(){;}
    		 			~CPSLibrary		(){m_PSs.clear();}

    void				OnCreate		();
    void				OnDestroy		();

    PS::SDef*			FindPS			(LPCSTR name);
    PS::CPEDef*			FindPE			(LPCSTR name);
    PS::PEIt			FindPEIt		(LPCSTR name);
	// editor part
    char*				GenerateName	(char* buffer, LPCSTR folder, LPCSTR pref);

    // get object properties methods
    IC PS::PSIt			FirstPS			()	{return m_PSs.begin();}
    IC PS::PSIt			LastPS			()	{return m_PSs.end();}
    IC PS::PEIt			FirstPE			()	{return m_PEs.begin();}
    IC PS::PEIt			LastPE			()	{return m_PEs.end();}

    PS::SDef*			AppendPS		(PS::SDef* src=0);
    PS::CPEDef*			AppendPE		(PS::CPEDef* src=0);
    void				Remove			(LPCSTR name);
    void				RenamePS		(PS::SDef* src, LPCSTR new_name);
    void				RenamePE		(PS::CPEDef* src, LPCSTR new_name);

#ifdef _EDITOR
	LPCSTR				GetCurrentPE	(bool bChooseWindow=true){return (bChooseWindow&&!m_CurrentPE[0])?ChoosePE():(m_CurrentPE[0]?m_CurrentPE:0);}
    LPCSTR				ChoosePE		();
#endif
    void				Reload			();
    void				Save			();

	void				OnDeviceCreate	();
	void				OnDeviceDestroy	();
};

#define PSLIB_FILENAME 			"particles2.xr"
#define PS_LIB_SIGN 			"PS_LIB"

#define PS_VERSION				0x0001
//----------------------------------------------------
#define PS_CHUNK_VERSION		0x0001
#define PS_CHUNK_FIRSTGEN		0x0002
#define PS_CHUNK_SECONDGEN		0x0003

#endif /*_INCDEF_PSLibrary_H_*/


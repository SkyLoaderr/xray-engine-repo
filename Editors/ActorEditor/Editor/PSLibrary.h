//----------------------------------------------------
// file: PSLibrary.h
//----------------------------------------------------
#ifndef PSLibraryH
#define PSLibraryH

#include "ParticleSystem.h"
#include "ParticleEffect.h"
#include "ParticleGroup.h"

class CPSLibrary{
    PS::PSVec			m_PSs;

    PS::PEDVec			m_PEDs;
    PS::PGDVec			m_PGDs;

#ifdef _EDITOR    
    AnsiString			m_CurrentParticles;
#endif

    bool 				Load			(LPCSTR nm);
    void				Save			(LPCSTR nm);
public:
						CPSLibrary		(){;}
    		 			~CPSLibrary		(){m_PSs.clear();}

    void				OnCreate		();
    void				OnDestroy		();

    PS::SDef*			FindPS			(LPCSTR name);
    PS::CPEDef*			FindPED			(LPCSTR name);
    PS::PEDIt			FindPEDIt		(LPCSTR name);
    PS::CPGDef*			FindPGD			(LPCSTR name);
    PS::PGDIt			FindPGDIt		(LPCSTR name);
	// editor part
    char*				GenerateName	(char* buffer, LPCSTR folder, LPCSTR pref);

    // get object properties methods
    IC PS::PSIt			FirstPS			()	{return m_PSs.begin();}
    IC PS::PSIt			LastPS			()	{return m_PSs.end();}
    IC PS::PEDIt		FirstPED		()	{return m_PEDs.begin();}
    IC PS::PEDIt		LastPED			()	{return m_PEDs.end();}
    IC PS::PGDIt		FirstPGD		()	{return m_PGDs.begin();}
    IC PS::PGDIt		LastPGD			()	{return m_PGDs.end();}

    PS::SDef*			AppendPS		(PS::SDef* src=0);
    PS::CPEDef*			AppendPED		(PS::CPEDef* src=0);
    PS::CPGDef*			AppendPGD		(PS::CPGDef* src=0);
    void				Remove			(LPCSTR name);
    void				RenamePS		(PS::SDef* src, LPCSTR new_name);
    void				RenamePED		(PS::CPEDef* src, LPCSTR new_name);
    void				RenamePGD		(PS::CPGDef* src, LPCSTR new_name);

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
#define PS_CHUNK_THIRDGEN		0x0004

#endif /*_INCDEF_PSLibrary_H_*/


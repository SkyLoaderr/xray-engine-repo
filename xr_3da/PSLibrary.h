//----------------------------------------------------
// file: PSLibrary.h
//----------------------------------------------------
#ifndef PSLibraryH
#define PSLibraryH

#include "ParticleSystem.h"
#include "ParticleGroup.h"

class ENGINE_API CPSLibrary{
    PS::PSVec			m_PSs;
    bool 				Load			(LPCSTR nm);
    void				Save			(LPCSTR nm);
    string256			m_CurrentPS;

    PS::PGVec			m_PGs;
public:
						CPSLibrary		(){;}
    		 			~CPSLibrary		(){m_PSs.clear();}

    void				OnCreate		();
    void				OnDestroy		();

    PS::SDef*			FindPS			(LPCSTR name);
    PS::CPGDef*			FindPG			(LPCSTR name);
    PS::PGIt			FindPGIt		(LPCSTR name);
	// editor part
    char*				GenerateName	(char* buffer, LPCSTR folder, LPCSTR pref);

    // get object properties methods
    IC PS::PSIt			FirstPS			()	{return m_PSs.begin();}
    IC PS::PSIt			LastPS			()	{return m_PSs.end();}
    IC PS::PGIt			FirstPG			()	{return m_PGs.begin();}
    IC PS::PGIt			LastPG			()	{return m_PGs.end();}

    PS::SDef*			AppendPS		(PS::SDef* src=0);
    PS::CPGDef*			AppendPG		(PS::CPGDef* src=0);
    void				Remove			(LPCSTR name);
    void				RenamePS		(PS::SDef* src, LPCSTR new_name);
    void				RenamePG		(PS::CPGDef* src, LPCSTR new_name);

	PS::SDef*			GetCurrentPS	(){return m_CurrentPS[0]?0:FindPS(m_CurrentPS);}
    PS::SDef*			ChoosePS		(bool bSetCurrent=true);

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

extern CPSLibrary PSLib;
#endif /*_INCDEF_PSLibrary_H_*/


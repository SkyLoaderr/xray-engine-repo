//---------------------------------------------------------------------------
#ifndef ImageManagerH
#define ImageManagerH

#include "folderlib.h"
#include "etextureparams.h"
#include "EThumbnail.h"

class ECORE_API CImageManager{
    void		MakeThumbnailImage	(ETextureThumbnail* THM, u32* data, u32 w, u32 h, u32 a);
    bool		MakeGameTexture		(ETextureThumbnail* THM, LPCSTR game_name, u32* data);
    bool		MakeGameTexture		(LPCSTR game_name, u32* data, u32 w, u32 h, STextureParams::ETFormat fmt, STextureParams::ETType, u32 flags=0);
public:
				CImageManager		(){;}
				~CImageManager		(){;}
	// texture routines
    void __stdcall 	RemoveTexture	(LPCSTR fname, EItemType type, bool& res);
    BOOL		CheckCompliance		(LPCSTR fname, int& compl);
    void		CheckCompliance		(FS_QueryMap& files, FS_QueryMap& compl);
    int			GetTextures			(FS_QueryMap& files, BOOL bFolder=FALSE);
//	int			GetServerModifiedTextures(CLocatorAPI::files_query& files);
	int 		GetLocalNewTextures	(FS_QueryMap& files);
	void		SafeCopyLocalToServer(FS_QueryMap& files);

	void		SynchronizeTextures	(bool sync_thm, bool sync_game, bool bForceGame, FS_QueryMap* source_map, AStringVec* sync_list_without_extention, FS_QueryMap* modif_map=0);
    void 		SynchronizeTexture	(LPCSTR tex_name, int age);
	// make/update routines
    void		CreateTextureThumbnail(ETextureThumbnail* THM, const AnsiString& src_name, LPCSTR path=0, bool bSetDefParam=true);
    BOOL		CreateOBJThumbnail	(LPCSTR tex_name, CEditableObject* obj, int age);
    void		CreateLODTexture	(Fbox bbox, LPCSTR tex_name, u32 tgt_w, u32 tgt_h, int samples, int age);
    void		CreateGameTexture	(LPCSTR src_name, ETextureThumbnail* thumb=0);
    bool		LoadTextureData		(LPCSTR src_name, U32Vec& data, u32& w, u32& h);

    // result 0-can't fit images, 1-ok, -1 can't load image 
    void		MergedTextureRemapUV(float& dest_u, float& dest_v, float src_u, float src_v, const Fvector2& offs, const Fvector2& scale, bool bRotate);
    int			CreateMergedTexture	(const RStringVec& src_names, LPCSTR dest_name, STextureParams::ETFormat fmt, int dest_width, int dest_height, Fvector2Vec& dest_offset, Fvector2Vec& dest_scale, boolVec& dest_rotate, U32Vec& remap);
    int			CreateMergedTexture	(const RStringVec& src_names, LPCSTR dest_name, STextureParams::ETFormat fmt, int dest_width_min, int dest_width_max, int dest_height_min, int dest_height_max, Fvector2Vec& dest_offset, Fvector2Vec& dest_scale, boolVec& dest_rotate, U32Vec& remap);

    EImageThumbnail* CreateThumbnail(LPCSTR src_name, ECustomThumbnail::THMType type, bool bLoad=true);

    void 		RefreshTextures		(AStringVec* modif);

    std::string	UpdateFileName		(std::string& fn);

    void		WriteAssociation	(CInifile* ltx_ini, LPCSTR base_name, const STextureParams& fmt);

    BOOL		CreateSmallerCubeMap(LPCSTR src_name, LPCSTR dst_name);
};

extern ECORE_API CImageManager ImageLib;
//---------------------------------------------------------------------------
#endif

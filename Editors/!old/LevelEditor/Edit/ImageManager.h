//---------------------------------------------------------------------------
#ifndef ImageManagerH
#define ImageManagerH

#include "folderlib.h"
#include "etextureparams.h"
#include "EThumbnail.h"

struct SSimpleImage{
	shared_str	name;
    DEFINE_VECTOR(U32Vec,DATAVec,DATAIt);
	DATAVec		layers;
    u32			w,h,a;
    u32			tag;
    int 		LongestEdge()const 	{return (w>h)?w:h;}
    int			Area()const			{return w*h;}
};
IC bool operator == (const SSimpleImage& a, shared_str nm){return a.name==nm;}
IC bool operator < (const SSimpleImage& a, const SSimpleImage& b){return a.name<b.name;}
DEFINE_VECTOR	(SSimpleImage,SSimpleImageVec,SSimpleImageVecIt);

class ECORE_API CImageManager{
    void		MakeThumbnailImage	(ETextureThumbnail* THM, u32* data, u32 w, u32 h, u32 a);
    bool		MakeGameTexture		(ETextureThumbnail* THM, LPCSTR game_name, u32* data);
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
//	void		ChangeFileAgeTo		(FS_QueryMap* source_map, int age);
	// make/update routines
    bool		MakeGameTexture		(LPCSTR game_name, u32* data, u32 w, u32 h, STextureParams::ETFormat fmt, STextureParams::ETType, u32 flags=0);
    void		CreateTextureThumbnail(ETextureThumbnail* THM, const AnsiString& src_name, LPCSTR path=0, bool bSetDefParam=true);
    BOOL		CreateOBJThumbnail	(LPCSTR tex_name, CEditableObject* obj, int age);
    void		CreateLODTexture	(const Fbox& bb, 	U32Vec& tgt_data, 	u32 tgt_w, u32 tgt_h, int samples);
    void		CreateLODTexture	(const Fbox& bbox, 	LPCSTR tex_name, 	u32 tgt_w, u32 tgt_h, int samples, int age);
    void		CreateGameTexture	(LPCSTR src_name, ETextureThumbnail* thumb=0);
    bool		LoadTextureData		(LPCSTR src_name, U32Vec& data, u32& w, u32& h);

    // result 0-can't fit images, 1-ok, -1 can't load image 
    void		MergedTextureRemapUV(float& dest_u, float& dest_v, float src_u, float src_v, const Fvector2& offs, const Fvector2& scale, bool bRotate);
    int			CreateMergedTexture	(const RStringVec& src_names, LPCSTR dest_name, STextureParams::ETFormat fmt, int dest_width, int dest_height, Fvector2Vec& dest_offset, Fvector2Vec& dest_scale, boolVec& dest_rotate, U32Vec& remap);
    int			CreateMergedTexture	(const RStringVec& src_names, LPCSTR dest_name, STextureParams::ETFormat fmt, int dest_width_min, int dest_width_max, int dest_height_min, int dest_height_max, Fvector2Vec& dest_offset, Fvector2Vec& dest_scale, boolVec& dest_rotate, U32Vec& remap);
    int			CreateMergedTexture	(u32 layer_cnt, SSimpleImageVec& src_images, SSimpleImage& dst_image, int dest_width, int dest_height, Fvector2Vec& dest_offset, Fvector2Vec& dest_scale, boolVec& dest_rotate, U32Vec& remap);
	int			CreateMergedTexture	(u32 layer_cnt, SSimpleImageVec& src_images, SSimpleImage& dst_image, int dest_width_min, int dest_width_max, int dest_height_min, int dest_height_max, Fvector2Vec& dest_offset, Fvector2Vec& dest_scale, boolVec& dest_rotate, U32Vec& remap);
	void 		ApplyBorders		(U32Vec& tgt_data, u32 w, u32 h);

    EImageThumbnail* CreateThumbnail(LPCSTR src_name, ECustomThumbnail::THMType type, bool bLoad=true);

    void 		RefreshTextures		(AStringVec* modif);

    xr_string	UpdateFileName		(xr_string& fn);

    void		WriteAssociation	(CInifile* ltx_ini, LPCSTR base_name, const STextureParams& fmt);

    BOOL		CreateSmallerCubeMap(LPCSTR src_name, LPCSTR dst_name);
};

extern ECORE_API CImageManager ImageLib;
//---------------------------------------------------------------------------
#endif

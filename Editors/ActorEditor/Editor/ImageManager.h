//---------------------------------------------------------------------------
#ifndef ImageManagerH
#define ImageManagerH

#include "folderlib.h"
#include "etextureparams.h"

class EImageThumbnail;

class CImageManager{
    void		MakeThumbnailImage	(EImageThumbnail* THM, u32* data, u32 w, u32 h, u32 a);
    void		MakeGameTexture		(EImageThumbnail* THM, LPCSTR game_name, u32* data);
    void		MakeGameTexture		(LPCSTR game_name, u32* data, u32 w, u32 h, STextureParams::ETFormat fmt, bool bGenMipMap);
public:
				CImageManager		(){;}
				~CImageManager		(){;}
	// texture routines
    BOOL __fastcall	RemoveTexture	(LPCSTR fname, EItemType type);
    BOOL		CheckCompliance		(LPCSTR fname, int& compl);
    void		CheckCompliance		(FS_QueryMap& files, FS_QueryMap& compl);
    int			GetTextures			(FS_QueryMap& files);
//	int			GetServerModifiedTextures(CLocatorAPI::files_query& files);
	int 		GetLocalNewTextures	(FS_QueryMap& files);
	void		SafeCopyLocalToServer(FS_QueryMap& files);
	void		SynchronizeTextures	(bool sync_thm, bool sync_game, bool bForceGame, FS_QueryMap* source_map, AStringVec* sync_list_without_extention, FS_QueryMap* modif_map=0);
    void 		SynchronizeTexture	(LPCSTR tex_name, int age);
	// make/update routines
    void		CreateTextureThumbnail(EImageThumbnail* THM, const AnsiString& src_name, LPCSTR path=0, bool bSetDefParam=true);
    BOOL		CreateOBJThumbnail	(LPCSTR tex_name, CEditableObject* obj, int age);
    void		CreateLODTexture	(Fbox bbox, LPCSTR tex_name, u32 tgt_w, u32 tgt_h, int samples, int age);
    void		CreateGameTexture	(const AnsiString& src_name, EImageThumbnail* thumb=0);
    bool		LoadTextureData		(const AnsiString& src_name, U32Vec& data, u32& w, u32& h);

    // result 0-can't fit images, 1-ok, -1 can't load image 
    int			CreateMergedTexture	(AStringVec& src_names, LPCSTR dest_name, int dest_width, int dest_height, Fvector2Vec& dest_offset, Fvector2Vec& dest_scale, boolVec& dest_rotate);
};

extern CImageManager ImageManager;
//---------------------------------------------------------------------------
#endif

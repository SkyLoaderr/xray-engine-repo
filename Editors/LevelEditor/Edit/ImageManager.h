//---------------------------------------------------------------------------
#ifndef ImageManagerH
#define ImageManagerH

class EImageThumbnail;

class CImageManager{
    void		MakeThumbnailImage	(EImageThumbnail* THM, u32* data, u32 w, u32 h, u32 a);
    void		MakeGameTexture		(EImageThumbnail* THM, LPCSTR game_name, u32* data);
public:
				CImageManager		(){;}
				~CImageManager		(){;}
	// texture routines
    BOOL __fastcall	RemoveTexture	(LPCSTR fname);
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
};

extern CImageManager ImageManager;
//---------------------------------------------------------------------------
#endif

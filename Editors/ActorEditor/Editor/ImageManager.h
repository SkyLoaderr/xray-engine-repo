//---------------------------------------------------------------------------
#ifndef EImageManagerH
#define EImageManagerH

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
    void		CheckCompliance		(FileMap& files, FileMap& compl);
    int			GetTextures			(FileMap& files);
	int			GetServerModifiedTextures(FileMap& files);
	int 		GetLocalNewTextures	(FileMap& files);
	void		SafeCopyLocalToServer(FileMap& files);
	void		SynchronizeTextures	(bool sync_thm, bool sync_game, bool bForceGame, FileMap* source_map, LPSTRVec* sync_list_without_extention, FileMap* modif_map=0);
    void 		SynchronizeTexture	(LPCSTR tex_name, int age);
    void		FreeModifVec		(LPSTRVec& vect){for (LPSTRIt it=vect.begin(); it!=vect.end(); it++) _FREE(*it);};
	// make/update routines
    void		CreateTextureThumbnail(EImageThumbnail* THM, const AnsiString& src_name, FSPath* path=0, bool bSetDefParam=true); 
    BOOL		CreateOBJThumbnail	(LPCSTR tex_name, CEditableObject* obj, int age);
    void		CreateLODTexture	(Fbox bbox, LPCSTR tex_name, u32 tgt_w, u32 tgt_h, int samples, int age);
    void		CreateGameTexture	(const AnsiString& src_name, EImageThumbnail* thumb=0);
    bool		LoadTextureData		(const AnsiString& src_name, U32Vec& data, u32& w, u32& h);
};

extern CImageManager ImageManager;
//---------------------------------------------------------------------------
#endif

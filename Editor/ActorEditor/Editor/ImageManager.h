//---------------------------------------------------------------------------
#ifndef EImageManagerH
#define EImageManagerH

class EImageThumbnail;

class CImageManager{
    void		MakeThumbnail		(EImageThumbnail* THM, DWORD* data, int w, int h, int a);
    void		MakeGameTexture		(EImageThumbnail* THM, LPCSTR game_name, DWORD* data);
public:
				CImageManager		(){;}
				~CImageManager		(){;}
	// sync routines
    int			GetTextures			(FileMap& files);
	int			GetModifiedTextures	(FileMap& files);
	void		SynchronizeThumbnail(EImageThumbnail* THM, LPCSTR src_name);
	void		SynchronizeTextures	(bool sync_thm, bool sync_game, bool bForceGame, FileMap* source_map, LPSTRVec* sync_list);
    void		FreeModifVec		(LPSTRVec& vect){for (LPSTRIt it=vect.begin(); it!=vect.end(); it++) _FREE(*it);};
	// make/update routines
    void		CreateThumbnail		(EImageThumbnail* THM, const AnsiString& src_name);
    void		CreateGameTexture	(const AnsiString& src_name, EImageThumbnail* thumb=0);
    bool		LoadTextureData		(const AnsiString& src_name, DWORDVec& data, int& w, int& h);
};

extern CImageManager ImageManager;
//---------------------------------------------------------------------------
#endif

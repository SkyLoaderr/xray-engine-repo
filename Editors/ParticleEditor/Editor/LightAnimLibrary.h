//---------------------------------------------------------------------------

#ifndef LightAnimLibraryH
#define LightAnimLibraryH
//---------------------------------------------------------------------------

class CLAItem{
public:
    string64    cName;
    float   	fFPS;
    DEFINE_MAP(int,DWORD,KeyMap,KeyPairIt);
    KeyMap		Keys;
    int			iFrameCount;
public:
                CLAItem				();
	void		InitDefault			();
    void		Load				(CStream& F);
    void		Save				(CFS_Base& F);
    DWORD		Interpolate			(int frame);
    DWORD		Calculate			(float T, int& frame);
    void		Resize				(int new_len);
    void		InsertKey			(int frame, DWORD color);
    void		DeleteKey			(int frame);
    void		MoveKey				(int from, int to);
    bool		IsKey				(int frame){return (Keys.end()!=Keys.find(frame));}
    int		 	PrevKeyFrame		(int frame);
    int		 	NextKeyFrame 		(int frame);
    int		 	FirstKeyFrame		(){return Keys.rend()->first;}
    int		 	LastKeyFrame		(){return Keys.rbegin()->first;}
};
DEFINE_VECTOR(CLAItem*,LAItemVec,LAItemIt);

class ELightAnimLibrary{
    LPCSTR			GenerateName		(LPSTR name, LPCSTR source);
public:
    LAItemVec       Items;
    LAItemIt		FindItemI			(LPCSTR name);
    CLAItem*		FindItem			(LPCSTR name);
public:
					ELightAnimLibrary	();
					~ELightAnimLibrary	();
    void			OnCreate			();
    void			OnDestroy			();
    void			Load				();
    void			Save				();
    void			Reload				();
    void			Unload				();
    CLAItem*		AppendItem			(LPCSTR folder_name, CLAItem* parent);
    void			DeleteItem			(LPCSTR name);
    void			RenameItem			(LPCSTR old_name, LPCSTR new_name);
    LAItemVec&		Objects				(){return Items;}
};

extern ELightAnimLibrary LALib;

#endif

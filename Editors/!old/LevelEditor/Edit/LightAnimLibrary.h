//---------------------------------------------------------------------------

#ifndef LightAnimLibraryH
#define LightAnimLibraryH
//---------------------------------------------------------------------------

class ENGINE_API CLAItem{
public:
    string64    cName;
    float   	fFPS;
    DEFINE_MAP(int,u32,KeyMap,KeyPairIt);
    KeyMap		Keys;
    int			iFrameCount;
public:
                CLAItem				();
	void		InitDefault			();
    void		Load				(IReader& F);
    void		Save				(IWriter& F);
    u32			Interpolate			(int frame);
    u32			Calculate			(float T, int& frame);
    void		Resize				(int new_len);
    void		InsertKey			(int frame, u32 color);
    void		DeleteKey			(int frame);
    void		MoveKey				(int from, int to);
    bool		IsKey				(int frame){return (Keys.end()!=Keys.find(frame));}
    int		 	PrevKeyFrame		(int frame);
    int		 	NextKeyFrame 		(int frame);
    int		 	FirstKeyFrame		(){return Keys.rend()->first;}
    int		 	LastKeyFrame		(){return Keys.rbegin()->first;}
};
DEFINE_VECTOR(CLAItem*,LAItemVec,LAItemIt);

class ENGINE_API ELightAnimLibrary{
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

extern ENGINE_API ELightAnimLibrary LALib;

#endif

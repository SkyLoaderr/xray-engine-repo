//---------------------------------------------------------------------------

#ifndef LightAnimLibraryH
#define LightAnimLibraryH
//---------------------------------------------------------------------------
#ifdef _EDITOR
	#include "FolderLib.h"              
#endif

class ENGINE_API CLAItem				{
public:
    shared_str		cName;
    float   		fFPS;
    DEFINE_MAP		(int,u32,KeyMap,KeyPairIt);
    KeyMap			Keys;
    int				iFrameCount;
public:
                	CLAItem				();

	void			InitDefault			();
    void			Load				(IReader& F);
    void			Save				(IWriter& F);
    u32				Interpolate			(int frame);
    u32				CalculateRGB		(float T, int& frame);
    u32				CalculateBGR		(float T, int& frame);
    void		    Resize				(int new_len);
    void		    InsertKey			(int frame, u32 color);
    void		    DeleteKey			(int frame);
    void		    MoveKey				(int from, int to);
    bool		    IsKey				(int frame){return (Keys.end()!=Keys.find(frame));}
    int		 	    PrevKeyFrame		(int frame);
    int		 	    NextKeyFrame 		(int frame);
    int		 	    FirstKeyFrame		(){return Keys.rend()->first;}
    int		 	    LastKeyFrame		(){return Keys.rbegin()->first;}
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
#ifdef _EDITOR       
    void __stdcall 	RemoveObject		(LPCSTR fname, EItemType type, bool& res);
    void __stdcall 	RenameObject		(LPCSTR fn0, LPCSTR fn1, EItemType type);
#endif

    void			OnCreate			();
    void			OnDestroy			();
    void			Load				();
    void			Save				();
    void			Reload				();
    void			Unload				();
    CLAItem*		AppendItem			(LPCSTR folder_name, CLAItem* parent);
    LAItemVec&		Objects				(){return Items;}
};

extern ENGINE_API ELightAnimLibrary LALib;

#endif

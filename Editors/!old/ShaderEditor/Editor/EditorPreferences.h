//---------------------------------------------------------------------------

#ifndef EditorPreferencesH
#define EditorPreferencesH
//---------------------------------------------------------------------------
// refs
class TProperties;

//---------------------------------------------------------------------------
enum{
    epoDrawPivot		= (1<<0),
    epoDrawAnimPath		= (1<<1),
    epoDrawJoints		= (1<<2),
    epoDrawBoneAxis		= (1<<3),
    epoDrawBoneNames	= (1<<4),
    epoDrawBoneShapes	= (1<<5),
    epoShowHint			= (1<<6),
    epoDrawLOD			= (1<<7),
};
class ECORE_API CEditorPreferences
{
private:	// User declarations
    TProperties*	m_ItemProps;
public:
	// view
    float 	view_np;
    float 	view_fp;
    float 	view_fov;
	// fog    
    u32 	fog_color;
    float	fog_fogness;
    // camera
    float	cam_fly_speed;
    float	cam_fly_alt;
    float	cam_sens_rot;
    float	cam_sens_move;
	// tools mouse
    float	tools_sens_rot;
    float	tools_sens_move;
    float	tools_sens_scale;
    // box pick
    BOOL	bp_lim_depth;
    BOOL	bp_cull;
    float	bp_depth_tolerance;
    // snap
    float	snap_angle;
    float	snap_move;
    float	snap_moveto;
    // grid
    float	grid_cell_size;
    u32 	grid_cell_count;
    // scene
    u32		scene_undo_level;
    u32		scene_recent_count;
    BOOL	scene_leave_eo_copy;
    u32		scene_clear_color;
    AStringVec scene_recent_list;
    // sound
	float	sound_rolloff;
    // objects
    Flags32	object_flags;
private:
	void 	__stdcall  OnClose	();
    void	ApplyValues			();
public:		// User declarations
    		CEditorPreferences	();
    void	Edit				();
    
    void	OnCreate			();
    void	OnDestroy			();

    void 	AppendRecentFile	(LPCSTR name);
    LPCSTR 	FirstRecentFile		(){return scene_recent_list.empty()?"":scene_recent_list.front().c_str();}
};
//---------------------------------------------------------------------------
extern ECORE_API CEditorPreferences EPrefs;
//---------------------------------------------------------------------------

#endif

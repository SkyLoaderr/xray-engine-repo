//---------------------------------------------------------------------------

#ifndef EditorPreferencesH
#define EditorPreferencesH
//---------------------------------------------------------------------------
// refs
class TProperties;

//---------------------------------------------------------------------------
class CEditorPreferences
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
private:
	void 	__fastcall OnClose	(void);
    void	ApplyValues			();
public:		// User declarations
    		CEditorPreferences	();
    void	Edit				();
    
    void	OnCreate			();
    void	OnDestroy			();
};
//---------------------------------------------------------------------------
extern CEditorPreferences EPrefs;
//---------------------------------------------------------------------------

#endif

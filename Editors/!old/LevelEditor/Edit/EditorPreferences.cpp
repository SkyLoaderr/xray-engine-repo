//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PropertiesList.h"
#include "ui_main.h"
#include "ui_toolscustom.h"
//---------------------------------------------------------------------------
CEditorPreferences EPrefs;
//---------------------------------------------------------------------------

CEditorPreferences::CEditorPreferences()
{
	// view
    view_np				= 0.1f;
    view_fp				= 1500.f;
    view_fov			= deg2rad(60.f);
	// fog    
    fog_color			= 0x00555555;
    fog_fogness			= 0.9;
    // camera
    cam_fly_speed		= 5.0f;
    cam_fly_alt			= 1.8f;
    cam_sens_rot		= 0.6f;
    cam_sens_move		= 0.6f;
	// tools mouse
    tools_sens_move		= 0.3f;
    tools_sens_rot		= 0.3f;
    tools_sens_scale	= 0.3f;
    // box pick
    bp_lim_depth		= TRUE;
    bp_cull				= TRUE;
    bp_depth_tolerance	= 0.1f;
    // snap
    snap_angle			= deg2rad(5.f);
    snap_move			= 0.1f;
    snap_moveto			= 0.5f;
    // grid
    grid_cell_size		= 1.f;
    grid_cell_count		= 100;
    // scene
    scene_undo_level	= 125;
    scene_recent_count	= 10;
    scene_leave_eo_copy = TRUE;
    scene_clear_color	= DEFAULT_CLEARCOLOR;
    // sounds
    sound_rolloff		= 1.f;
    // objects
    object_flags.zero	();
}
//---------------------------------------------------------------------------

void CEditorPreferences::ApplyValues()
{
	Tools->m_MoveSnap		= snap_move;
	Tools->m_MoveSnapTo		= snap_moveto;
	Tools->m_RotateSnapAngle= snap_angle;

    Device.m_Camera.SetViewport(view_np, view_fp, rad2deg(view_fov));
    Tools->SetFog	(fog_color,fog_fogness);

    UI->m_MouseSM	= 0.2f*tools_sens_move*tools_sens_move;
    UI->m_MouseSR	= 0.02f*tools_sens_rot*tools_sens_rot;
    UI->m_MouseSS	= 0.02f*tools_sens_scale*tools_sens_scale;

    Device.m_Camera.SetSensitivity	(cam_sens_move, cam_sens_rot);
    Device.m_Camera.SetFlyParams	(cam_fly_speed, cam_fly_alt);

    ExecCommand		(COMMAND_UPDATE_GRID);

	psSoundRolloff	= sound_rolloff;
}
//---------------------------------------------------------------------------

void __stdcall CEditorPreferences::OnClose	()
{
	ApplyValues	();	
}
//---------------------------------------------------------------------------

void CEditorPreferences::Edit()
{
    // fill prop
	PropItemVec props;

    PHelper().CreateFloat	(props,"View\\Near Plane",				&view_np, 			0.01f,	10.f);
    PHelper().CreateFloat	(props,"View\\Far Plane", 				&view_fp,			10.f, 	10000.f);
    PHelper().CreateAngle	(props,"View\\FOV",		  				&view_fov,			deg2rad(0.1f), deg2rad(170.f));

    PHelper().CreateColor	(props,"Fog\\Color",					&fog_color	);
    PHelper().CreateFloat	(props,"Fog\\Fogness",					&fog_fogness, 		0.f, 	100.f);

    PHelper().CreateFloat	(props,"Camera\\Sensetivity\\Move",		&cam_sens_move);
    PHelper().CreateFloat	(props,"Camera\\Sensetivity\\Rotate",	&cam_sens_rot);
    PHelper().CreateFloat	(props,"Camera\\Fly\\Speed",			&cam_fly_speed, 	0.01f, 	100.f);
    PHelper().CreateFloat	(props,"Camera\\Fly\\Altitude",			&cam_fly_alt, 		0.f, 	1000.f);

    PHelper().CreateFloat	(props,"Tools\\Sensetivity\\Move",		&tools_sens_move);
    PHelper().CreateFloat	(props,"Tools\\Sensetivity\\Rotate",	&tools_sens_rot);
    PHelper().CreateFloat	(props,"Tools\\Sensetivity\\Scale",		&tools_sens_scale);

    PHelper().CreateBOOL	(props,"Box Pick\\Limited Depth",		&bp_lim_depth);
    PHelper().CreateBOOL	(props,"Box Pick\\Back Face Culling",	&bp_cull);
    PHelper().CreateFloat	(props,"Box Pick\\Depth Tolerance",		&bp_depth_tolerance,0.f, 	10000.f);

    PHelper().CreateAngle	(props,"Snap\\Angle",					&snap_angle,		0, 		PI_MUL_2);
    PHelper().CreateFloat	(props,"Snap\\Move",					&snap_move, 		0.01f,	1000.f);
    PHelper().CreateFloat	(props,"Snap\\Move To", 				&snap_moveto,		0.01f,	1000.f);

    PHelper().CreateFloat	(props,"Grid\\Cell Size", 				&grid_cell_size,	0.1f,	10.f);
    PHelper().CreateU32	(props,"Grid\\Cell Count", 				&grid_cell_count,	10, 	1000);

    PHelper().CreateColor	(props,"Scene\\Clear Color",			&scene_clear_color	);
    PHelper().CreateU32	(props,"Scene\\Undo Level", 			&scene_undo_level,	0, 		125);
    PHelper().CreateU32	(props,"Scene\\Recent Count", 			&scene_recent_count,0, 		25);
    PHelper().CreateBOOL	(props,"Scene\\Always Keep Object Copy",&scene_leave_eo_copy);

    PHelper().CreateFloat	(props,"Sounds\\Rolloff Factor",		&sound_rolloff, 	0.f,	10.f);
    PHelper().CreateFlag32(props,"Sounds\\Use Hardware",					&psSoundFlags, 	ssHardware);
    PHelper().CreateFlag32(props,"Sounds\\Use EAX",						&psSoundFlags, 	ssEAX);

    PHelper().CreateFlag32(props,"Objects\\Show Hint",					&object_flags, 	epoShowHint);
    PHelper().CreateFlag32(props,"Objects\\Draw Pivot",					&object_flags, 	epoDrawPivot);
    PHelper().CreateFlag32(props,"Objects\\Draw Animation Path",			&object_flags, 	epoDrawAnimPath);
    PHelper().CreateFlag32(props,"Objects\\Draw LOD",						&object_flags, 	epoDrawLOD);
    PHelper().CreateFlag32(props,"Objects\\Skeleton\\Draw Joints",		&object_flags, 	epoDrawJoints);
    PHelper().CreateFlag32(props,"Objects\\Skeleton\\Draw Bone Axis",		&object_flags, 	epoDrawBoneAxis);
    PHelper().CreateFlag32(props,"Objects\\Skeleton\\Draw Bone Names",	&object_flags, 	epoDrawBoneNames);
    PHelper().CreateFlag32(props,"Objects\\Skeleton\\Draw Bone Shapes",	&object_flags, 	epoDrawBoneShapes);
    
	m_ItemProps->AssignItems		(props);
    m_ItemProps->ShowPropertiesModal();
}
//---------------------------------------------------------------------------

#define R_FLOAT_SAFE(S,L,D)	I->line_exist(S,L)?I->r_float(S,L):D;
#define R_U32_SAFE(S,L,D) 	I->line_exist(S,L)?I->r_u32(S,L):D;
#define R_BOOL_SAFE(S,L,D) 	I->line_exist(S,L)?I->r_bool(S,L):D;
#define R_STRING_SAFE(S,L,D)I->line_exist(S,L)?I->r_string_wb(S,L):D;

void CEditorPreferences::OnCreate()
{
	std::string	fn;
	INI_NAME	(fn);
    CInifile* 	I = xr_new<CInifile>(fn.c_str(), TRUE, TRUE, TRUE);

    psDeviceFlags.flags		= R_U32_SAFE	("editor_prefs","device_flags",	psDeviceFlags.flags);
    psSoundFlags.flags		= R_U32_SAFE	("editor_prefs","sound_flags",	psSoundFlags.flags)

    Tools->m_Settings.flags	= R_U32_SAFE	("editor_prefs","tools_settings",Tools->m_Settings.flags);
    
    view_np				= R_FLOAT_SAFE	("editor_prefs","view_np"			,view_np		 	);
    view_fp				= R_FLOAT_SAFE	("editor_prefs","view_fp"			,view_fp		 	);
    view_fov			= R_FLOAT_SAFE	("editor_prefs","view_fov"			,view_fov			);

    fog_color			= R_U32_SAFE	("editor_prefs","fog_color"			,fog_color			);
    fog_fogness			= R_FLOAT_SAFE	("editor_prefs","fog_fogness"		,fog_fogness	 	);

    cam_fly_speed		= R_FLOAT_SAFE	("editor_prefs","cam_fly_speed"		,cam_fly_speed		);
    cam_fly_alt			= R_FLOAT_SAFE	("editor_prefs","cam_fly_alt"		,cam_fly_alt	 	);
    cam_sens_rot		= R_FLOAT_SAFE	("editor_prefs","cam_sens_rot"		,cam_sens_rot		);
    cam_sens_move		= R_FLOAT_SAFE	("editor_prefs","cam_sens_move"		,cam_sens_move		);

    tools_sens_move		= R_FLOAT_SAFE	("editor_prefs","tools_sens_move"	,tools_sens_move  	);
    tools_sens_rot		= R_FLOAT_SAFE	("editor_prefs","tools_sens_rot"	,tools_sens_rot		);
    tools_sens_scale	= R_FLOAT_SAFE	("editor_prefs","tools_sens_scale"	,tools_sens_scale	);
    
    bp_lim_depth		= R_BOOL_SAFE	("editor_prefs","bp_lim_depth"		,bp_lim_depth		);
    bp_cull				= R_BOOL_SAFE	("editor_prefs","bp_lim_depth"		,bp_cull		  	);
    bp_depth_tolerance	= R_FLOAT_SAFE	("editor_prefs","tools_sens_rot"	,bp_depth_tolerance	);

    snap_angle			= R_FLOAT_SAFE	("editor_prefs","snap_angle"		,snap_angle			);
    snap_move			= R_FLOAT_SAFE	("editor_prefs","snap_move"			,snap_move			);
    snap_moveto			= R_FLOAT_SAFE	("editor_prefs","snap_moveto"		,snap_moveto	   	);

    grid_cell_size		= R_FLOAT_SAFE	("editor_prefs","grid_cell_size"	,grid_cell_size		);
    grid_cell_count		= R_U32_SAFE	("editor_prefs","grid_cell_count"	,grid_cell_count   	);

    scene_undo_level	= R_U32_SAFE	("editor_prefs","scene_undo_level"	,scene_undo_level	);
    scene_recent_count	= R_U32_SAFE	("editor_prefs","scene_recent_count",scene_recent_count	);
    scene_leave_eo_copy = R_BOOL_SAFE	("editor_prefs","scene_leave_eo_copy",scene_leave_eo_copy);
    scene_clear_color	= R_U32_SAFE	("editor_prefs","scene_clear_color"	,scene_clear_color	);

    sound_rolloff		= R_FLOAT_SAFE	("editor_prefs","sound_rolloff"		,sound_rolloff   	);

    object_flags.flags	= R_U32_SAFE	("editor_prefs","object_flags"		,object_flags.flags );

	// read recent list    
    for (u32 i=0; i<scene_recent_count; i++){
    	shared_str fn  	= R_STRING_SAFE	("editor_prefs",AnsiString().sprintf("recent_files_%d",i).c_str(),shared_str("") );
        if (fn.size())	scene_recent_list.push_back(*fn);
    }

    xr_delete	(I);

    ApplyValues			();

	m_ItemProps 		= TProperties::CreateModalForm("Editor Preferences",false,0,0,TOnCloseEvent(this,&CEditorPreferences::OnClose),TProperties::plItemFolders|TProperties::plFullExpand|TProperties::plFullSort);
}
//---------------------------------------------------------------------------

void CEditorPreferences::OnDestroy()
{
    TProperties::DestroyForm(m_ItemProps);

	std::string fn;
	INI_NAME	(fn);
    CInifile* 	I = xr_new<CInifile>(fn.c_str(), FALSE, TRUE, TRUE);

    I->w_u32	("editor_prefs","device_flags",		psDeviceFlags.flags	);
    I->w_u32	("editor_prefs","sound_flags",		psSoundFlags.flags	);

    I->w_u32	("editor_prefs","tools_settings",	Tools->m_Settings.flags	);

    I->w_float	("editor_prefs","view_np",			view_np			);
    I->w_float	("editor_prefs","view_fp",			view_fp			);
    I->w_float	("editor_prefs","view_fov",			view_fov		);

    I->w_u32	("editor_prefs","fog_color",		fog_color		);
    I->w_float	("editor_prefs","fog_fogness",		fog_fogness		);

    I->w_float	("editor_prefs","cam_fly_speed",	cam_fly_speed	);
    I->w_float	("editor_prefs","cam_fly_alt",		cam_fly_alt		);
    I->w_float	("editor_prefs","cam_sens_rot",		cam_sens_rot	);
    I->w_float	("editor_prefs","cam_sens_move",	cam_sens_move	);

    I->w_float	("editor_prefs","tools_sens_rot",	tools_sens_rot	);
    I->w_float	("editor_prefs","tools_sens_move",	tools_sens_move	);
    I->w_float	("editor_prefs","tools_sens_scale",	tools_sens_scale);

    I->w_bool	("editor_prefs","bp_lim_depth",		bp_lim_depth	);
    I->w_bool	("editor_prefs","bp_lim_depth",		bp_cull			);
    I->w_float	("editor_prefs","bp_depth_tolerance",bp_depth_tolerance	);

    I->w_float	("editor_prefs","snap_angle",		snap_angle		);
    I->w_float	("editor_prefs","snap_move",		snap_move		);
    I->w_float	("editor_prefs","snap_moveto",		snap_moveto		);

    I->w_float	("editor_prefs","grid_cell_size",	grid_cell_size	);
    I->w_u32	("editor_prefs","grid_cell_count",	grid_cell_count	);

    I->w_u32	("editor_prefs","scene_undo_level",		scene_undo_level	);
    I->w_u32	("editor_prefs","scene_recent_count",	scene_recent_count	);
    I->w_bool	("editor_prefs","scene_leave_eo_copy",	scene_leave_eo_copy	);
    I->w_u32	("editor_prefs","scene_clear_color",	scene_clear_color 	);

    I->w_float	("editor_prefs","sound_rolloff",	sound_rolloff 	);

    I->w_u32	("editor_prefs","object_flags",		object_flags.flags);

    for (AStringIt it=scene_recent_list.begin(); it!=scene_recent_list.end(); it++){
    	AnsiString L; L.sprintf("recent_files_%d",it-scene_recent_list.begin());
    	AnsiString V; V.sprintf("\"%s\"",it->c_str());
		I->w_string("editor_prefs",L.c_str(),V.c_str());
    }

	xr_delete	(I);
}
//---------------------------------------------------------------------------

void CEditorPreferences::AppendRecentFile(LPCSTR name)
{
    for (AStringIt it=scene_recent_list.begin(); it!=scene_recent_list.end(); it++){
    	if (*it==name){
        	scene_recent_list.erase	(it);
            break;
        }
    }
	scene_recent_list.insert(scene_recent_list.begin(),name);
	while (scene_recent_list.size()>=EPrefs.scene_recent_count) 
    	scene_recent_list.pop_back();

    ExecCommand				(COMMAND_REFRESH_UI_BAR);
}
//---------------------------------------------------------------------------


#ifndef SceneSummaryInfoH
#define SceneSummaryInfoH
#pragma once

DEFINE_SET(AnsiString,AStringSet,AStringSetIt);
struct SSceneSummary{   	
	AStringSet	textures;
	AStringSet	det_textures;
	AStringSet	do_textures;
	AStringSet	lod_objects;
	AStringSet	mu_objects;
	AStringSet	waves;
	AStringSet	pe_static;
    int			face_cnt;
    int			vert_cnt;
    int			hom_face_cnt;
    int			hom_vert_cnt;
    int 		object_mu_ref_cnt;
    int 		object_lod_ref_cnt;
    int 		light_point_cnt;
    int 		light_spot_cnt;
    int 		light_dynamic_cnt;
    int 		light_static_cnt;
    int 		light_breakable_cnt;
    int 		light_procedural_cnt;
    int			glow_cnt;
    int			sector_cnt;
    int			portal_cnt;
    int 		sound_source_cnt;
    int 		pe_static_cnt;

    			SSceneSummary(){ Clear(); }
    void		FillProp	(PropItemVec& items);
    void		Clear(){
                    textures.clear		();
                    do_textures.clear	();
                    det_textures.clear	();
                    lod_objects.clear	();
                    mu_objects.clear	();
                    waves.clear			();
                    pe_static;
    				face_cnt			= 0;
                    vert_cnt			= 0;
                    hom_face_cnt		= 0;
                    hom_vert_cnt		= 0;
                    object_mu_ref_cnt	= 0;
                    object_lod_ref_cnt	= 0;
                    light_point_cnt		= 0;
                    light_spot_cnt		= 0;
                    light_dynamic_cnt	= 0;
                    light_static_cnt	= 0;
                    light_breakable_cnt	= 0;
                    light_procedural_cnt= 0;
                    glow_cnt			= 0;
                    sector_cnt			= 0;
                    portal_cnt			= 0;
                    sound_source_cnt	= 0;
                    pe_static_cnt		= 0;
    }
};
#endif

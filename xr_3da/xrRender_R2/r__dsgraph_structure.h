#pragma once

#include "..\render.h"
#include "..\ispatial.h"
#include "r__dsgraph_types.h"
#include "r__sector.h"

//////////////////////////////////////////////////////////////////////////
// feedback	for receiving visuals										//
//////////////////////////////////////////////////////////////////////////
class	R_feedback
{
public:
	virtual		void	rfeedback_static	(IRender_Visual*	V)		= 0;
};

//////////////////////////////////////////////////////////////////////////
// common part of interface implementation for all D3D renderers		//
//////////////////////////////////////////////////////////////////////////
class	R_dsgraph_structure										: public IRender_interface
{
public:
	IRenderable*												val_pObject;
	Fmatrix*													val_pTransform;
	BOOL														val_bHUD;
	BOOL														val_bInvisible;
	BOOL														val_bRecordMP;		// record nearest for multi-pass
	R_feedback*													val_feedback;		// feedback for geometry being rendered
	u32															val_feedback_breakp;// breakpoint
	u32															phase;
	u32															marker;
	bool														pmask		[2];
public:
	// Dynamic scene graph
	R_dsgraph::mapNormal_T										mapNormal	[2];	// 2==(priority/2)
	R_dsgraph::mapMatrix_T										mapMatrix	[2];
	R_dsgraph::mapSorted_T										mapSorted;
	R_dsgraph::mapHUD_T											mapHUD;
	R_dsgraph::mapLOD_T											mapLOD;
	R_dsgraph::mapSorted_T										mapDistort;

#if RENDER==R_R2
	R_dsgraph::mapSorted_T										mapEmissive;
#endif

	// Runtime structures 
	xr_vector<R_dsgraph::mapNormalVS::TNode*>					nrmVS;
	xr_vector<R_dsgraph::mapNormalPS::TNode*>					nrmPS;
	xr_vector<R_dsgraph::mapNormalCS::TNode*>					nrmCS;
	xr_vector<R_dsgraph::mapNormalStates::TNode*>				nrmStates;
	xr_vector<R_dsgraph::mapNormalTextures::TNode*>				nrmTextures;
	xr_vector<R_dsgraph::mapNormalTextures::TNode*>				nrmTexturesTemp;
	xr_vector<R_dsgraph::mapNormalVB::TNode*>					nrmVB;

	xr_vector<R_dsgraph::mapMatrixVS::TNode*>					matVS;
	xr_vector<R_dsgraph::mapMatrixPS::TNode*>					matPS;
	xr_vector<R_dsgraph::mapMatrixCS::TNode*>					matCS;
	xr_vector<R_dsgraph::mapMatrixStates::TNode*>				matStates;
	xr_vector<R_dsgraph::mapMatrixTextures::TNode*>				matTextures;
	xr_vector<R_dsgraph::mapMatrixTextures::TNode*>				matTexturesTemp;
	xr_vector<R_dsgraph::mapMatrixVB::TNode*>					matVB;

	xr_vector<R_dsgraph::_LodItem>								lstLODs;
	xr_vector<int>												lstLODgroups;
	xr_vector<ISpatial*>										lstRenderables;
	xr_vector<ISpatial*>										lstSpatial;
	xr_vector<IRender_Visual*>									lstVisuals;

	xr_vector<IRender_Visual*>									lstRecorded;

	u32															counter_S;
	u32															counter_D;
public:
	virtual		void					set_Transform			(Fmatrix*	M	)				{ VERIFY(M);	val_pTransform = M;	}
	virtual		void					set_HUD					(BOOL 		V	)				{ val_bHUD		= V;				}
	virtual		void					set_Invisible			(BOOL 		V	)				{ val_bInvisible= V;				}
				void					set_Feedback			(R_feedback*V, u32	id)			{ val_feedback_breakp = id; val_feedback = V;	}
				void					get_Counters			(u32&	s,	u32& d)				{ s=counter_S; d=counter_D;			}
				void					clear_Counters			()								{ counter_S=counter_D=0; 			}
public:
	R_dsgraph_structure	()
	{
		val_pObject			= NULL;
		val_pTransform		= NULL;
		val_bHUD			= FALSE;
		val_bInvisible		= FALSE;
		val_bRecordMP		= FALSE;
		val_feedback		= 0;
		val_feedback_breakp	= 0;
		marker				= 0;
		r_pmask				(true,true);
	};

	void		r_pmask											(bool _1, bool _2)				{ pmask[0]=_1; pmask[1]=_2;			}

	void		r_dsgraph_insert_dynamic						(IRender_Visual	*pVisual, Fvector& Center);
	void		r_dsgraph_insert_static							(IRender_Visual	*pVisual);

	void		r_dsgraph_render_graph							(u32	_priority,	bool _clear=true);
	void		r_dsgraph_render_hud							();
	void		r_dsgraph_render_lods							();
	void		r_dsgraph_render_sorted							();
	void		r_dsgraph_render_emissive						();
	void		r_dsgraph_render_distort						();
	void		r_dsgraph_render_subspace						(IRender_Sector* _sector, CFrustum* _frustum, Fmatrix& mCombined, Fvector& _cop, BOOL _dynamic	);
	void		r_dsgraph_render_subspace						(IRender_Sector* _sector, Fmatrix& mCombined, Fvector& _cop, BOOL _dynamic	);
	void		r_dsgraph_render_R1_box							(IRender_Sector* _sector, Fbox& _bb, int _element);
};

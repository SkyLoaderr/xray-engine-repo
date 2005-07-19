#ifndef PH_WORLD_H
#define PH_WORLD_H

// refs
struct SGameMtlPair;

class CPHMesh {
	dGeomID Geom;
public:
	dGeomID GetGeom(){return Geom;}
	void Create(dSpaceID space, dWorldID world);
	void Destroy();
};

#define PHWORLD_SOUND_CACHE_SIZE 8

////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class CPHWorld	: public pureFrame
#ifdef DEBUG
, public pureRender
#endif
{
	double						m_start_time												;
	u32							m_delay														;
	u32							m_previous_delay											;
	u32							m_reduce_delay												;
	u32							m_update_delay_count										;
	bool						b_world_freezed												;
	bool						b_processing;
	static const u32			update_delay=1												;
///	dSpaceID					Space														;

	CPHMesh						Mesh														;
	PH_OBJECT_STORAGE			m_objects													;
	PH_OBJECT_STORAGE			m_freezed_objects											;
	PH_OBJECT_STORAGE			m_recently_disabled_objects									;
	PH_UPDATE_OBJECT_STORAGE	m_update_objects											;
	dGeomID						m_motion_ray;
protected:
	ref_sound					m_sound_cache[PHWORLD_SOUND_CACHE_SIZE];
public:
	void						play_collide_sound(SGameMtlPair* mtl_pair, const Fvector& pos, float volume);
public:
	xr_vector<ISpatial*>		r_spatial;
public:
	u64							m_steps_num													;
	double						m_frame_sum													;
	dReal						m_frame_time												;
	float						m_update_time												;
	u16							disable_count												;
	
								CPHWorld						()							;
	virtual						~CPHWorld						(){}						;

//IC	dSpaceID					GetSpace						()			{return Space;}	;

	void						Create							()							;
	void						AddObject						(CPHObject* object)			;
	void						AddUpdateObject					(CPHUpdateObject* object)	;
	void						AddRecentlyDisabled				(CPHObject* object)			;
	void						RemoveFromRecentlyDisabled		(PH_OBJECT_I i)				;
	void						RemoveObject					(PH_OBJECT_I i)				;
	void						RemoveUpdateObject				(PH_UPDATE_OBJECT_I i)		;
	dGeomID						GetMeshGeom						()							{return Mesh.GetGeom();}
IC	dGeomID						GetMotionRayGeom				()							{return m_motion_ray;}
	void						Destroy							()							;

	void						FrameStep						(dReal step=0.025f)			;
	void						Step							()							;
	void						CutVelocity						(float l_limit, float a_limit);
	void 						Freeze							()							;
	void 						UnFreeze						()							;
	void						AddFreezedObject				(CPHObject* obj)			;
	void						RemoveFreezedObject				(PH_OBJECT_I i)				;
	bool 						IsFreezed						()							;
IC	bool						Processing						()							{return b_processing;}
	u32							CalcNumSteps					(u32 dTime)					;
	u16							ObjectsNumber					()							;
	u16							UpdateObjectsNumber				()							;
#ifdef DEBUG
	virtual void 				OnRender						()							;
#endif
	virtual void				OnFrame							()							;


};

#endif
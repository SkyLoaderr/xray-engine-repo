#ifndef PH_WORLD_H
#define PH_WORLD_H

//#define DRAW_CONTACTS


class CPHMesh {
	dGeomID Geom;
public:
	dGeomID GetGeom(){return Geom;}
	void Create(dSpaceID space, dWorldID world);
	void Destroy();
};


////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class CPHWorld	: public pureFrame
#ifdef DRAW_CONTACTS
, public pureRender
#endif
{
	double						m_start_time												;
	u32							m_delay														;
	u32							m_previous_delay											;
	u32							m_reduce_delay												;
	u32							m_update_delay_count										;
	bool						b_world_freezed												;
	static const u32			update_delay=1												;
///	dSpaceID					Space														;

	CPHMesh						Mesh														;
	PH_OBJECT_STORAGE			m_objects													;
	PH_OBJECT_STORAGE			m_freezed_objects											;
	PH_UPDATE_OBJECT_STORAGE	m_update_objects											;
	dGeomID						m_motion_ray;
public:
	xr_vector<ISpatial*>		r_spatial;
public:
	u64							m_steps_num													;
	double						m_frame_sum													;
	dReal						m_frame_time												;
	float						m_update_time												;
	unsigned int				disable_count												;

								CPHWorld						()							;
	virtual						~CPHWorld						()				{}			;

//IC	dSpaceID					GetSpace						()			{return Space;}	;

	void						Create							()							;
	void						AddObject						(CPHObject* object)			;
	void						AddUpdateObject					(CPHUpdateObject* object)	;

	void						RemoveObject					(PH_OBJECT_I i)				;
	void						RemoveUpdateObject				(PH_UPDATE_OBJECT_I i)		;
	dGeomID						GetMeshGeom						()							{return Mesh.GetGeom();}
IC	dGeomID						GetMotionRayGeom				()							{return m_motion_ray;}
	void						Destroy							()							;

	void						FrameStep						(dReal step=0.025f)			;
	void						Step							()							;
	void 						Freeze							()							;
	void 						UnFreeze						()							;
	void						AddFreezedObject				(CPHObject* obj)			;
	void						RemoveFreezedObject				(PH_OBJECT_I i)				;
	bool 						IsFreezed						()							;
	u32							CalcNumSteps					(u32 dTime)					;

#ifdef DRAW_CONTACTS
	virtual void 				OnRender						()							;
#endif
	virtual void				OnFrame							()							;


};

#endif
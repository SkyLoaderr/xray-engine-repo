#ifndef PH_DESTROYABLE
#define PH_DESTROYABLE

class CPhysicsShellHolder;

class CPHDestroyable
{
			shared_str					m_destroyed_obj_visual_name																												;
			Flags8						m_flags																																	;
			enum
			{
				fl_destroyable		= 1<<0,
				fl_destroyed		= 1<<1
			};
	virtual CPhysicsShellHolder*		PPhysicsShellHolder			()																											=0;
public:
										CPHDestroyable				()																											;
			void						Init						()																											;
			void						RespawnInit					()																											;
	
			void						Destroy						(u16 source_id=u16(-1),LPCSTR section="ph_skeleton_object")													;	

			void						Load						(LPCSTR section)																							;
			void						Load						(CInifile* ini,LPCSTR section)																				;
IC			bool						CanDestroy					()												{return m_flags.test(fl_destroyable)&&!m_flags.test(fl_destroyed);}
virtual		void						GenSpawnReplace				(u16 source_id,LPCSTR section)																								;
};


#endif

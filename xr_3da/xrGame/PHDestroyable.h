#ifndef PH_DESTROYABLE
#define PH_DESTROYABLE

class CPhysicsShellHolder;
class CSE_Abstract;
class CPHDestroyableNotificate;
class CPHDestroyableNotificator
{

public:
	virtual		void						NotificateDestroy			(CPHDestroyableNotificate *dn)=0;
};

class	CPHDestroyable :
public  CPHDestroyableNotificator

{
			xr_vector<shared_str>		m_destroyed_obj_visual_names																											;
			u16							m_depended_objects;
			Flags8						m_flags																																	;
			enum
			{
				fl_destroyable		= 1<<0,
				fl_destroyed		= 1<<1,
				fl_released			= 1<<2,
			};
	virtual CPhysicsShellHolder*		PPhysicsShellHolder			()																											=0;
public:
										CPHDestroyable				()																											;
			void						Init						()																											;
			void						RespawnInit					()																											;
	
			void						Destroy						(u16 ref_id=u16(-1),LPCSTR section="ph_skeleton_object")													;	

			void						Load						(LPCSTR section)																							;
			void						Load						(CInifile* ini,LPCSTR section)																				;
virtual		void						NotificateDestroy			(CPHDestroyableNotificate *dn);
IC			bool						CanDestroy					()												{return m_flags.test(fl_destroyable)&&!m_flags.test(fl_destroyed);}
virtual		bool						CanRemoveObject				(){return true;}
virtual		void						SheduleUpdate				(u32 dt);
virtual		void						GenSpawnReplace				(u16 source_id,LPCSTR section,shared_str visual_name)														;
virtual		void						InitServerObject			(CSE_Abstract*				D)																				;
};


#endif

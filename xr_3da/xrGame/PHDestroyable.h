#ifndef PH_DESTROYABLE
#define PH_DESTROYABLE

class CPhysicsShellHolder;

class CPHDestroyable
{
			ref_str						m_destroyed_obj_visual_name	;
			Flags8						m_flags						;
			enum
			{
				fl_destroyable		= 1<<0,
				fl_destroyed		= 1<<1
			};
	virtual CPhysicsShellHolder*		PPhysicsShellHolder			()						=0;
public:
										CPHDestroyable				()						;
			void						Init						()						;
			void						RespawnInit					()						;
			u8							Destroy						()						;	
			void						Load						(LPCSTR section)		;
};


#endif

#ifndef PH_DESTROYABLE
#define PH_DESTROYABLE

class CPhysicsShellHolder;

class CPHDestroyable
{
			ref_str						m_destroyed_obj_visual_name	;
			bool						b_canbe_destroyed			;//temporarely for objects wich do not have desroyed model
protected:
	virtual CPhysicsShellHolder*		PPhysicsShellHolder			()						=0;
public:
										CPHDestroyable				()						;
			void						Init						()						;
			void						Destroy						()						;	
			void						Load						(LPCSTR section)		;
};


#endif

#pragma once
#include "control_combase.h"

#include "anim_triple.h"
#include "control_jump.h"


class CAnimationSequencer;
class CControlRotationJump;



class CControlManagerCustom : public CControl_ComBase {
	CAnimationSequencer		*m_sequencer;
	CAnimationTriple		*m_triple_anim;

	CControlRotationJump	*m_rotation_jump;
	CControlJump			*m_jump;

public:
					CControlManagerCustom	();
					~CControlManagerCustom	();

	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);
	virtual void	on_start_control		(ControlCom::EContolType type);
	virtual void	on_stop_control			(ControlCom::EContolType type);
	virtual void	update_frame			();
	virtual void	update_schedule			();

			void	add_ability				(ControlCom::EContolType);

	//-------------------------------------------------------------------------------
	// Sequencer
	void		seq_init				();
	void		seq_add					(MotionID motion);
	void		seq_switch				();					// Перейти в следующее состояние, если такового не имеется - завершить
	void		seq_run					(MotionID motion);

	//-------------------------------------------------------------------------------
	// Triple Animation
	void		ta_activate				(const SAnimationTripleData &data);
	void		ta_pointbreak			();
	void		ta_fill_data			(SAnimationTripleData &data, LPCSTR s1, LPCSTR s2, LPCSTR s3, bool execute_once, bool skip_prep, u32 capture_type = ControlCom::eCaptureDir | ControlCom::eCapturePath | ControlCom::eCaptureMovement);
	bool		ta_is_active			();
	void		ta_deactivate			();
	
	//-------------------------------------------------------------------------------
	// Jump
	void		jump					(CObject *obj, const SControlJumpData &ta);
	void		jump					(const SControlJumpData &ta);
	void		load_jump_data			(LPCSTR s1, LPCSTR s2, LPCSTR s3, u32 vel_mask);
	
	bool		jump					(CObject *obj);
private:

	void		check_attack_jump		();
	void		check_jump_over_physics	();
};


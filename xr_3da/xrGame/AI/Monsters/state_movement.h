#pragma once
#include "state.h"

template<typename _Object>
class CStateMovement : public CState <_Object> {
	typedef CStateMovement<_Object> inherited;

	enum EPathState {
		ePathStateUndefined,	// ��������� �� ����������
		ePathStateOutdated,		// � ������ ��������� ���� �� ��� ��������
		ePathStateNotReady,		// � ������ ��������� ���� ��� ��� ����� �� �� ��� ��������
		ePathStateBuilt,		// � ������ ��������� ���� ��������
	} m_path_state;
	
	bool		m_path_targeted;	// ����������� ���� ���� � m_target 

	struct {
		Fvector	position;
		u32		node;
	} m_target;

public:
						CStateMovement			(_Object *obj, EStateType s_type = ST_Unknown, void *data = 0);
	virtual 			~CStateMovement			(); 

	virtual void		initialize				();
	virtual void		path_update				();
	
	// ���� �� ����� ���� ��������
	virtual	void		path_failed				() {}
	// �������� ����� ����
	virtual void		path_end				() {}
	// ������� ����
	virtual	void		select_target_point		() {}
			
protected:
			EPathState	path_state				() {return m_path_state;}
private:

			void		update_path_info		();

};
#include "state_movement_inline.h"



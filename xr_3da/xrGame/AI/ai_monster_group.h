#pragma once

class CEntity;

// ���� ������
enum ESquadCommand {
	SC_EXPLORE,
	SC_ATTACK,
	SC_THREATEN,
	SC_COVER,
	SC_FOLLOW,
	SC_FEEL_DANGER,
	SC_NONE
};

// ���� ��������� ������
enum ESquadTaskState {
	TS_REQUEST,		// ������ �� ���������� ������
	TS_PROGRESS,	// ������ � �������� ����������
	TS_REFUSED,		// � ���������� ������ - ��������
};

// ��������� ������
enum ELeaderState {
	LS_IDLE,
	LS_EAT,
	LS_WANDER,
	LS_ATTACK, 
	LS_DEFENCE
};

// ������
struct GTask {
	struct {
		ESquadCommand		command;
		ESquadTaskState		type;
		u32					ttl;			// �����, �� �������� ��������� ��������� ��������
		bool				need_reset;		// ���� type = TS_REQUEST, � need_reset=true - ������ ������ �������� ���������� ���������
	} state;

	struct {
		Fvector				position;
		u32					node;
		CObject				*entity;
	} target;
};

struct SMemberEnemy {
	CEntity *member;
	CEntity *enemy;
};

/////////////////////////////////////////////////////////////////////////////////////////
// MonsterSquad Class
class CMonsterSquad {

	CEntity				*leader;
	ELeaderState		leader_state;
	
public:
				CMonsterSquad() : leader(0) {}
				~CMonsterSquad() {}
	
	// -----------------------------------------------------------------

	void		RegisterMember		(CEntity *pE);
	void		RemoveMember		(CEntity *pE);

	bool		SquadActive			();

	// -----------------------------------------------------------------

	void		SetLeader			(CEntity *pE) {leader = pE;}
	CEntity		*GetLeader			() {return leader;}
	void		SetLeaderState		(ELeaderState ls) {leader_state = ls;}

	///////////////////////////////////////////////////////////////////////////////////////
	//  ����� ������� ��������
	//////////////////////////////////////////////////////////////////////////////////////

	void		UpdateMonsterData	(CEntity *pE, CEntity *pEnemy);
	void		UpdateDecentralized	();
	Fvector		GetTargetPoint		(CEntity *pE, u32 &time);
	
	struct SEntityState {
		CEntity *pEnemy;		// ������� ����. ����������� �� ������� NPC
		Fvector target;			// �������������� ������� ����������� �� ������� Squad (��� ������ �������� �����������)
		u32		last_updated;	// ����� ���������� ���������� pEnemy �� ������� NPC
		u32		last_repos;		// ����� ���������� ���������� target �� ������� Squad (���� 0 - �� �� ����� target)
	};

	DEFINE_VECTOR		(CEntity*, ENTITY_VEC,	ENTITY_VEC_IT);
	DEFINE_MAP			(CEntity*, SEntityState, ENTITY_STATE_MAP, ENTITY_STATE_MAP_IT);
	DEFINE_VECTOR		(SMemberEnemy, MEMBER_ENEMY_VEC, MEMBER_ENEMY_VEC_IT);

	ENTITY_STATE_MAP	states;
	MEMBER_ENEMY_VEC	vect_copy;
	MEMBER_ENEMY_VEC	general_enemy;		
	ENTITY_VEC			members;

	struct _elem {
		CEntity *pE;
		Fvector p_from;
		float	yaw;
	};

	xr_vector<_elem>	lines;


	void		SetupMemeberPositions_Deviation (MEMBER_ENEMY_VEC &members, CEntity *enemy);
	void		SetupMemeberPositions_TargetDir (MEMBER_ENEMY_VEC &members, CEntity *enemy);


	//////////////////////////////////////////////////////////////////////////////////////
};


class CMonsterSquadManager {
	DEFINE_VECTOR(CMonsterSquad*, MONSTER_SQUAD_VEC, MONSTER_SQUAD_VEC_IT);
	DEFINE_VECTOR(MONSTER_SQUAD_VEC, MONSTER_TEAM_VEC,MONSTER_TEAM_VEC_IT);

	MONSTER_TEAM_VEC team;

public:
					CMonsterSquadManager	();
					~CMonsterSquadManager	();

	void			register_member			(u8 team_id, u8 squad_id, CEntity *e);
	void			remove_member			(u8 team_id, u8 squad_id, CEntity *e);

	CMonsterSquad	*get_squad				(u8 team_id, u8 squad_id);
	CMonsterSquad	*get_squad				(const CEntity *entity);

};


IC CMonsterSquadManager &monster_squad();

#include "ai_monster_group_inline.h"

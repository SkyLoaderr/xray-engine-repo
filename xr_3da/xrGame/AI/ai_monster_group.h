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

struct SEntityState {
	CEntity *pEnemy;
	Fvector target_pos;
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
		Fvector				pos;
		CObject				*entity;
		u32					node;
	} target;
};


/////////////////////////////////////////////////////////////////////////////////////////
// MonsterSquad Class
class CMonsterSquad {
	DEFINE_MAP		(CEntity*, GTask,	SQUAD_MAP,	SQUAD_MAP_IT);
	DEFINE_VECTOR	(CEntity*, ENTITY_VEC,	ENTITY_VEC_IT);
	DEFINE_MAP		(CEntity*, SEntityState, ENTITY_STATE_MAP, ENTITY_STATE_MAP_IT);

	CEntity				*leader;
	ELeaderState		leader_state;
	
	SQUAD_MAP			squad;
	
	u8					id;

	ENTITY_STATE_MAP	states;

public:
				CMonsterSquad(u8 i) : id(i), leader(0) {}
				~CMonsterSquad() {}
	
	// -----------------------------------------------------------------

	void		RegisterMember		(CEntity *pE);
	void		RemoveMember		(CEntity *pE);

	// -----------------------------------------------------------------

	void		SetLeader			(CEntity *pE) {leader = pE;}
	CEntity		*GetLeader			() {return leader;}
	void		SetLeaderState		(ELeaderState ls) {leader_state = ls;}

	// -----------------------------------------------------------------

	// ���������������� �������� �������
	void		ProcessGroupIntel	();
	void		ProcessGroupIntel	(const GTask &task);

	// -----------------------------------------------------------------
	
	// ������������������ ����������
	void		UpdateMonsterData	(CEntity *pE, CEntity *pEnemy);
	void		UpdateDecentalized	();
	Fvector		GetTargetPoint		(CEntity *pE);

	// -----------------------------------------------------------------

	// �������� ������ ��� NPC
	GTask		&GetTask			(CEntity *pE);

	// �������� id ������ squad
	u8			GetID				() {return id;}

	// DEBUG
	void		Dump				();

private:
	ENTITY_VEC	enemies;
	ENTITY_VEC	members;
	u32			dest_node;

	void		AskMember			(CEntity *pE, const GTask &new_task);

private:
	void		TaskIdle			();
	void		Explore				(xr_vector<u32> &nodes, const Fvector &centroid, const Fvector &dir, CEntity *pE, GTask *pTask);
	bool		IsPriorityHigher	(ESquadCommand com_new, ESquadCommand com_old);
	
	void		InitTask			(GTask *task);
	bool		ActiveTask			(GTask *task);


	CEntity		*GetNearestEnemy	(CEntity *t, ENTITY_VEC *ev);
	
};


class CSquadManager {
	DEFINE_MAP(u8, CMonsterSquad*, SQUADS_MAP, SQUADS_MAP_IT);
	SQUADS_MAP squads;

public:
					CSquadManager	();
					~CSquadManager	();
		
	void			RegisterMember	(u8 squad_id, CEntity *e);
	void			RemoveMember	(u8 squad_id, CEntity *e);

	//-------------------------------------------------------------------

	CMonsterSquad	*GetSquad		(u8 squad_id);

	//-------------------------------------------------------------------
	// Utilities
	
	u8				TransformPriority(ESquadCommand com);
	
	//-------------------------------------------------------------------

	void			Dump			();
};



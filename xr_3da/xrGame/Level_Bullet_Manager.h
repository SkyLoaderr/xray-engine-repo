// Level_Bullet_Manager.h:  для обеспечения полета пули по траектории
//							все пули и осколки передаются сюда
//////////////////////////////////////////////////////////////////////

#pragma once


#include "weaponammo.h"


//структура, описывающая пулю и ее свойства в полете
struct SBullet
{
	SBullet(const Fvector& position,
			const Fvector& direction,
			float start_speed,
			float power,
			float impulse,
			u16	sender_id,
			ALife::EHitType e_hit_type,
			const CCartridge& cartridge);
	~SBullet();


	//номер кадра на котором была запущена пуля
	u32				frame_num;

	//текущая позиция
	Fvector			pos;
	//текущая скорость
	Fvector			dir;
	float			speed;
	
	float			hit_power;
	float			hit_impulse;

	//ID персонажа который иницировал действие
	u16				parent_id;		

	//коэфициенты и параметры патрона
	float			dist_k;
	float			hit_k;
	float			impulse_k;
	float			pierce_k;
	bool			tracer;
	float			wallmark_size;
	//тип наносимого хита
	ALife::EHitType hit_type;

	//предыдущая позиция
	Fvector			prev_pos;
};



class CLevel;



class CBulletManager: public pureRender
{
	friend CLevel;
public:
	CBulletManager();
	virtual ~CBulletManager();

	void Clear		();

	void AddBullet	(SBullet* pBullet);
	void Update		();

	virtual void OnRender ();

protected:

	//функция обработки хитов объектов
	static BOOL __stdcall firetrace_callback(Collide::rq_result& result, LPVOID params);
	//попадание по динамическому объекту
	void DynamicObjectHit	(SBullet* bullet, const Fvector& end_point, 
							Collide::rq_result& R, 	u16 target_material);
	//попадание по статическому объекту
	void StaticObjectHit	(SBullet* bullet, const Fvector& end_point, 
							Collide::rq_result& R, u16 target_material);
	//отметка на пораженном объекте
	void FireShotmark		(const SBullet* bullet, const Fvector& vDir, 
							 const Fvector &vEnd, Collide::rq_result& R, 
							 u16 target_material);


	//просчет полета пули за некоторый промежуток времени
	//принимается что на этом участке пуля движется прямолинейно
	//и равномерно, а после просчета также изменяется текущая
	//скорость и положение с учетом гравитации и ветра
	//возвращаем true если пуля продолжает полет
	bool CalcBullet (SBullet* bullet, u32 delta_time);


	DEF_LIST(BULLET_LIST, SBullet*);
	//список пуль находящихся в данный момент на уровне
	BULLET_LIST m_BulletList;

	//остаток времени, который не был учтен на предыдущем кадре
	u32	m_dwTimeRemainder;
	//фиксированное время шага просчета пули
	u32	m_dwStepTime;


	//материал для пули и осколков (инициализируется в CLevel::Load)
	u16 bullet_material_idx;
};
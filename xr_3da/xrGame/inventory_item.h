////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_item.h
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Victor Reutsky, Yuri Dobronravin
//	Description : Inventory item
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "inventory_space.h"
#include "physic_item.h"
#include "phsynchronize.h"
#include "entitycondition.h"
#include "hit_immunity.h"

enum EHandDependence{
	hdNone	= 0,
	hd1Hand	= 1,
	hd2Hand	= 2
};

class CInventoryItem : public CPhysicItem, public CHitImmunity
{
	typedef CPhysicItem	inherited;
public:
					CInventoryItem		();
	virtual			~CInventoryItem		();

public:
	virtual CInventoryItem*		cast_inventory_item		()		 {return this;}

public:
	virtual void	Load				(LPCSTR section);

	virtual LPCSTR	Name				();
	virtual LPCSTR	NameShort			();
	virtual LPCSTR	NameComplex			();
	shared_str		ItemDescription		() { return m_Description; }

	
	virtual void	OnEvent				(NET_Packet& P, u16 type);
	
	virtual bool	Useful				() const;									// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool	Attach				(PIItem pIItem) {return false;}
	virtual bool	Detach				(PIItem pIItem) {return false;}
	//при детаче спаунится новая вещь при заданно названии секции
	virtual bool	Detach				(const char* item_section_name);
	//проверяет может ли элемент быть присоединен
	//не производя самого действия
	virtual bool	CanAttach			(PIItem pIItem) {return false;}
	virtual bool	CanDetach			(LPCSTR item_section_name) {return false;}

	virtual EHandDependence		HandDependence		()	const	{return hd1Hand;};
	virtual bool	Activate			();									// !!! Переопределить. (см. в Inventory.cpp)
	virtual void	Deactivate			();								// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool	Action				(s32 cmd, u32 flags) {return false;}	// true если известная команда, иначе false

	// вещь спрятано в инвентаре
	virtual bool	IsHidden		()	const	{return true;}
	//вещь убирается в инвентарь
	virtual bool	IsHiding		()	const	{return false;}
	//вызывается при завершении анимации
	virtual void	OnAnimationEnd	()		{}
	
	virtual s32		Sort				(PIItem pIItem);						// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool	Merge				(PIItem pIItem);						// !!! Переопределить. (см. в Inventory.cpp)

	virtual void	OnH_B_Chield		();
	virtual void	OnH_A_Chield		();
	virtual void	OnH_B_Independent	();

	virtual void	save				(NET_Packet &output_packet);
	virtual void	load				(IReader &input_packet);
	virtual BOOL	net_SaveRelevant	()								{return TRUE;}


	virtual void	UpdateCL			();

	virtual	void	Hit					(	float P, Fvector &dir,	
											CObject* who, s16 element,
											Fvector position_in_object_space, 
											float impulse, 
											ALife::EHitType hit_type = ALife::eHitTypeWound);

			void	Drop				();										// Если объект в инвенторе, то он будет выброшен

			u32		Cost				() const	{return m_cost;}
			float	Weight				() const	{return m_weight;}		

public:
	// Указатель на инвентарь. Всегда полезно знать где находишься :)
	CInventory*		m_pInventory;
	shared_str			m_name;
	shared_str			m_nameShort;
	char			m_nameComplex[255];
	bool			m_drop;
	//текущее положение вещи в инвентаре
	EItemPlace		m_eItemPlace;


	virtual void	OnMoveToSlot		() {};
	virtual void	OnMoveToBelt		() {};
	virtual void	OnMoveToRuck		() {};

	virtual int		GetGridWidth		() const {return m_iGridWidth;}
	virtual int		GetGridHeight		() const {return m_iGridHeight;}
	virtual int		GetXPos				() const {return m_iXPos;}
	virtual int		GetYPos				() const {return m_iYPos;}

			float	GetCondition		()  const {return m_fCondition;}
			void	ChangeCondition		(float fDeltaCondition);

	virtual u32		GetSlot				()  const {return m_slot;}
			void	SetSlot				(u32 slot) {m_slot = slot;}

			bool	Belt				() {return m_belt;}
			void	Belt				(bool on_belt) {m_belt = on_belt;}
			bool	Ruck				() {return m_ruck;}
			void	Ruck				(bool on_ruck) {m_ruck = on_ruck;}
	
			
protected:
	// Слот в который можно установить объект (NO_ACTIVE_SLOT если нельзя)
	u32				m_slot;
	// Может ли объект быть на поясе или в рюкзаке
	bool			m_belt, m_ruck;			

	// цена по умолчанию
	u32				m_cost;
	// вес объекта (без подсоединненых вещей)
	float			m_weight;
	
	//состояние вещи, 1.0 - полностью работоспособная
	// 0 - испорченная
	float			m_fCondition;
	//флаг использования состояния для вещи
	bool			m_bUsingCondition;

	int				m_iGridWidth;										//ширина в сетке инвенторя
	int				m_iGridHeight;										//высота в сетке инвенторя

	int 			m_iXPos;											//позиция X в сетке инвенторя
	int 			m_iYPos;											//позиция Y в сетке инвенторя
	// Тектс описания вещи
	shared_str			m_Description;

	////////// network //////////////////////////////////////////////////
public:
	virtual void	make_Interpolation	();
	virtual void	PH_B_CrPr			(); // actions & operations before physic correction-prediction steps
	virtual void	PH_I_CrPr			(); // actions & operations after correction before prediction steps
#ifdef DEBUG
	virtual void	PH_Ch_CrPr			(); // 
#endif
	virtual void	PH_A_CrPr			(); // actions & operations after phisic correction-prediction steps

	virtual void	net_Import			(NET_Packet& P);					// import from server
	virtual void	net_Export			(NET_Packet& P);					// export to server

	virtual void	activate_physic_shell	();
protected:
	virtual void	UpdateXForm	();

protected:
	struct net_update_IItem {
		u32					dwTimeStamp;
		SPHNetState			State;
	};

	xr_deque<net_update_IItem>	NET_IItem;
	/////////////////////////////////////////////
	/// spline coeff /////////////////////
	float			SCoeff[3][4];

#ifdef DEBUG
	DEF_VECTOR		(VIS_POSITION, Fvector);
	VIS_POSITION	LastVisPos;
#endif

	Fvector			IStartPos;
	Fquaternion		IStartRot;

	Fvector			IRecPos;
	Fquaternion		IRecRot;

	Fvector			IEndPos;
	Fquaternion		IEndRot;	

	SPHNetState		LastState;
	SPHNetState		RecalculatedState;

#ifdef DEBUG
	SPHNetState		CheckState;
#endif
	SPHNetState		PredictedState;

	bool			m_bInInterpolation		;
	bool			m_bInterpolate			;
	u32				m_dwIStartTime			;
	u32				m_dwIEndTime			;
	u32				m_dwILastUpdateTime		;

	void			CalculateInterpolationParams();

private:
	u64				m_inventory_mask;
	u32				m_dwFrameLoad;
	u32				m_dwFrameReload;
	u32				m_dwFrameReinit;
	u32				m_dwFrameSpawn;
	u32				m_dwFrameDestroy;
	u32				m_dwFrameClient;

protected:
	bool			m_useful_for_NPC;

public:
	IC		int		GetVolume				() const;
	IC		int		GetHeight				() const;
	IC		int		GetWidth				() const;
	virtual BOOL	net_Spawn				(LPVOID DC);
	virtual void	net_Destroy				();
	virtual void	renderable_Render		();
	virtual void	reload					(LPCSTR section);
	virtual void	reinit					();
	virtual bool	can_kill				() const;
	virtual CInventoryItem *can_kill		(CInventory *inventory) const;
	virtual const CInventoryItem *can_kill	(const xr_vector<const CGameObject*> &items) const;
	virtual CInventoryItem *can_make_killing(const CInventory *inventory) const;
	virtual bool	ready_to_kill			() const;
	IC		bool	useful_for_NPC			() const;
#ifdef DEBUG
	virtual void			OnRender			();
#endif
};


#include "inventory_item_inline.h"
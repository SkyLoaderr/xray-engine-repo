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

class CInventoryItem : public CPhysicItem {
	typedef CPhysicItem	inherited;
public:
					CInventoryItem		();
	virtual			~CInventoryItem		();

	virtual void	Load				(LPCSTR section);

	virtual LPCSTR	Name				();
	virtual LPCSTR	NameShort			();
	virtual LPCSTR	NameComplex			();

	
	virtual void	OnEvent				(NET_Packet& P, u16 type);
	
	virtual bool	Useful				() const;									// !!! ��������������. (��. � Inventory.cpp)
	virtual bool	Attach				(PIItem pIItem) {return false;}
	virtual bool	Detach				(PIItem pIItem) {return false;}
	//��� ������ ��������� ����� ���� ��� ������� �������� ������
	virtual bool	Detach				(const char* item_section_name);
	//��������� ����� �� ������� ���� �����������
	//�� ��������� ������ ��������
	virtual bool	CanAttach			(PIItem pIItem) {return false;}
	virtual bool	CanDetach			(LPCSTR item_section_name) {return false;}

	virtual bool	Activate			();									// !!! ��������������. (��. � Inventory.cpp)
	virtual void	Deactivate			();								// !!! ��������������. (��. � Inventory.cpp)
	virtual bool	Action				(s32 cmd, u32 flags) {return false;}	// true ���� ��������� �������, ����� false

	// ���� �������� � ���������
	virtual bool	IsHidden		()	const	{return true;}
	//���� ��������� � ���������
	virtual bool	IsHiding		()	const	{return false;}
	//���������� ��� ���������� ��������
	virtual void	OnAnimationEnd	()		{}
	
	virtual s32		Sort				(PIItem pIItem);						// !!! ��������������. (��. � Inventory.cpp)
	virtual bool	Merge				(PIItem pIItem);						// !!! ��������������. (��. � Inventory.cpp)

	virtual void	OnH_B_Chield		();
	virtual void	OnH_A_Chield		();
	virtual void	OnH_B_Independent	();

	virtual void	UpdateCL			();

	virtual	void	Hit					(	float P, Fvector &dir,	
											CObject* who, s16 element,
											Fvector position_in_object_space, 
											float impulse, 
											ALife::EHitType hit_type = ALife::eHitTypeWound);

			bool	DetachAll			();										// ��������� �������� ��������. ������ ������ ���� � �������
			void	Drop				();										// ���� ������ � ���������, �� �� ����� ��������

			u32		Cost				() const	{return m_cost;}
			float	Weight				() const	{return m_weight;}		

public:
	CInventory		*m_pInventory;								// ��������� �� ���������. ������ ������� ����� ��� ���������� :)
	LPCSTR			m_name;
	LPCSTR			m_nameShort;
	char			m_nameComplex[255];
	bool			m_drop;
	//������� ��������� ���� � ���������
	EItemPlace		m_eItemPlace;


	virtual int		GetGridWidth		() const {return m_iGridWidth;}
	virtual int		GetGridHeight		() const {return m_iGridHeight;}
	virtual int		GetXPos				() const {return m_iXPos;}
	virtual int		GetYPos				() const {return m_iYPos;}

			float	GetCondition		()  const {return m_fCondition;}
			void	ChangeCondition		(float fDeltaCondition);

			u32		GetSlot				()  const {return m_slot;}
			void	SetSlot				(u32 slot) {m_slot = slot;}

			bool	Belt				() {return m_belt;}
			void	Belt				(bool on_belt) {m_belt = on_belt;}
			bool	Ruck				() {return m_ruck;}
			void	Ruck				(bool on_ruck) {m_ruck = on_ruck;}
protected:
	// ���� � ������� ����� ���������� ������ (NO_ACTIVE_SLOT ���� ������)
	u32				m_slot;
	// ����� �� ������ ���� �� ����� ��� � �������
	bool			m_belt, m_ruck;			

	// ���� �� ���������
	u32				m_cost;
	// ��� ������� (��� �������������� �����)
	float			m_weight;
	
	//��������� ����, 1.0 - ��������� ���������������
	// 0 - �����������
	float			m_fCondition;
	//���� ������������� ��������� ��� ����
	bool			m_bUsingCondition;

	//������������ ��������� ����� ����� (�������� ������ ��� ��� ����)
	//������������ ����
	HitTypeSVec		m_HitTypeK;

	int				m_iGridWidth;										//������ � ����� ���������
	int				m_iGridHeight;										//������ � ����� ���������

	int 			m_iXPos;											//������� X � ����� ���������
	int 			m_iYPos;											//������� Y � ����� ���������

	////////// network //////////////////////////////////////////////////
public:
	virtual void	make_Interpolation	();
	virtual void	PH_B_CrPr			(); // actions & operations before physic correction-prediction steps
	virtual void	PH_I_CrPr			(); // actions & operations after correction before prediction steps
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
	bool			m_bHasUpdate;

	Fvector			IStartPos;
	Fquaternion		IStartRot;

	Fvector			IEndPos;
	Fquaternion		IEndRot;

	SPHNetState		RecalculatedState;
	SPHNetState		PredictedState;

	bool			m_bInInterpolation		;
	bool			m_bInterpolate			;
	u32				m_dwIStartTime			;
	u32				m_dwIEndTime			;
	u32				m_dwILastUpdateTime		;

private:
	u64				m_inventory_mask;
	u32				m_dwFrameLoad;
	u32				m_dwFrameReload;
	u32				m_dwFrameReinit;
	u32				m_dwFrameSpawn;
	u32				m_dwFrameDestroy;
	u32				m_dwFrameClient;

public:
	virtual void	BuildInventoryMask		(const CInventory *inventory);
	IC		u64		InventoryMask			() const;
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
	virtual const CInventoryItem *can_kill	(const xr_set<const CGameObject*> &items) const;
	virtual CInventoryItem *can_make_killing(const CInventory *inventory) const;
	virtual bool	ready_to_kill			() const;
};

#include "inventory_item_inline.h"
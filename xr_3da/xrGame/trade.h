#pragma once

class CInventoryOwner;
class CInventory;
class CInventoryItem;
class CEntity;

struct TradeFactors {
	float   fBuyFactorHostile;
	float   fBuyFactorFriendly;
	float   fSellFactorHostile;
	float   fSellFactorFriendly;

	bool	Loaded;

	TradeFactors(){Loaded = false;}
};


class CTrade {
	bool	TradeState;					// ����� ��������. true - �������
	u32		m_dwLastTradeTime;			

	typedef enum tagTraderType {
		TT_NONE,
		TT_TRADER,
		TT_STALKER,
		TT_ACTOR,
	} EOwnerType;

	struct SInventoryOwner {
		EOwnerType		type;
		CEntity			*base;
		CInventoryOwner	*inv_owner;

		void Set (EOwnerType t, CEntity	*b, CInventoryOwner *io) { type = t; base = b; inv_owner = io;}
	};

	//���� ����� �������� ������������� � �������� ��� ���������
	bool	m_bNeedToUpdateArtefactTasks;
public:
	static TradeFactors m_tTradeFactors;

public:
	SInventoryOwner	pThis;
	SInventoryOwner pPartner;

public:
	
	CTrade(CInventoryOwner	*p_io);
	~CTrade();

	

	bool		CanTrade();								// ���������, ����� �� ������ ������ ��������� � ������������� pPartner
	
	void		StartTrade(CInventoryOwner* pInvOwner);
	void		StartTrade();
	void		StopTrade();
	bool		IsInTradeState() {return TradeState;}

	void		Communicate();
	
	bool		OfferTrade(SInventoryOwner man);					// man ���������� ���������
	
	void		SellItem(int id);
	void		SellItem(CInventoryItem* pItem);

	CInventoryOwner*	GetPartner();	
	CTrade*				GetPartnerTrade();
	CInventory*			GetPartnerInventory();

	//���������� ���� �������� � ������ ��������� ����������� ��������.
	u32			GetItemPrice(CInventoryItem* pItem);

	void		UpdateTrade();
private:
	bool		SetPartner(CEntity *p);
	void		RemovePartner();

	// ����� ��������� ��� �������� (���� TRADER, �� ��������� ����� ammunition_depot)
	CInventory	&GetTradeInv(SInventoryOwner owner);
};
#pragma once

class CInventoryOwner;


struct TradeFactors {
	float	TraderBuyPriceFactor;
	float	TraderSellPriceFactor;
	float	StalkerNeutralBuyPriceFactor;
	float	StalkerNeutralSellPriceFactor;
	float	StalkerFriendBuyPriceFactor;
	float	StalkerFriendSellPriceFactor;

	bool	Loaded;

	TradeFactors(){Loaded = false;}
};


class CTrade {
	bool	TradeState;					// режим торговли. true - включен
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

public:
	static TradeFactors m_tTradeFactors;

public:
	SInventoryOwner	pThis;
	SInventoryOwner pPartner;

public:
	
	CTrade(CInventoryOwner	*p_io);
	~CTrade();

	

	bool		CanTrade();								// Проверяет, может ли данный объект торговать и устанавливает pPartner
	
	void		StartTrade();
	void		StopTrade();
	bool		IsInTradeState() {return TradeState;}

	void		Communicate();
	
	bool		OfferTrade(SInventoryOwner man);					// man предлагает торговать
	
	void		ShowItems();
	void		ShowMoney();
	void		ShowArtifactPrices();
	void		SellItem(int id);

	void		UpdateTrade();
private:
	bool		SetPartner(CEntity *p);
	void		RemovePartner();

	// выбор инвентаря для торговли (если TRADER, то торговать через ammunition_depot)
	CInventory *GetTradeInv(SInventoryOwner owner);				
};
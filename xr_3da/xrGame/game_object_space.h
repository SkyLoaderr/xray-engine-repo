#pragma once

namespace GameObject {
	enum ECallbackType {
		eTradeStart,
		eTradeStop,
		eTradeSellBuyItem,
		eTradePerformTradeOperation,

		eZoneEnter,
		eZoneExit,

		eDeath,

		ePatrolPathInPoint,

		eInventoryPda,
		eInventoryInfo,

		eUseObject,

		eHit,

		eSound,

		eActionTypeMovement,
		eActionTypeWatch,
		eActionTypeAnimation,
		eActionTypeSound,
		eActionTypeParticle,
		eActionTypeObject,

		eActorSleep,

		eHelicopterOnPoint,
		eHelicopterOnHit,

		ePatrolExtrapolateCallback,
		
		eEnemyCallback,
	};
};

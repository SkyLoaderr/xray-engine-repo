////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_anomaly_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife anomaly registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_known_anomaly.h"
#include "object_broker.h"

class CALifeAnomalyRegistry {
protected:
	ALife::ANOMALY_P_VECTOR_VECTOR					m_anomalies;
	ALife::ANOMALY_P_VECTOR_SVECTOR					m_anomaly_cross_table;

public:
	IC												CALifeAnomalyRegistry	(LPCSTR section);
	virtual											~CALifeAnomalyRegistry	();
	virtual	void									save					(IWriter &memory_stream);
	virtual	void									load					(IReader &file_stream);
	IC		void									clear					();
	IC		void									add						(CALifeKnownAnomaly *anomaly);
	IC		const ALife::ANOMALY_P_VECTOR_SVECTOR	&cross					() const;
};

#include "alife_anomaly_registry_inline.h"
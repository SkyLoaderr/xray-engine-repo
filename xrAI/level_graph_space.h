////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph_space.h
//	Created 	: 02.10.2001
//  Modified 	: 08.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Level graph space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace LevelGraph {
	class CHeader : private hdrNODES {
	private:
		friend class CRenumbererConverter;

	public:
		IC	u32				version					() const;
		IC	u32				vertex_count			() const;
		IC	float			cell_size				() const;
		IC	float			factor_y				() const;
		IC	const Fbox		&box					() const;
	};

	typedef NodePosition	CPosition;

	class CVertex : private NodeCompressed {
	private:
		friend class CRenumbererConverter;

	public:
		IC	u32				link					(int i) const;
		IC	u8				light					() const;
		IC	u16				cover					(u8 index) const;
		IC	u16				plane					() const;
		IC	const CPosition &position				() const;
		IC	bool			operator<				(const LevelGraph::CVertex &vertex) const;
		IC	bool			operator>				(const LevelGraph::CVertex &vertex) const;
		IC	bool			operator==				(const LevelGraph::CVertex &vertex) const;
		friend class CLevelGraph;
	};

	struct SSegment {
		Fvector v1;
		Fvector v2;
	};

	struct SContour : public SSegment {
		Fvector v3;
		Fvector v4;
	};
};

#include "stdafx.h"

#define TRACE_QZERO_TOLERANCE	0.1f

void Fquaternion::set(const _matrix& M){
	float trace,s;
	
	trace = M._11 + M._22 + M._33;
	if (trace > 0.0f){
		s = sqrtf(trace + 1.0f);
		w = s * 0.5f;
		s = 0.5f / s;
		
		x = (M._32 - M._23) * s;
		y = (M._13 - M._31) * s;
		z = (M._21 - M._12) * s;
	}else{
		int biggest;
		enum {A,E,I};
		if (M._11 > M._22){
			if (M._33 > M._11)
				biggest = I;	
			else
				biggest = A;
		}else{
			if (M._33 > M._11)
				biggest = I;
			else
				biggest = E;
		}
		
		// in the unusual case the original trace fails to produce a good sqrt, try others...
		switch (biggest){
		case A:
			s = sqrtf( M._11 - (M._22 + M._33) + 1.0f);
			if (s > TRACE_QZERO_TOLERANCE){
				x = s * 0.5f;
				s = 0.5f / s;
				w = (M._32 - M._23) * s;
				y = (M._12 + M._21) * s;
				z = (M._13 + M._31) * s;
				break;
			}
			// I
			s = sqrtf( M._33 - (M._11 + M._22) + 1.0f);
			if (s > TRACE_QZERO_TOLERANCE){
				z = s * 0.5f;
				s = 0.5f / s;
				w = (M._21 - M._12) * s;
				x = (M._31 + M._13) * s;
				y = (M._32 + M._23) * s;
				break;
			}
			// E
			s = sqrtf( M._22 - (M._33 + M._11) + 1.0f);
			if (s > TRACE_QZERO_TOLERANCE){
				y = s * 0.5f;
				s = 0.5f / s;
				w = (M._13 - M._31) * s;
				z = (M._23 + M._32) * s;
				x = (M._21 + M._12) * s;
				break;
			}
			break;
		case E:
			s = sqrtf( M._22 - (M._33 + M._11) + 1.0f);
			if (s > TRACE_QZERO_TOLERANCE){
				y = s * 0.5f;
				s = 0.5f / s;
				w = (M._13 - M._31) * s;
				z = (M._23 + M._32) * s;
				x = (M._21 + M._12) * s;
				break;
			}
			// I
			s = sqrtf( M._33 - (M._11 + M._22) + 1.0f);
			if (s > TRACE_QZERO_TOLERANCE){
				z = s * 0.5f;
				s = 0.5f / s;
				w = (M._21 - M._12) * s;
				x = (M._31 + M._13) * s;
				y = (M._32 + M._23) * s;
				break;
			}
			// A
			s = sqrtf( M._11 - (M._22 + M._33) + 1.0f);
			if (s > TRACE_QZERO_TOLERANCE){
				x = s * 0.5f;
				s = 0.5f / s;
				w = (M._32 - M._23) * s;
				y = (M._12 + M._21) * s;
				z = (M._13 + M._31) * s;
				break;
			}
			break;
		case I:
			s = sqrtf( M._33 - (M._11 + M._22) + 1.0f);
			if (s > TRACE_QZERO_TOLERANCE){
				z = s * 0.5f;
				s = 0.5f / s;
				w = (M._21 - M._12) * s;
				x = (M._31 + M._13) * s;
				y = (M._32 + M._23) * s;
				break;
			}
			// A
			s = sqrtf( M._11 - (M._22 + M._33) + 1.0f);
			if (s > TRACE_QZERO_TOLERANCE){
				x = s * 0.5f;
				s = 0.5f / s;
				w = (M._32 - M._23) * s;
				y = (M._12 + M._21) * s;
				z = (M._13 + M._31) * s;
				break;
			}
			// E
			s = sqrtf( M._22 - (M._33 + M._11) + 1.0f);
			if (s > TRACE_QZERO_TOLERANCE){
				y = s * 0.5f;
				s = 0.5f / s;
				w = (M._13 - M._31) * s;
				z = (M._23 + M._32) * s;
				x = (M._21 + M._12) * s;
				break;
			}
			break;
		default:
			VERIFY(0);
		}
		}
}

#ifndef __Q__
#define __Q__

typedef struct _quaternion {
	float x,y,z,w;

	__forceinline	void	RotationYawPitchRoll(const _vector &ypr) {
		float fSinYaw   = sinf(ypr.x/2);
		float fSinPitch = sinf(ypr.y/2);
		float fSinRoll  = sinf(ypr.z/2);
		float fCosYaw   = cosf(ypr.x/2);
		float fCosPitch = cosf(ypr.y/2);
		float fCosRoll  = cosf(ypr.z/2);

		x = fSinRoll * fCosPitch * fCosYaw - fCosRoll * fSinPitch * fSinYaw;
		y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
		z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
		w = fCosRoll * fCosPitch * fCosYaw + fSinRoll * fSinPitch * fSinYaw;
	}
	__forceinline	void	RotationYawPitchRoll(float x, float y, float z) {
		float fSinYaw   = sinf(x/2);
		float fSinPitch = sinf(y/2);
		float fSinRoll  = sinf(z/2);
		float fCosYaw   = cosf(x/2);
		float fCosPitch = cosf(y/2);
		float fCosRoll  = cosf(z/2);

		x = fSinRoll * fCosPitch * fCosYaw - fCosRoll * fSinPitch * fSinYaw;
		y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
		z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
		w = fCosRoll * fCosPitch * fCosYaw + fSinRoll * fSinPitch * fSinYaw;
	}
} Fquaternion;


#endif

#pragma once
#include <cmath>
#include <cstdint>
#include <cstring>

enum Axis {
	XAxis = 0,
	YAxis = 1,
	ZAxis = 2,
	XAxisF = 3,
	YAxisF = 4,
	ZAxisF = 5,
};

class WVector2D {
public:
	float x = 0.0f;
	float y = 0.0f;
};

class WVector {
public:
	constexpr WVector() : x(0), y(0), z(0) {}
	constexpr WVector(float x, float y, float z) : x(x), y(y), z(z) {}

	union {
		struct {
			float x;
			float y;
			float z;
		};

		float p[3]{0.0f, 0.0f, 0.0f};
	};

	static int WisEqual(const WVector& a1, const WVector& a2, float a3) {
		if (std::abs(a1.x - a2.x) >= a3)
			return false;
		if (std::abs(a1.y - a2.y) >= a3)
			return false;
		if (std::abs(a1.z - a2.z) >= a3)
			return false;
		return true;
	}

	void Normalize();
};

constexpr static WVector VEC_ZERO = {0.0f, 0.0f, 0.0f};
constexpr static WVector VEC_UNIT_POS_X = {1.0f, 0.0f, 0.0f};
constexpr static WVector VEC_UNIT_POS_Y = {0.0f, 1.0f, 0.0f};
constexpr static WVector VEC_UNIT_POS_Z = {0.0f, 0.0f, 1.0f};
constexpr static WVector VEC_UNIT_NEG_X = {-1.0f, 0.0f, 0.0f};
constexpr static WVector VEC_UNIT_NEG_Y = {0.0f, -1.0f, 0.0f};
constexpr static WVector VEC_UNIT_NEG_Z = {0.0f, 0.0f, -1.0f};

inline void WVector::Normalize() {
	if (!WisEqual(*this, VEC_ZERO, 0.00001f)) {
		float m = std::sqrt(this->z * this->z + this->y * this->y + this->x * this->x);
		this->x *= 1.0f / m;
		this->y *= 1.0f / m;
		this->z *= 1.0f / m;
	} else {
		this->x *= 0.0f;
		this->y *= 0.0f;
		this->z *= 0.0f;
	}
}

inline WVector operator*(const WVector& v, float m) {
	WVector result;
	result.x = m * v.x;
	result.y = m * v.y;
	result.z = m * v.z;
	return result;
}

inline WVector operator+(const WVector& a, const WVector& b) {
	WVector result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}

inline WVector WCrossProduct(const WVector& a1, const WVector& a2) {
	WVector result;
	result.x = a1.y * a2.z - a1.z * a2.y;
	result.y = a1.z * a2.x - a2.z * a1.x;
	result.z = a1.x * a2.y - a1.y * a2.x;
	return result;
}

class WVector4 {
public:
	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};

		float p[4];
	};
};

class WMatrix {
public:
	WMatrix() {
		memset(p, 0, sizeof(p));
	}

	WMatrix operator~() const {
		WMatrix result;
		float x = this->ya.y * this->za.z - this->za.y * this->ya.z;
		float y = this->za.x * this->ya.z - this->za.z * this->ya.x;
		float z = this->za.y * this->ya.x - this->ya.y * this->za.x;
		float f = 1.0f / (z * this->xa.z + y * this->xa.y + x * this->xa.x);
		result.xa.x = f * x;
		result.xa.y = f * (this->za.y * this->xa.z - this->xa.y * this->za.z);
		result.xa.z = f * (this->xa.y * this->ya.z - this->xa.z * this->ya.y);
		result.ya.x = f * y;
		result.ya.y = f * (this->xa.x * this->za.z - this->xa.z * this->za.x);
		result.ya.z = f * (this->xa.z * this->ya.x - this->xa.x * this->ya.z);
		result.za.x = f * z;
		result.za.y = f * (this->xa.y * this->za.x - this->za.y * this->xa.x);
		result.za.z = f * (this->ya.y * this->xa.x - this->xa.y * this->ya.x);
		result.pivot.x = -(result.xa.x * this->pivot.x + result.za.x * this->pivot.z + result.ya.x * this->pivot.y);
		result.pivot.y = -(result.xa.y * this->pivot.x + result.za.y * this->pivot.z + result.ya.y * this->pivot.y);
		result.pivot.z = -(result.xa.z * this->pivot.x + result.za.z * this->pivot.z + result.ya.z * this->pivot.y);
		return result;
	}

	union {
		struct {
			WVector xa;
			WVector ya;
			WVector za;
			WVector pivot;
		};

		struct {
			float xx, yx, zx;
			float xy, yy, zy;
			float xz, yz, zz;
			float xm, ym, zm;
		};

		float p[12];
	};
};

inline WMatrix operator*(const WMatrix& m, const WVector& v) {
	WMatrix result = m;
	result.xa.x *= v.x;
	result.ya.x *= v.x;
	result.za.x *= v.x;
	result.pivot.x *= v.x;
	result.xa.y *= v.y;
	result.ya.y *= v.y;
	result.za.y *= v.y;
	result.pivot.y *= v.y;
	result.xa.z *= v.z;
	result.ya.z *= v.z;
	result.za.z *= v.z;
	result.pivot.z *= v.z;
	return result;
}

inline WMatrix operator*(const WMatrix& m1, const WMatrix& m2) {
	WMatrix result;
	result.xa.x = m1.xa.y * m2.ya.x + m1.xa.x * m2.xa.x + m1.xa.z * m2.za.x;
	result.xa.y = m1.xa.z * m2.za.y + m1.xa.y * m2.ya.y + m1.xa.x * m2.xa.y;
	result.xa.z = m1.xa.z * m2.za.z + m1.xa.x * m2.xa.z + m1.xa.y * m2.ya.z;
	result.ya.x = m1.ya.z * m2.za.x + m1.ya.x * m2.xa.x + m1.ya.y * m2.ya.x;
	result.ya.y = m1.ya.z * m2.za.y + m1.ya.y * m2.ya.y + m1.ya.x * m2.xa.y;
	result.ya.z = m1.ya.x * m2.xa.z + m1.ya.z * m2.za.z + m1.ya.y * m2.ya.z;
	result.za.x = m1.za.z * m2.za.x + m1.za.x * m2.xa.x + m1.za.y * m2.ya.x;
	result.za.y = m1.za.z * m2.za.y + m1.za.y * m2.ya.y + m1.za.x * m2.xa.y;
	result.za.z = m1.za.x * m2.xa.z + m1.za.z * m2.za.z + m1.za.y * m2.ya.z;
	result.pivot.x = m1.pivot.z * m2.za.x + m1.pivot.x * m2.xa.x + m1.pivot.y * m2.ya.x + m2.pivot.x;
	result.pivot.y = m1.pivot.z * m2.za.y + m1.pivot.y * m2.ya.y + m1.pivot.x * m2.xa.y + m2.pivot.y;
	result.pivot.z = m1.pivot.x * m2.xa.z + m1.pivot.z * m2.za.z + m1.pivot.y * m2.ya.z + m2.pivot.z;
	return result;
}

inline WVector operator*(const WVector& v, const WMatrix& m) {
	WVector result;
	result.x = m.za.x * v.z + m.ya.x * v.y + m.xa.x * v.x + m.pivot.x;
	result.y = m.za.y * v.z + m.ya.y * v.y + m.xa.y * v.x + m.pivot.y;
	result.z = m.za.z * v.z + m.ya.z * v.y + m.xa.z * v.x + m.pivot.z;
	return result;
}

inline WMatrix RotMat(WVector a) {
	// TODO: This assumes RotMat is taking euler angles, which may or may not be true.
	WMatrix result;
	float cx = std::cos(a.x / 2);
	float sx = std::sin(a.x / 2);
	float cy = std::cos(a.y / 2);
	float sy = std::sin(a.y / 2);
	float cz = std::cos(a.z / 2);
	float sz = std::sin(a.z / 2);
	float q0 = cx * cy * cz + sx * sy * sz;
	float q1 = sx * cy * cz - cx * sy * sz;
	float q2 = cx * sy * cz + sx * cy * sz;
	float q3 = cx * cy * sz - sx * sy * cz;
	result.p[0] = 1 - 2 * (q2 * q2 + q3 * q3);
	result.p[1] = 0 + 2 * (q1 * q2 - q0 * q3);
	result.p[2] = 0 + 2 * (q0 * q2 + q1 * q3);
	result.p[3] = 0 + 2 * (q1 * q2 + q0 * q3);
	result.p[4] = 1 - 2 * (q1 * q1 + q3 * q3);
	result.p[5] = 0 + 2 * (q2 * q3 - q0 * q1);
	result.p[6] = 0 + 2 * (q1 * q3 - q0 * q2);
	result.p[7] = 0 + 2 * (q0 * q1 + q2 * q3);
	result.p[8] = 1 - 2 * (q1 * q1 + q2 * q2);
	result.p[9] = 0;
	result.p[10] = 0;
	result.p[11] = 0;
	return result;
}

inline WVector RotVec(const WVector& a1, const WMatrix& a2) {
	WVector result;
	result.x = a2.za.x * a1.z + a2.ya.x * a1.y + a2.xa.x * a1.x;
	result.y = a2.za.y * a1.z + a2.ya.y * a1.y + a2.xa.y * a1.x;
	result.z = a2.za.z * a1.z + a2.ya.z * a1.y + a2.xa.z * a1.x;
	return result;
}

class WMatrix4 {
public:
	constexpr WMatrix4() {}

	explicit constexpr WMatrix4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
	                            float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
		: p{m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44} { }

	union {
		struct {
			WVector xa;
			float wx;
			WVector ya;
			float wy;
			WVector za;
			float wz;
			WVector pivot;
			float wm;
		};

		struct {
			float xx, yx, zx;
			unsigned char gapC[4];
			float xy, yy, zy;
			unsigned char gap1C[4];
			float xz, yz, zz;
			unsigned char gap2C[4];
			float xm, ym, zm;
		};

		float m[4][4];
		float p[16]{};
	};
};

constexpr static WMatrix4 MAT4_IDENTITY{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1,
};

constexpr static WMatrix4 MAT4_ZERO{
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
};

inline void SetWMatrix4FromWMatrix(WMatrix4& m4, const WMatrix& m) {
	m4.xa = m.xa;
	m4.wx = 0.0f;
	m4.ya = m.ya;
	m4.wy = 0.0f;
	m4.za = m.za;
	m4.wz = 0.0f;
	m4.pivot = m.pivot;
	m4.wm = 1.0f;
}

class WPlane {
public:
	WPlane() {}

	union {
		struct {
			float x;
			float y;
			float z;
		};

		struct {
			float a;
			float b;
			float c;
		};

		WVector normal{};
	};

	float dis{};
};

typedef struct _WPOINT {
	float x = 0.0f;
	float y = 0.0f;
} WPOINT;

class WPoint : public WPOINT {
public:
	WPoint() {}
	WPoint(float x, float y) : WPOINT{x, y} {}
};

typedef struct _WRECT {
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
} WRECT;

class WRect : public WRECT {};

struct WQuat {
	float x;
	float y;
	float z;
	float w;
};

struct WSphere {
	WVector pos;
	float radius;
};

struct Wobb {
	WVector center;
	WVector extend[3];
};


typedef struct _WSIZE {
	float w;
	float h;
} WSIZE;

struct WSize : WSIZE {};

constexpr double g_pi = 3.14159265358979323846;
constexpr double g_pi2 = 1.57079632679489661923;
constexpr double g_pi4 = 0.785398163397448309616;

constexpr float g_pif = 3.14159265358979323846f;
constexpr float g_pi2f = 1.57079632679489661923f;
constexpr float g_pi4f = 0.785398163397448309616f;

inline uint32_t ULblend(uint32_t x, uint32_t y, uint8_t a) {
	uint32_t v1 = (a * (y >> 8 & 0xFF00FF) + (255 - a) * (x >> 8 & 0xFF00FF)) >> 0;
	uint32_t v2 = (a * (y >> 0 & 0xFF00FF) + (255 - a) * (x >> 0 & 0xFF00FF)) >> 8;
	uint32_t v3 = (a * (y >> 8 & 0xFF00FF) + (255 - a) * (x >> 8 & 0xFF00FF)) >> 0;
	return v1 ^ ((v2 ^ v3) & 0xFF00FF);
}

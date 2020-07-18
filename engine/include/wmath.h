#pragma once
#include <cmath>
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

static WVector operator*(const WVector& v, float m) {
	WVector result;
	result.x = m * v.x;
	result.y = m * v.y;
	result.z = m * v.z;
	return result;
}

static WVector operator+(const WVector& a, const WVector& b) {
	WVector result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}

static WVector WCrossProduct(const WVector& a1, const WVector& a2) {
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

static WMatrix operator*(const WMatrix& a2, const WMatrix& a3) {
	WMatrix result;
	result.xa.x = a2.xa.y * a3.ya.x + a2.xa.x * a3.xa.x + a2.xa.z * a3.za.x;
	result.xa.y = a2.xa.z * a3.za.y + a2.xa.y * a3.ya.y + a2.xa.x * a3.xa.y;
	result.xa.z = a2.xa.z * a3.za.z + a2.xa.x * a3.xa.z + a2.xa.y * a3.ya.z;
	result.ya.x = a2.ya.z * a3.za.x + a2.ya.x * a3.xa.x + a2.ya.y * a3.ya.x;
	result.ya.y = a2.ya.z * a3.za.y + a2.ya.y * a3.ya.y + a2.ya.x * a3.xa.y;
	result.ya.z = a2.ya.x * a3.xa.z + a2.ya.z * a3.za.z + a2.ya.y * a3.ya.z;
	result.za.x = a2.za.z * a3.za.x + a2.za.x * a3.xa.x + a2.za.y * a3.ya.x;
	result.za.y = a2.za.z * a3.za.y + a2.za.y * a3.ya.y + a2.za.x * a3.xa.y;
	result.za.z = a2.za.x * a3.xa.z + a2.za.z * a3.za.z + a2.za.y * a3.ya.z;
	result.pivot.x = a2.pivot.z * a3.za.x + a2.pivot.x * a3.xa.x + a2.pivot.y * a3.ya.x + a3.pivot.x;
	result.pivot.y = a2.pivot.z * a3.za.y + a2.pivot.y * a3.ya.y + a2.pivot.x * a3.xa.y + a3.pivot.y;
	result.pivot.z = a2.pivot.x * a3.xa.z + a2.pivot.z * a3.za.z + a2.pivot.y * a3.ya.z + a3.pivot.z;
	return result;
}

static WMatrix RotMat(WVector a) {
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

class WMatrix4 {
public:
	WMatrix4() {
		memset(p, 0, sizeof(p));
	}

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
		float p[16];
	};
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

	union {
		float dis{};
		float d;
	};
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

typedef struct _WSIZE {
	float w;
	float h;
} WSIZE;

struct WSize : public WSIZE {};

constexpr double g_pi = 3.14159265358979323846;
constexpr double g_pi2 = 1.57079632679489661923;
constexpr double g_pi4 = 0.785398163397448309616;

constexpr float g_pif = 3.14159265358979323846f;
constexpr float g_pi2f = 1.57079632679489661923f;
constexpr float g_pi4f = 0.785398163397448309616f;

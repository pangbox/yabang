#pragma once
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
	union {
		struct {
			float x;
			float y;
			float z;
		};

		float p[3]{0.0f, 0.0f, 0.0f};
	};
};

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
};

typedef struct _WRECT {
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
} WRECT;

class WRect : public WRECT {
};

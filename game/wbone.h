#pragma once
#include <map>
#include <vector>

#include "waabb.h"
#include "wflags.h"
#include "wmesh.h"
#include "wscene.h"

class WBone {
public:
	enum RenderFlag {
		NOCLIP = 1 << 0,
		NOUPDATEMAT = 1 << 1,
	};

	enum w_bone_flag {
		HIDEBONE = 1 << 0,
		SELFILLUM = 1 << 1,
		HASRIGIDVTX = 1 << 2,
		VISIBLE = 1 << 3,
		CLIPPING = 1 << 4,
		CLONED_MESH = 1 << 5,
		CLONED_ANI = 1 << 6,
		ROTATEBONE = 1 << 9,
		MOVEBONE = 1 << 10,
		TRANSFORM = 1 << 12,
		PHYSICSMODEL = 1 << 13,
		COLORPERVERTEX = 1 << 14,
		CHECK_SHADOW = 1 << 15,
		MESHBONE = 1 << 16,
		MIRRORED = 1 << 17,
		INVISIBLE = 1 << 18,
		SCALEBONE = 1 << 19,
	};

	struct w_bone_physic_link {
		struct w_bone_physic_link_element {
			int offset;
			float length;
		};

		int vtxnum;
		WVector* pos;
		WVector* vel;
		WVector** list;
		w_bone_physic_link_element* link;
		int* linkoffset;
	};

	struct w_normalmerge {
		struct N {
			WVector m_n;
			std::vector<int> m_idxList;
		};

		struct P {
			WVector m_p;
			std::map<w_mesh*, std::vector<N>> m_meshList;
		};

		std::map<float, std::vector<P>> m_wlist;
	};

private:
	w_bone_physic_link* m_physic;
	w_mesh* m_mesh;
	char m_name[64];
	unsigned int m_hashCode;
	char* m_motion_bone_name;
	WQuat m_basicQuat;
	WMatrix m_basicMat;
	WSphere m_bound_sphere;
	Waabb m_bound_aabb;
	w_keyframe_pos* m_keyPos;
	int m_keyPosNum;
	w_keyframe_rot* m_keyRot;
	int m_keyRotNum;
	int m_keyFlag;
	WBone* m_parent;
	WBone* m_child;
	WBone* m_next;
	float m_lightLine;
	int m_rendMode;
	int m_id;
	unsigned int m_selfillumColor;
	char m_alpha;
	float m_scale;
	WFlags m_flag;
	WMatrix m_localMat;
	WMatrix m_matrix;
	WMatrix m_rotateMat;
	WVector m_moveVec;
	LightSet m_light;
	unsigned int m_vtxColor;
	float m_bonescale;
};

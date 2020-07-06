#pragma once
#include "waabb.h"
#include "wlist.h"
#include "wmath.h"

class WBone;
class WVector;
class WxBatchGrp;
class WxBatch;

struct w_mesh;

struct w_keyframe_pos {
	float t;
	WVector pos;
};

struct w_keyframe_rot {
	float t;
	WQuat rot;
};

struct w_keyframe_scale {
	float t;
	float scale;
};

struct w_common_bone_data {
	int xid;
	char name[64];
	WSphere boundsphere;
	WMatrix basicMat;
	Waabb aabb;
	w_keyframe_pos* keyPos;
	w_keyframe_rot* keyRot;
	w_keyframe_scale* keyScale;
	int keyPosNum;
	int keyRotNum;
	int keyScaleNum;
	int keyFlag;
	w_mesh* mesh;
};

struct w_bound_box_info {
	Waabb aabb;
	WVector spherePivot;
	float sphereLength;
	char* option;
	char name[1];
};

struct w_bound_box {
	WBone* bone;
	w_bound_box_info* info;
};

struct w_motion_data {
	float s;
	float e;
	float end;
	int option1;
	float option2;
	char* applybone;
	char* name;
	char* nextmotion;
	char ptr[1];
};

struct w_motion_addition {
	WList<w_motion_data*>* mdList;
	WBone* rootbone;
	float s;
	float e;
	float length;
	float h_gap;
	char name[1];
};

struct w_face_animation
{
	w_mesh** mesh;
	int meshNum;
	int group;
	char name[32];
	char filename[32];
	float tu;
	float tv;
	float du;
	float dv;
	float scalex;
	float scaley;
	int texHandle;
};

struct w_share_pet_data {
	struct w_frame {
		int num;
		int check;
		char* data;
	};
	
	char* petName;
	WBone* rootbone;
	float length;
	int faceNum;
	int numFramedata;
	w_frame* frame;
	Waabb bound;
	WList<w_bound_box*>* bbList;
	WList<w_motion_data*>* mdList;
	WList<w_face_animation*>* fanimList;
};

struct w_mesh {
	int vtxNum;
	int rigidNum;
	int blendedRigidNum;
	int blendedTotalNum;
	WVector* vecList;
	WVector* normList;
	WVector* blendedVecList;
	float* blendWeightList;
	WVector* blendedNormalList;
	unsigned int* originalVtxColorList;
	int maxBoneNum;
	float* weightList;
	int indexNum;
	unsigned __int16* indexList;
	int originalTexHandle;
	float originalTu;
	float originalTv;
	float originalScaleu;
	float originalScalev;
	unsigned int diffuse;
	float(*uvBackup)[2];
	float(*uvData)[2];
	bool bEnv;
	bool bSpec;
	int group;
	int flags;
	w_mesh* next;
	float alpha;
	WBone** boneList;
	WBone** blendedBoneList;
	unsigned int* vtxColorList;
	unsigned int** vtxColorPtrList;
	WVector* myNormalList;
	WVector* myBlendedNormalList;
	int texHandle;
	float tu;
	float tv;
	float scaleu;
	float scalev;
	int clipFlag;
	int drawFlag;
	WxBatchGrp* xpBatchGrp;
	WxBatch* xpBatch;
	int xiBaseVtxIdx;
	int xiBaseIdxIdx;
	int xiDrawFlag2;
	WBone* aabbBone;
	Waabb localAabb;
	char mpetName[64];
};

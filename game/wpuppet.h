#pragma once
#include <vector>

#include "waabb.h"
#include "wlist.h"
#include "wmath.h"
#include "wmesh.h"
#include "wresource.h"

class WBone;

class WPuppet : public WResource {
public:
	enum w_get_bone_flag {
		FIXEDPIVOT = 0x1,
		ADDFORCE = 0x2,
		EXCEPTPARENT = 0x4,
		EX_PONYTAIL = 0x8,
	};

	struct w_tex_piece {
		float pu, pv;
		float scalex, scaley;
		int texHandle;
		int count;
		char name[1];
	};

private:
	int m_lightmode;
	WList<WBone*> m_phybone;
	WMatrix m_mat;
	WList<w_motion_addition*> m_addedMotionList;
	int m_rendMode;
	bool m_bClone;
	bool m_bHaveCenter;
	w_share_pet_data::w_frame* m_frame;
	int m_framenum;
	char m_petName[64];
	int m_faceNum;
	Waabb m_bound;
	WSphere m_boundSphere;
	w_bound_box m_BBox;
	WList<int>* m_storeTexList;
	WList<WPuppet::w_tex_piece*> m_use_tex_piece;
	WList<w_bound_box*> m_bbList;
	WList<w_motion_data*>* m_mdList;
	WList<w_face_animation*> m_fanimList;
	WList<WBone*> m_boneList;
	float m_aniLen;
	int m_iPetType;
	WBone* m_rootbone;
	std::vector<WMatrix const*, std::allocator<WMatrix const*> > m_xTransfMatPtrList;
};

#pragma once
#include "GameEngine_Ver3_83.h"
#include <fstream>
#include <sstream>

//�Q�[�����S��Ŏg�p����\���̂Ȃǂ��`����
//-----------------------------------------------------------------------------------------------

#include "fpscounter.h"
#include "BEnemy.h"
#include "task_Map2D.h"
#include "task_Player.h"
#include "task_Effect00.h"

//-----------------------------------------------------------------------------------------------
namespace  MyPG
{
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//----------------------------------------------
	//�J������{�`
	class Camera
	{
		Camera() :
			fov(0.f),
			nearPlane(0.f),
			forePlane(0.f),
			aspect(0.f) { }
		Camera(
			const ML::Vec3& tg_,	//	��ʑ̂̈ʒu
			const ML::Vec3& pos_,	//	�J�����̈ʒu
			const ML::Vec3& up_,	//	�J�����̏�����������x�N�g���i��̂x�{�Œ�j
			float				fov_,	//	����p
			float				np_,	//	�O�N���b�v���ʁi������O�͉f��Ȃ��j
			float				fp_,	//	��N���b�v���ʁi��������͉f��Ȃ��j
			float				asp_);	//	�A�X�y�N�g��i��ʂ̔䗦�ɍ��킹��@�����c�j			
	public:
		//	�r���[���i�����֘A�j
		ML::Vec3 target;			//	��ʑ̂̈ʒu
		ML::Vec3 pos;			//	�J�����̈ʒu
		ML::Vec3 up;				//	�J�����̏�����������x�N�g���i��̂x�{�Œ�j
		//	�ˉe���i����͈͊֘A�j
		float fov;					//	����p
		float nearPlane;			//	�O�N���b�v���ʁi������O�͉f��Ȃ��j
		float forePlane;			//	��N���b�v���ʁi��������͉f��Ȃ��j
		float aspect;				//	�A�X�y�N�g��i��ʂ̔䗦�ɍ��킹��@�����c�j
		//	�s����
		ML::Mat4x4  matView, matProj;
		~Camera();
		using SP = shared_ptr<Camera>;
		//	�J�����𐶐�����
		static SP Create(
			const ML::Vec3& tg_,	//	��ʑ̂̈ʒu
			const ML::Vec3& pos_,	//	�J�����̈ʒu
			const ML::Vec3& up_,	//	�J�����̏�����������x�N�g���i��̂x�{�Œ�j
			float				fov_,	//	����p
			float				np_,	//	�O�N���b�v���ʁi������O�͉f��Ȃ��j
			float				fp_,	//	��N���b�v���ʁi��������͉f��Ȃ��j
			float				asp_);	//	�A�X�y�N�g��i��ʂ̔䗦�ɍ��킹��@�����c�j	
		//	�J�����̐ݒ�
		void UpDate();
	};
	//----------------------------------------------
	class MyGameEngine : public GameEngine
	{
	public:
		MyGameEngine();
		//�Q�[���G���W���ɒǉ��������̂̏������ƊJ��
		bool Initialize(HWND wnd_);
		~MyGameEngine();
		//�Q�[���G���W���ɒǉ��������̂̃X�e�b�v����
		void UpDate();

		//3DPG1�Ή��ɂ��ǉ�
			//2D�`����̃f�t�H���g�p�����[�^�ݒ�
		void Set2DRenderState(DWORD l_);
		//3D�`����̃f�t�H���g�p�����[�^�ݒ�
		void Set3DRenderState(DWORD l_);

		//�Q�[���G���W���ɒǉ����������͉̂��ɉ�����
		//�G�̌��o������ߖ�
		shared_ptr<vector<BEnemy::SP>> qa_Enemies;
		//�}�b�v�̌��o������ߖ�
		Map2D::Object::SP qa_Map;
		//�v���C���̌��o������ߖ�
		Player::Object::SP qa_Player;
		//----------------------------------------------
		MyPG::Camera::SP		camera[4];		//	�J����

		XI::Mouse::SP  mouse;
		XI::GamePad::SP  in1, in2, in3, in4;	//��荇�����S�{
		//�Q�c�J������`
		ML::Box2D				camera2D;	//  �Q�c�X�N���[������p
		map<string, int> evFlags;

		bool isDead = false;//���ꂽ���ۂ�
		bool isGameOver = false; //�Q�[���I�[�o�[�t���O
		bool isClear = false;//�N���A�t���O
		bool unHitEnemy = false;//�G�Ƃ̓����蔻��
		int GameOverCnt = 0;
		int MaxGameOver = 0;

		int stage = 1;//�X�e�[�W
		int maxStage = 3;//�ő�X�e�[�W��
		int remaining = 3;//�c�@��
		int score = 0;//�X�R�A

		//bool Jump2Check = false;	//�W�����v2����
		//bool DestroyFlag = false; //�j��t���O
		//bool AttackFlag = false; //�U���t���O

		//bool TransparentFlag = false; //�����t���O
		//�ϐ��̏���l�A�����l���`�F�b�N���A�����𒴂��Ȃ��͈͂ŕԂ�
		template<typename T>
		T Clamp(const T& x, const T& low, const T& high);

		//�J�����̍��W��K�p
		ML::Box2D ApplyCamera2D(const ML::Box2D& draw);

		//----------------------------------------------
		FPSCounter* c;

		void CreateEffect(int no, ML::Vec2 pos);
		void Dbg_ToConsole(const char* str, ...);
		void Dbg_ToDisplay(int x, int y, const char* str, ...);
		void Dbg_ToTitle(const char* str, ...);
		void Dbg_ToBox(const char* str, ...);
		void Dbg_ToFileOut(const char* str, ...);

		//�f�o�b�O��`
		//��������
		DG::Image::SP rectImage;

		enum DEBUGRECTMODE {
			RED, BLUE, GREEN, ORANGE,
			REDFRAME, BLUEFRAME, GREENFRAME, ORANGEFRAME,
		};
		struct DebugRectInfo {
			ML::Box2D	r;
			int			flag;
		};

		std::vector<DebugRectInfo> debugRectArray;
		//�������p
		void debugRectLoad();
		void debugRectReset();
		void debugRectDraw();
		//�\����������`�ƕ\�����[�h��n��
		//RED,BLUE,GREEN,ORANGE�̋�`��FRAME�͘g�̂�
		void debugRect(ML::Box2D r_, int flag = 4, int offsetx = 0, int offsety = 0);

		enum COUNTER_FLAGS {
			NONE,
			ACTIVE,//�ғ���
			LIMIT,//�I������
			END,//�I��
		};

		struct COUNTER {
			COUNTER_FLAGS flag;
			int count;
		};
		std::map < std::string, COUNTER > counterMap;


		void InitCounter();
		void ActCounter();
		void AllEndCounter();
		COUNTER_FLAGS getCounterFlag(string str);
		void StartCounter(string str, int count);


	};
}
extern MyPG::MyGameEngine* ge;



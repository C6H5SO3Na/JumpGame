#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�G00
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace Enemy00
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("�G");	//�O���[�v��
	const  string  defName("�G00");	//�^�X�N��
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()	override;
		bool  Finalize()	override;
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//���L����ϐ��͂����ɒǉ�����
		DG::Image::SP img,img0;
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
	//�ύX�s����������������������������������������������������
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//�������� ������true�Ń^�X�N�V�X�e���֎����o�^
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//�u�������v�^�X�N�������ɂP�񂾂��s������
		void  UpDate()			override;//�u���s�v�P�t���[�����ɍs������
		void  Render2D_AF()		override;//�u2D�`��v�P�t���[�����ɍs������
		bool  Finalize();		//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	//�ύX������������������������������������������������������
	public:
		enum class State {
			Normal, Clear, Dead, Non
		};
		//�L�����̌���
		enum class Angle_LR {
			Right, Left
		};

		//enum class Anim {
		//	Idle, Walk, Jump, fall, Clear, Dead
		//};
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����
		ML::Vec2 pos;//�v���C�����W
		ML::Vec2 moveVec;//�ړ����x
		ML::Box2D hitBase;//�����蔻��͈�
		ML::Box2D src;//�摜�̐؂���͈�
		ML::Box2D drawBase;//�`��͈�
		ML::Box2D footBase;//��������p
		ML::Box2D headBase;//���㔻��p
		ML::Box2D leftSideBase;//������p
		ML::Box2D rightSideBase;//������p
		State state = State::Non;
		Angle_LR angle = Angle_LR::Right;
		//Anim animKind = Anim::Idle;
		float fallSpeed = 0.f;
		//float jumpPow = 0.f;
		bool hitFlag = false;
		int	moveCnt;//�s�������p�J�E���^

		int	animCnt;//�A�j���[�V���������p�J�E���^
		void  Move();
		void  CheckMove(ML::Vec2& e_);
		bool  CheckFoot();
		bool  CheckLeftSide();
		bool  CheckRightSide();
		void  CheckHitPlayer();
		ML::Box2D CenterBox(int w, int h);
	};
}
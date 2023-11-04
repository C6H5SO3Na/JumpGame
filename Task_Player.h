#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace Player
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("�v���C��");	//�O���[�v��
	const  string  defName("NoName");	//�^�X�N��
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
		DG::Image::SP img;
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
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����
		//���
		enum class State {
			Normal, Clear, Dead, Non
		};
		//�L�����̌���
		enum class Angle_LR {
			Right, Left
		};

		enum class Anim {
			Idle, Walk, Jump, Fall, Clear, Dead
		};
		State state = State::Non;
		ML::Vec2 pos;//�v���C�����W
		ML::Box2D hitBase;//�����蔻��͈�
		int	moveCnt;//�s�������p�J�E���^
		int	animCnt;//�A�j���[�V���������p�J�E���^

		void Operation();
		void CheckMove(ML::Vec2& e_);
		bool CheckFoot();
		bool CheckHead();
		void Animation();
		ML::Box2D CenterBox(int w, int h);
		ML::Box2D MultiplyBox2D(ML::Box2D box2D, float n);

	private:
		ML::Vec2 moveVec;//�ړ����x
		ML::Box2D src;//�摜�̐؂���͈�
		ML::Box2D drawBase;//�`��͈�
		ML::Box2D footBase;//��������p
		ML::Box2D headBase;//���㔻��p
		Angle_LR angle = Angle_LR::Right;
		Anim animKind = Anim::Idle;
		float fallSpeed = 0.f;
		float jumpPow = 0.f;
		bool isHitFloor = false;
	};
}
#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
#include "BChara.h"

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
	class  Object : public  BChara
	{
		//�ύX�s����������������������������������������������������
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//�������� ������true�Ń^�X�N�V�X�e���֎����o�^
		static  Object::SP  Create(bool flagGameEnginePushBack_);	
		static void  Spawn(const ML::Vec2& pos);//�^�X�N����&�p�����[�^�[�w��
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
		//���G��ԊǗ��N���X
		class Invincible {
			bool doFlash = false;// �_�Ńt���O
			bool flag = false;//���G�t���O
			int cnt = 0;//���G�J�E���g
		public:
			//���G���ۂ����擾
			bool isInvincible() const { return flag; }

			//�_�ŊJ�n
			void startFlash() { doFlash = true; }

			//�_�Œ������擾
			bool isFlash() const { return doFlash; }
			//�J�n
			void start()
			{
				flag = true;
				cnt = 100;
			}

			//����
			void operation()
			{
				if (!isInvincible()) { return; }
				if (cnt > 0) {
					--cnt;
				}
				else {
					flag = false;
					doFlash = false;
				}
			}
		};

		//�̗͊Ǘ��N���X
		class Life {
			int now = 0;
			int max = 0;
		public:
			//�R���X�g���N�^
			Life(const int& now_, const int& max_)
			{
				now = now_;
				max = max_;
			}
			//�f�t�H���g�R���X�g���N�^�֎~
			Life() = delete;

			//���C�t��������
			void addNow(const int& n) { now += n; }
			//���݂̃��C�t�擾
			int getNow() const { return now; }
			//�ő僉�C�t�擾
			int getMax() const { return max; }
			//�����Ă��邩�ۂ�
			bool isLive() const { return now >= 0; }
		};
	public:
		Life life;
		bool CheckHitEnemyHead();
	private:
		//�|�����[�t�B�Y��
		//��Ԃ̒��ۃC���^�[�t�F�[�X
		class StateBase {
		public:
			virtual ~StateBase() {}
			virtual void think() = 0;
			virtual void move() = 0;
			virtual void anim() = 0;
		private:
			ML::Box2D animTable_;
		};

		//�j���[�g����
		class IdleState :public StateBase {
		public:
			IdleState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		//���s
		class WalkState :public StateBase {
		public:
			WalkState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		//�W�����v
		class JumpState :public StateBase {
		public:
			JumpState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		//����
		class FallState :public StateBase {
		public:
			FallState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		//��e
		class HurtState :public StateBase {
		public:
			HurtState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		//��e
		class ClearState :public StateBase {
		public:
			ClearState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		//��e
		class DeadState :public StateBase {
		public:
			DeadState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		StateBase* state;

		float jumpPow;
		void Think() override;
		void Move() override;
		void Operation();
		void Animation();
		void ChangeState(StateBase* const state);
		void DamageOperation();
		void Recieved(const int& power) override;
		void Dead() override;
		//���C�t�̑���
		void LifeOperation(const int& addLife);
		void AdjustCameraPlayer() const;

		Invincible invincible;
		XI::VGamePad input;
	};
}
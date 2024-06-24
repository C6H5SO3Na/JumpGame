//#pragma warning(disable:4996)
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
		enum class Anim {
			Idle, Walk, Jump, Fall, Hurt, Clear, Dead
		};

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
			Life() {};
			//���C�t��������
			void addNow(const int& n) { now += n; }
			//���݂̃��C�t�擾
			int getNow() const { return now; }
			//�ő僉�C�t�擾
			int getMax() const { return max; }
		};

		void Operation();
		void Animation();
		bool CheckHitEnemyHead();
		bool CheckHitGoalFlag() const;
		void ChangeAnim(const Anim& anim);
		void DamageOperation();
		void Dead();
		//���C�t�̑���
		void LifeOperation(const int& addLife);

		Invincible invincible;

		Life life;
	private:
		Anim animKind;
		float jumpPow;
	};
}
#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�}�b�v
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace Map2D
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("�t�B�[���h");	//�O���[�v��
	const  string  defName("�}�b�v");	//�^�X�N��
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
		int map[100][100];//�ꉞ100*100
		int sizeY, sizeX;
		int chipSize;//�`�b�v�̃T�C�Y
		ML::Box2D hitBase;//�s�N�Z���P�ʂ̃}�b�v�T�C�Y������
		enum { chipKind = 16 };//�}�b�v�`�b�v�̎��
		ML::Box2D chip[chipKind];

		void DrawMapChip(int map, int x, int y);
		bool LoadMap(const string& fpath_);
		bool CheckHit(ML::Box2D hit_);
		void AdjustCameraPos();//�}�b�v�O�������Ȃ��悤�ɃJ�������ʒu����
	};
}
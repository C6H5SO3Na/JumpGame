//-------------------------------------------------------------------
//�Q�[���I�[�o�[�A���ʕ\��
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_GameOver.h"
#include "Task_Title.h"
#include "Task_PressSKey.h"
#include "sound.h"
#include "easing.h"

namespace GameOver
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		img = DG::Image::Create("./data/image/GameOver.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		img.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		res = Resource::Create();

		//���f�[�^������
		render2D_Priority[1] = 1.0f;
		phase = Phase::PlayingJingle;

		//���^�X�N�̐���

		//BGM(�W���O��)
		se::LoadFile("GameOver", "./data/Sound/BGM/GameOver.wav");
		se::Play("GameOver");

		//�J�E���^�[�J�n
		ge->StartCounter("Jingle", 60 * 3);
		//todo:�C�[�W���O�K�p�@GAME OVER���S�𕪗�����
		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���


		if (!ge->QuitFlag() && nextTaskCreate) {
			//�������p���^�X�N�̐���
			auto title = Title::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();
		//�i�K���̏���
		switch (phase)
		{
			//�C�[�W���O��
		case Phase::PlayingJingle:
			if (ge->getCounterFlag("Jingle") == ge->LIMIT) {
				PressSKey::Object::Create(true);
				phase = Phase::PressSKey;
			}
			break;

			//PressSKey���_�ł��Ă����
		case Phase::PressSKey:
			//PressStartKey�����������ʐ���
			if (ge->GetTask<PressSKey::Object>(PressSKey::defGroupName, PressSKey::defName) == nullptr)
			{
				ge->StartCounter("Fadeout", 45); //�t�F�[�h��90�t���[���Ȃ̂Ŕ�����45�Ő؂�ւ�
				//�t�F�[�h�̃G�t�F�N�g
				ge->CreateEffect(99, ML::Vec2(0.f, 0.f));
				phase = Phase::FadeOut;
			}
			break;

			//�t�F�[�h�A�E�g��
		case Phase::FadeOut:
			if (ge->getCounterFlag("Fadeout") == ge->LIMIT) {
				Kill();
			}
			break;
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(0, 0, ge->screen2DWidth, ge->screen2DHeight);
		ML::Box2D src(0, 0, 1920, 1080);
		res->img->Draw(draw, src);
	}

	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^
				
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() : phase(Phase::None) {	}
	//-------------------------------------------------------------------
	//���\�[�X�N���X�̐���
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { Finalize(); }
}
//-------------------------------------------------------------------
//�^�C�g�����
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Title.h"
#include "Task_StartGame.h"
#include "Task_PressSKey.h"
#include "sound.h"
#include "easing.h"

namespace Title
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		imgLogo = DG::Image::Create("./data/image/TitleLogo.png");
		imgBG = DG::Image::Create("./data/image/Title.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		imgLogo.reset();
		imgBG.reset();
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
		phase = Phase::IsEasing;

		//�C�[�W���O�Z�b�g&�J�n
		easing::Set("�^�C�g������", easing::EXPOOUT, -100, 100, 60);
		easing::Start("�^�C�g������");

		//BGM
		bgm::LoadFile("Title", "./data/Sound/BGM/Title.mp3");
		bgm::Play("Title");

		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		//BGM��~
		bgm::Stop("Title");
		ge->KillAll_G("UI");

		//�Q�[���J�n���̏���
		ge->stage = 1;
		ge->remaining = 3;
		ge->score = 0;

		if (!ge->QuitFlag() && nextTaskCreate) {
			//�������p���^�X�N�̐���
			auto game = StartGame::Object::Create(true);
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
		case Phase::IsEasing:
			if (easing::GetState("�^�C�g������") == easing::EQ_STATE::EQ_END) {
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
		easing::UpDate();
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		DrawBG();
		DrawLogo();
	}

	//-------------------------------------------------------------------
	//�w�i�`��
	void  Object::DrawBG() const
	{
		ML::Box2D src(0, 0, 1920, 1080);
		ML::Box2D draw(0, 0, ge->screen2DWidth, ge->screen2DHeight);
		res->imgBG->Draw(draw, src);
	}

	//-------------------------------------------------------------------
	//���S�`��
	void  Object::DrawLogo() const
	{
		ML::Box2D src(0, 0, 1228, 197);
		ML::Box2D draw(ge->screen2DWidth / 4 - 100, 0, src.w, src.h);
		draw.y += static_cast<int>(easing::GetPos("�^�C�g������"));//�C�[�W���O��K�p
		res->imgLogo->Draw(draw, src);
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
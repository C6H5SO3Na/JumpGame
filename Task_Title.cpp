//-------------------------------------------------------------------
//�^�C�g�����
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Title.h"
#include "Task_StartGame.h"
#include "Task_StageEditor.h"
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
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		this->res = Resource::Create();

		//���f�[�^������
		render2D_Priority[1] = 1.0f;
		isFadeout = false;
		//�C�[�W���O�Z�b�g
		easing::Set("�^�C�g������", easing::EXPOOUT, -100, 100, 60);
		//BGM
		bgm::LoadFile("Title", "./data/Sound/BGM/Title.mp3");
		bgm::Play("Title");
		//���^�X�N�̐���
		auto pressStartKey = PressSKey::Object::Create(true);

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
		if (!ge->QuitFlag() && this->nextTaskCreate) {
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
		case 0:
			easing::Start("�^�C�g������");
			++phase;
			break;

		case 1:
			if (easing::GetState("�^�C�g������") == easing::EQ_STATE::EQ_END) {
				++phase;
			}
			break;
		case 2:
			//PressStartKey�����������ʐ���
			if (ge->GetTask<PressSKey::Object>(PressSKey::defGroupName, PressSKey::defName) == nullptr)
			{
				if (!isFadeout) {
					ge->StartCounter("Fadeout", 45); //�t�F�[�h��90�t���[���Ȃ̂Ŕ�����45�Ő؂�ւ�
					ge->CreateEffect(99, ML::Vec2());
					isFadeout = true;
				}
			}

			if (ge->getCounterFlag("Fadeout") == ge->LIMIT) {
				this->Kill();
			}
			break;
		}
		easing::UpDate();
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		//
		{
			ML::Box2D draw(0, 0, ge->screen2DWidth, ge->screen2DHeight);
			ML::Box2D src(0, 0, 1920, 1080);
			res->imgBG->Draw(draw, src);
		}

		//�e�L�X�g�{�b�N�X
		ML::Box2D src(0, 0, 1228, 197);
		ML::Box2D draw(ge->screen2DWidth / 4 - 100, 0, src.w, src.h);
		string text;
		//�i�K���̏���
		switch (phase)
		{
		case 2://fall through
		case 1://fall through
		case 0:
			draw.y += static_cast<int>(easing::GetPos("�^�C�g������"));
			res->imgLogo->Draw(draw, src);
			break;
		}
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
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object():isFadeout() {	}
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
	Resource::~Resource() { this->Finalize(); }
}
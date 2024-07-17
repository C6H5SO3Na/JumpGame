//-------------------------------------------------------------------
//�Q�[���J�n���
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_StartGame.h"
#include "Task_Game.h"
#include <assert.h>

namespace StartGame
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		font = DG::Font::Create("�l�r ����", 30, 60);
		stageImg = DG::Image::Create("./data/image/Stage" + to_string(ge->stage) + ".jpg");
		playerImg = DG::Image::Create("./data/image/Idle_KG_1.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		font.reset();
		stageImg.reset();
		playerImg.reset();
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

		//�ϐ�������
		render2D_Priority[1] = 1.0f;
		phase = Phase::FadeIn;

		//�J�E���g�J�n
		ge->StartCounter("FadeIn", 45);
		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���

		if (!ge->QuitFlag() && nextTaskCreate) {
			//�������p���^�X�N�̐���
			auto game = Game::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		switch (phase) {
		case Phase::FadeIn:
			//�t�F�[�h�C���܂ő҂�
			if (ge->getCounterFlag("FadeIn") == ge->LIMIT) {
				ge->StartCounter("Start", 120);
				phase = Phase::ShowInfo;
			}
			break;
		case Phase::ShowInfo:
			//�����҂�����
			if (ge->getCounterFlag("Start") == ge->LIMIT) {
				//2D�J������`(�G�t�F�N�g������Ȃ��悤��)
				ge->camera2D = ML::Box2D(0, 0, ge->screen2DWidth, ge->screen2DHeight);

				//�t�F�[�h�̃G�t�F�N�g
				ge->CreateEffect(99, ML::Vec2(0.f, 0.f));
				ge->StartCounter("FadeOut", 45);
				phase = Phase::FadeOut;
			}
			break;
		case Phase::FadeOut:
			//���S�Ƀt�F�[�h�A�E�g������
			if (ge->getCounterFlag("FadeOut") == ge->LIMIT) {
				Kill();//���̃^�X�N��
			}
			break;
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		DrawStageNum();
		DrawScore();
		DrawRemain();
		DrawImageStage();
	}

	//-------------------------------------------------------------------
	//�X�e�[�W���`��
	void  Object::DrawStageNum() const
	{
		ML::Box2D textBox(500, 200, 500, 200);
		string text = "Stage " + to_string(ge->stage);
		res->font->Draw(textBox, text);
	}

	//-------------------------------------------------------------------
	//�X�R�A�`��
	void  Object::DrawScore() const
	{
		ML::Box2D textBox(1000, 200, 500, 200);
		string text = "Score:" + to_string(ge->score);
		res->font->Draw(textBox, text);
	}

	//-------------------------------------------------------------------
	//�c�@�`��
	void  Object::DrawRemain() const
	{
		DrawPlayerImage();
		DrawRemainNum();
	}

	//-------------------------------------------------------------------
	//�c�@�̃v���C���A�C�R���`��
	void  Object::DrawPlayerImage() const
	{
		ML::Box2D draw(1550, 750, 200, 128);
		ML::Box2D src(0, 0, 100, 64);
		res->playerImg->Draw(draw, src);
	}

	//-------------------------------------------------------------------
	//�c�@���`��
	void  Object::DrawRemainNum() const
	{
		ML::Box2D textBox(1700, 800, 500, 200);
		string text = "x " + to_string(ge->remaining);
		res->font->Draw(textBox, text);
	}

	//-------------------------------------------------------------------
	//�X�e�[�W�摜�`��
	void  Object::DrawImageStage() const
	{
		ML::Box2D draw(400, 300, 960, 540);
		ML::Box2D src(0, 0, 1920, 1080);
		res->stageImg->Draw(draw, src);
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
	Object::Object() :phase(Phase::None) {	}
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
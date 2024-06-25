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
		stageImg = DG::Image::Create("./data/image/Stage" + to_string(ge->stage) + ".png");
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
		this->res = Resource::Create();

		render2D_Priority[1] = 1.0f;

		ge->StartCounter("FadeIn", 45);
		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���

		if (!ge->QuitFlag() && this->nextTaskCreate) {
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
		case 0:
			//�t�F�[�h�C���܂ő҂�
			if (ge->getCounterFlag("FadeIn") == ge->LIMIT) {
				++phase;
				ge->StartCounter("Start", 120);
			}
			break;
		case 1:
			//A�{�^���������ꂽ��
			if (ge->getCounterFlag("Start") == ge->LIMIT) {
				//2D�J������`(�G�t�F�N�g������Ȃ��悤��)
				ge->camera2D = ML::Box2D(0, 0, ge->screen2DWidth, ge->screen2DHeight);
				ge->CreateEffect(99, ML::Vec2());
				++phase;
				ge->StartCounter("FadeOut", 45);
			}
			break;
		case 2:
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
		//�X�e�[�W��
		{
			ML::Box2D textBox(500, 200, 500, 200);
			string text = "Stage " + to_string(ge->stage);
			res->font->Draw(textBox, text);
		}

		//�X�R�A
		{
			ML::Box2D textBox(1000, 200, 500, 200);
			string text = "Score:" + to_string(ge->score);
			res->font->Draw(textBox, text);
		}

		//�c�@
		{
			ML::Box2D textBox(1700, 800, 500, 200);
			string text = "x " + to_string(ge->remaining);
			res->font->Draw(textBox, text);
		}

		//�X�e�[�W�摜
		{
			ML::Box2D draw(400, 300, 960, 540);
			ML::Box2D src(0, 0, 1920, 1080);
			res->stageImg->Draw(draw, src);
		}

		//�v���C���摜(�c�@�p)
		{
			ML::Box2D draw(1550, 750, 200, 128);
			ML::Box2D src(0, 0, 100, 64);
			res->playerImg->Draw(draw, src);
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
	Object::Object() {	}
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
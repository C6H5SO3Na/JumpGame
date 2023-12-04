//-------------------------------------------------------------------
//�Q�[���{��
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_StageEditor.h"

#include "Task_Map2D.h"

#include "randomLib.h"
#include <assert.h>

namespace StageEditor
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		font = DG::Font::Create("���C���I", 30, 60);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		font.reset();
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
		deadCnt = 0;
		ge->isDead = false;

		//2D�J������`
		ge->camera2D = ML::Box2D(0, 0, ge->screen2DWidth, ge->screen2DHeight - 100);

		//�f�o�b�O�p�̋�`
		render2D_Priority[1] = 0.f;
		ge->debugRectLoad();

		//���^�X�N�̐���
		//�}�b�v�̐���
		auto map = Map2D::Object::Create(true);
		map->LoadMap("./data/Map/test3.csv");

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		ge->KillAll_G("�v���C��");
		ge->KillAll_G("�t�B�[���h");
		ge->KillAll_G("�G");
		ge->KillAll_G("�X�e�[�W���");
		ge->debugRectReset();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���

		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		//�}�b�v�̌��o�������炷
		ge->qa_Map = ge->GetTask<Map2D::Object>(Map2D::defGroupName);
		//�v���C���̌��o�������炷
		ge->qa_Player = ge->GetTask<Player::Object>(Player::defGroupName);
		auto inp = ge->in1->GetState();
		auto mouse = ge->mouse->GetState();
		mousePos = mouse.pos;

		if (mouse.LB.down) {
			isClicked = true;
			prePos = mousePos;
			preCamera2D = { ge->camera2D.x, ge->camera2D.y };
		}

		if (isClicked) {
			mapPos.x = prePos.x - mousePos.x + preCamera2D.x;
			mapPos.y = prePos.y - mousePos.y + preCamera2D.y;
		}
		else {
			mapPos.x = ge->camera2D.x;
			mapPos.y = ge->camera2D.y;
		}

		if (mouse.RB.down) {
			ge->qa_Map->map[(mousePos.y + ge->camera2D.y) / 64][(mousePos.x + ge->camera2D.x) / 64] = 1;
		}

		if (mouse.LB.up) {
			isClicked = false;
		}

		{
			//�J�����̍��W���X�V
			ge->camera2D.x = mapPos.x;
			ge->camera2D.y = mapPos.y;
			//�}�b�v�̊O�����f��Ȃ��悤�ɃJ�����𒲐�����
			Map2D::Object::SP map = ge->qa_Map;
			if (map != nullptr) {
				map->AdjustCameraPos();
			}
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ge->Dbg_ToDisplay(100, 100, "Game");
		ge->Dbg_ToDisplay(100, 120, "Push B1");

		ML::Box2D textBox(0, 0, 1000, 1000);
		string text = to_string(mousePos.x) + to_string(mousePos.y);
		res->font->DrawF(textBox, text, DG::Font::x1);
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
	Object::Object() :deadCnt(0) {	}
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
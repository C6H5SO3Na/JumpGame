//-------------------------------------------------------------------
//�Q�[���{��
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Game.h"
#include "Task_StartGame.h"
#include "Task_Result.h"

#include "Task_Map2D.h"
#include "Task_Player.h"
#include "Task_Enemy00.h"
#include "randomLib.h"
#include <assert.h>

namespace Game
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
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
		deadCnt = 0;
		ge->isDead = false;

		//2D�J������`
		ge->camera2D = ML::Box2D(-200, -100, ge->screen2DWidth, ge->screen2DHeight);

		//�f�o�b�O�p�̋�`
		render2D_Priority[1] = 0.f;
		ge->debugRectLoad();
		//���^�X�N�̐���
		//�}�b�v�̐���
		auto map = Map2D::Object::Create(true);
		map->LoadMap("./data/Map/test2.csv");

		//�X�|�[�� �v���C��
		auto player = Player::Object::Create(true);
		player->pos = map->playerSpawnPos;
		for (int i = 0; i < 10; ++i) {
			SpawnEnemy(ML::Vec2(1000.f, 800.f), 0);
		}
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
		ge->debugRectReset();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
			//�c�@��0�����ɂȂ�����Q�[���I�[�o�[��ʂɐ���
			if (ge->remaining < 0) {
				auto result = Result::Object::Create(true);
			}
			else {
				auto startGame = StartGame::Object::Create(true);
			}
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		//�G�̌��o�������炷
		ge->qa_Enemies = ge->GetTasks<BChara>("�G");
		//�}�b�v�̌��o�������炷
		ge->qa_Map = ge->GetTask<Map2D::Object>(Map2D::defGroupName);
		auto inp = ge->in1->GetState();

		//���ꂽ��
		if (ge->isDead) {
			++deadCnt;
			if (deadCnt >= 60 * 3) {//����Ă��΂炭�o�ߌ�
				Kill();//���̃^�X�N��
			}
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ge->Dbg_ToDisplay(100, 100, "Game");
		ge->Dbg_ToDisplay(100, 120, "Push B1");

		//�f�o�b�O��`�\��
		{
			ge->debugRectDraw();
		}
	}
	//-------------------------------------------------------------------
	//�G�̃X�|�[��
	void Object::SpawnEnemy(ML::Vec2 pos, int kind)
	{
		switch (kind) {
		case 0:
			auto enemy = Enemy00::Object::Create(true);
			enemy->pos = pos;
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
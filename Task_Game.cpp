//-------------------------------------------------------------------
//�Q�[���{��
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Game.h"
#include "Task_StartGame.h"
#include "Task_Result.h"
#include "Task_Ending.h"

#include "Task_Map2D.h"
#include "Task_StageInfo.h"
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
		ge->isDead = false;
		ge->isClear = false;

		//2D�J������`
		ge->camera2D = ML::Box2D(-200, -100, ge->screen2DWidth, ge->screen2DHeight);

		//�f�o�b�O�p�̋�`
		render2D_Priority[1] = 0.f;
		ge->debugRectLoad();

		//���^�X�N�̐���
		//�}�b�v�̐���
		auto map = Map2D::Object::Create(true);
		map->LoadMap("./data/map/stage"+to_string(ge->stage) + ".csv");

		//�G�̐���
		map->LoadEnemy("./data/enemy.csv");

		//�X�|�[�� �v���C��
		auto player = Player::Object::Create(true);
		player->SetPos(map->GetPlayerSpawnpos());

		//�X�e�[�W���\��
		auto stageInfo = StageInfo::Object::Create(true);

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
		ge->KillAll_G("�I�u�W�F�N�g");
		ge->KillAll_G("�X�e�[�W���");
		ge->debugRectReset();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
			
			if (ge->isDead) {
				//�c�@��0�����ɂȂ�����Q�[���I�[�o�[��ʂɐ���
				if (ge->remaining < 0) {
					auto result = Result::Object::Create(true);
				}
				else {
					--ge->remaining;
					auto startGame = StartGame::Object::Create(true);
				}
			}
			else if(ge->stage >= ge->maxStage){
				auto ending = Ending::Object::Create(true);
			}
			else {
				++ge->stage;
				auto startGame = StartGame::Object::Create(true);
			}
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		//�v���C���̌��o�������炷
		ge->qa_Player = ge->GetTask<Player::Object>(Player::defGroupName);
		//�G�̌��o�������炷
		ge->qa_Enemies = ge->GetTasks<BEnemy>(Enemy00::defGroupName);
		//�}�b�v�̌��o�������炷
		ge->qa_Map = ge->GetTask<Map2D::Object>(Map2D::defGroupName);
		auto inp = ge->in1->GetState();

		//�N���A������
		if (ge->isClear) {
			++cnt;
			switch (afterDeadPhase) {
			case 0:
				if (cnt >= 60 * 3) {//����Ă��΂炭�o�ߌ�
					++afterDeadPhase;
				}
				break;
			case 1:
				ge->CreateEffect(99, ML::Vec2());
				++afterDeadPhase;
			case 2:
				//���S�Ƀt�F�[�h�A�E�g������
				if (cnt >= 60 * 3 + 45) {
					Kill();//���̃^�X�N��
				}
			}
		}
		//���ꂽ��
		else if (ge->isDead) {
			++cnt;
			switch (afterDeadPhase) {
			case 0:
				if (cnt >= 60 * 3) {//����Ă��΂炭�o�ߌ�
					++afterDeadPhase;
				}
				break;
			case 1:
				ge->CreateEffect(99, ML::Vec2());
				++afterDeadPhase;
			case 2:
				//���S�Ƀt�F�[�h�A�E�g������
				if (cnt >= 60 * 3 + 45) {
					Kill();//���̃^�X�N��
				}
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
#ifdef isDebugMode
		ge->debugRectDraw();
#endif
	}
	//-------------------------------------------------------------------
	//�G�̃X�|�[��
	void Object::SpawnEnemy(ML::Vec2 pos, int kind)
	{
		auto enemy = Enemy00::Object::Create(true);
		enemy->SetPos(pos);
		enemy->SetType(static_cast<Enemy00::Object::Type>(kind));
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
	Object::Object() :cnt(),afterDeadPhase(), afterClearPhase() {	}
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
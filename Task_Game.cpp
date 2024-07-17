//-------------------------------------------------------------------
//�Q�[���{��
//-------------------------------------------------------------------
//#define DEBUG
#include "MyPG.h"
#include "Task_Game.h"
#include "Task_StartGame.h"
#include "Task_GameOver.h"
#include "Task_Ending.h"

#include "Task_Map2D.h"
#include "Task_EnemyManager.h"
#include "Task_StageInfo.h"
#include "Task_Player.h"
#include "Task_Enemy00.h"
#include "randomLib.h"
#include "sound.h"
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
		res = Resource::Create();

		//���f�[�^������
		ge->isDead = false;
		ge->isClear = false;
		//�f�o�b�O�p�̋�`
#ifdef DEBUG
		render2D_Priority[1] = 0.f;
		ge->debugRectLoad();
#endif
		//���^�X�N�̐���
		//�}�b�v�̐���
		auto map = Map2D::Object::Create(true);
		map->LoadMap("./data/map/stage"+to_string(ge->stage) + ".csv");

		//�G�}�l�[�W���[�̐���
		EnemyManager::Object::Create(true);

		//�X�|�[�� �v���C��
		Player::Object::Spawn(map->GetPlayerSpawnpos());

		//�X�e�[�W���\��
		auto stageInfo = StageInfo::Object::Create(true);

		//BGM
		bgm::LoadFile("Main", "./data/Sound/BGM/Game.mp3");
		bgm::LoadFile("StageClear", "./data/Sound/BGM/StageClear.mp3");
		bgm::Play("Main");

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		bgm::Stop("Main");
		//���f�[�^���^�X�N���
		ge->KillAll_G("�v���C��");
		ge->KillAll_G("�t�B�[���h");
		ge->KillAll_G("�G");
		ge->KillAll_G("�I�u�W�F�N�g");
		ge->KillAll_G("�X�e�[�W���");
		ge->debugRectReset();

		if (!ge->QuitFlag() && nextTaskCreate) {
			//�������p���^�X�N�̐���
			
			if (ge->isDead) {
				//�Q�[���I�[�o�[��ʂɐ���
				if (ge->remaining <= 0) {
					GameOver::Object::Create(true);
				}
				else {
					--ge->remaining;
					StartGame::Object::Create(true);
				}
			}
			else if(ge->stage >= ge->maxStage){
				Ending::Object::Create(true);
			}
			else {
				++ge->stage;
				StartGame::Object::Create(true);
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
		ge->qa_Enemies = ge->GetTasks<BChara>(Enemy00::defGroupName);
		//�}�b�v�̌��o�������炷
		ge->qa_Map = ge->GetTask<Map2D::Object>(Map2D::defGroupName);

		//�N���A������ or ���ꂽ��
		if (ge->isClear || ge->isDead) {
			switch (nextStagePhase) {
			case 0:
				if (ge->isClear){
					bgm::Stop("Main");
					bgm::Play("StageClear");
				}
				ge->StartCounter("ClearCount", 60 * 3);
				++nextStagePhase;
				break;
			case 1:
				if (ge->getCounterFlag("ClearCount") == ge->LIMIT) {
					++nextStagePhase;
				}
				break;
			case 2:
				ge->CreateEffect(99, ML::Vec2());
				ge->StartCounter("FadeoutCount", 45);
				++nextStagePhase;
				break;
			case 3:
				if (ge->getCounterFlag("FadeoutCount") == ge->LIMIT) {
					bgm::Stop("StageClear");
					Kill();//���̃^�X�N��
				}
				break;
			}
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		//�f�o�b�O��`�\��
#ifdef DEBUG
		ge->debugRectDraw();
#endif
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
	Object::Object() :cnt(),nextStagePhase(), afterClearPhase() {	}
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
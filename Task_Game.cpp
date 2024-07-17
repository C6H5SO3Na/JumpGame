//-------------------------------------------------------------------
//ゲーム本編
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
	//リソースの初期化
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		res = Resource::Create();

		//★データ初期化
		ge->isDead = false;
		ge->isClear = false;
		//デバッグ用の矩形
#ifdef DEBUG
		render2D_Priority[1] = 0.f;
		ge->debugRectLoad();
#endif
		//★タスクの生成
		//マップの生成
		auto map = Map2D::Object::Create(true);
		map->LoadMap("./data/map/stage"+to_string(ge->stage) + ".csv");

		//敵マネージャーの生成
		EnemyManager::Object::Create(true);

		//スポーン プレイヤ
		Player::Object::Spawn(map->GetPlayerSpawnpos());

		//ステージ情報表示
		auto stageInfo = StageInfo::Object::Create(true);

		//BGM
		bgm::LoadFile("Main", "./data/Sound/BGM/Game.mp3");
		bgm::LoadFile("StageClear", "./data/Sound/BGM/StageClear.mp3");
		bgm::Play("Main");

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		bgm::Stop("Main");
		//★データ＆タスク解放
		ge->KillAll_G("プレイヤ");
		ge->KillAll_G("フィールド");
		ge->KillAll_G("敵");
		ge->KillAll_G("オブジェクト");
		ge->KillAll_G("ステージ情報");
		ge->debugRectReset();

		if (!ge->QuitFlag() && nextTaskCreate) {
			//★引き継ぎタスクの生成
			
			if (ge->isDead) {
				//ゲームオーバー画面に推移
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
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		//プレイヤの検出数を減らす
		ge->qa_Player = ge->GetTask<Player::Object>(Player::defGroupName);
		//敵の検出数を減らす
		ge->qa_Enemies = ge->GetTasks<BChara>(Enemy00::defGroupName);
		//マップの検出数を減らす
		ge->qa_Map = ge->GetTask<Map2D::Object>(Map2D::defGroupName);

		//クリアしたら or やられたら
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
					Kill();//次のタスクへ
				}
				break;
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//デバッグ矩形表示
#ifdef DEBUG
		ge->debugRectDraw();
#endif
	}
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録

			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
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
	//リソースクラスの生成
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
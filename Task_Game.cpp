//-------------------------------------------------------------------
//ゲーム本編
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
		this->res = Resource::Create();

		//★データ初期化
		ge->isDead = false;
		ge->isClear = false;

		//2Dカメラ矩形
		ge->camera2D = ML::Box2D(-200, -100, ge->screen2DWidth, ge->screen2DHeight);

		//デバッグ用の矩形
		render2D_Priority[1] = 0.f;
		ge->debugRectLoad();

		//★タスクの生成
		//マップの生成
		auto map = Map2D::Object::Create(true);
		map->LoadMap("./data/map/stage"+to_string(ge->stage) + ".csv");

		//敵の生成
		map->LoadEnemy("./data/enemy.csv");

		//スポーン プレイヤ
		auto player = Player::Object::Create(true);
		player->SetPos(map->GetPlayerSpawnpos());

		//ステージ情報表示
		auto stageInfo = StageInfo::Object::Create(true);

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->KillAll_G("プレイヤ");
		ge->KillAll_G("フィールド");
		ge->KillAll_G("敵");
		ge->KillAll_G("オブジェクト");
		ge->KillAll_G("ステージ情報");
		ge->debugRectReset();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			
			if (ge->isDead) {
				//残機が0未満になったらゲームオーバー画面に推移
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
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		//プレイヤの検出数を減らす
		ge->qa_Player = ge->GetTask<Player::Object>(Player::defGroupName);
		//敵の検出数を減らす
		ge->qa_Enemies = ge->GetTasks<BEnemy>(Enemy00::defGroupName);
		//マップの検出数を減らす
		ge->qa_Map = ge->GetTask<Map2D::Object>(Map2D::defGroupName);
		auto inp = ge->in1->GetState();

		//クリアしたら
		if (ge->isClear) {
			++cnt;
			switch (afterDeadPhase) {
			case 0:
				if (cnt >= 60 * 3) {//やられてしばらく経過後
					++afterDeadPhase;
				}
				break;
			case 1:
				ge->CreateEffect(99, ML::Vec2());
				++afterDeadPhase;
			case 2:
				//完全にフェードアウトしたら
				if (cnt >= 60 * 3 + 45) {
					Kill();//次のタスクへ
				}
			}
		}
		//やられたら
		else if (ge->isDead) {
			++cnt;
			switch (afterDeadPhase) {
			case 0:
				if (cnt >= 60 * 3) {//やられてしばらく経過後
					++afterDeadPhase;
				}
				break;
			case 1:
				ge->CreateEffect(99, ML::Vec2());
				++afterDeadPhase;
			case 2:
				//完全にフェードアウトしたら
				if (cnt >= 60 * 3 + 45) {
					Kill();//次のタスクへ
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ge->Dbg_ToDisplay(100, 100, "Game");
		ge->Dbg_ToDisplay(100, 120, "Push B1");

		//デバッグ矩形表示
#ifdef isDebugMode
		ge->debugRectDraw();
#endif
	}
	//-------------------------------------------------------------------
	//敵のスポーン
	void Object::SpawnEnemy(ML::Vec2 pos, int kind)
	{
		auto enemy = Enemy00::Object::Create(true);
		enemy->SetPos(pos);
		enemy->SetType(static_cast<Enemy00::Object::Type>(kind));
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
	Resource::~Resource() { this->Finalize(); }
}
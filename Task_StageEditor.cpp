//-------------------------------------------------------------------
//ゲーム本編
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
	//リソースの初期化
	bool  Resource::Initialize()
	{
		font = DG::Font::Create("メイリオ", 30, 60);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		font.reset();
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
		deadCnt = 0;
		ge->isDead = false;

		//2Dカメラ矩形
		ge->camera2D = ML::Box2D(0, 0, ge->screen2DWidth, ge->screen2DHeight - 100);

		//デバッグ用の矩形
		render2D_Priority[1] = 0.f;
		ge->debugRectLoad();

		//★タスクの生成
		//マップの生成
		auto map = Map2D::Object::Create(true);
		map->LoadMap("./data/Map/test3.csv");

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
		ge->KillAll_G("ステージ情報");
		ge->debugRectReset();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成

		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		//マップの検出数を減らす
		ge->qa_Map = ge->GetTask<Map2D::Object>(Map2D::defGroupName);
		//プレイヤの検出数を減らす
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
			//カメラの座標を更新
			ge->camera2D.x = mapPos.x;
			ge->camera2D.y = mapPos.y;
			//マップの外側が映らないようにカメラを調整する
			Map2D::Object::SP map = ge->qa_Map;
			if (map != nullptr) {
				map->AdjustCameraPos();
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ge->Dbg_ToDisplay(100, 100, "Game");
		ge->Dbg_ToDisplay(100, 120, "Push B1");

		ML::Box2D textBox(0, 0, 1000, 1000);
		string text = to_string(mousePos.x) + to_string(mousePos.y);
		res->font->DrawF(textBox, text, DG::Font::x1);
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
	Object::Object() :deadCnt(0) {	}
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
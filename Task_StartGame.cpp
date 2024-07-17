//-------------------------------------------------------------------
//ゲーム開始画面
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_StartGame.h"
#include "Task_Game.h"
#include <assert.h>

namespace StartGame
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		font = DG::Font::Create("ＭＳ 明朝", 30, 60);
		stageImg = DG::Image::Create("./data/image/Stage" + to_string(ge->stage) + ".jpg");
		playerImg = DG::Image::Create("./data/image/Idle_KG_1.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		font.reset();
		stageImg.reset();
		playerImg.reset();
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

		//変数初期化
		render2D_Priority[1] = 1.0f;
		phase = Phase::FadeIn;

		//カウント開始
		ge->StartCounter("FadeIn", 45);
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放

		if (!ge->QuitFlag() && nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto game = Game::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		switch (phase) {
		case Phase::FadeIn:
			//フェードインまで待つ
			if (ge->getCounterFlag("FadeIn") == ge->LIMIT) {
				ge->StartCounter("Start", 120);
				phase = Phase::ShowInfo;
			}
			break;
		case Phase::ShowInfo:
			//少し待ったら
			if (ge->getCounterFlag("Start") == ge->LIMIT) {
				//2Dカメラ矩形(エフェクトがずれないように)
				ge->camera2D = ML::Box2D(0, 0, ge->screen2DWidth, ge->screen2DHeight);

				//フェードのエフェクト
				ge->CreateEffect(99, ML::Vec2(0.f, 0.f));
				ge->StartCounter("FadeOut", 45);
				phase = Phase::FadeOut;
			}
			break;
		case Phase::FadeOut:
			//完全にフェードアウトしたら
			if (ge->getCounterFlag("FadeOut") == ge->LIMIT) {
				Kill();//次のタスクへ
			}
			break;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		DrawStageNum();
		DrawScore();
		DrawRemain();
		DrawImageStage();
	}

	//-------------------------------------------------------------------
	//ステージ数描画
	void  Object::DrawStageNum() const
	{
		ML::Box2D textBox(500, 200, 500, 200);
		string text = "Stage " + to_string(ge->stage);
		res->font->Draw(textBox, text);
	}

	//-------------------------------------------------------------------
	//スコア描画
	void  Object::DrawScore() const
	{
		ML::Box2D textBox(1000, 200, 500, 200);
		string text = "Score:" + to_string(ge->score);
		res->font->Draw(textBox, text);
	}

	//-------------------------------------------------------------------
	//残機描画
	void  Object::DrawRemain() const
	{
		DrawPlayerImage();
		DrawRemainNum();
	}

	//-------------------------------------------------------------------
	//残機のプレイヤアイコン描画
	void  Object::DrawPlayerImage() const
	{
		ML::Box2D draw(1550, 750, 200, 128);
		ML::Box2D src(0, 0, 100, 64);
		res->playerImg->Draw(draw, src);
	}

	//-------------------------------------------------------------------
	//残機数描画
	void  Object::DrawRemainNum() const
	{
		ML::Box2D textBox(1700, 800, 500, 200);
		string text = "x " + to_string(ge->remaining);
		res->font->Draw(textBox, text);
	}

	//-------------------------------------------------------------------
	//ステージ画像描画
	void  Object::DrawImageStage() const
	{
		ML::Box2D draw(400, 300, 960, 540);
		ML::Box2D src(0, 0, 1920, 1080);
		res->stageImg->Draw(draw, src);
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
	Object::Object() :phase(Phase::None) {	}
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
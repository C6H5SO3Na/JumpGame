//-------------------------------------------------------------------
//ゲームオーバー、結果表示
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_GameOver.h"
#include "Task_Title.h"
#include "Task_PressSKey.h"
#include "sound.h"
#include "easing.h"

namespace GameOver
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		img = DG::Image::Create("./data/image/GameOver.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		img.reset();
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
		render2D_Priority[1] = 1.0f;
		phase = Phase::PlayingJingle;

		//★タスクの生成

		//BGM(ジングル)
		se::LoadFile("GameOver", "./data/Sound/BGM/GameOver.wav");
		se::Play("GameOver");

		//カウンター開始
		ge->StartCounter("Jingle", 60 * 3);
		//todo:イージング適用　GAME OVERロゴを分離する
		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto title = Title::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();
		//段階毎の処理
		switch (phase)
		{
			//イージング中
		case Phase::PlayingJingle:
			if (ge->getCounterFlag("Jingle") == ge->LIMIT) {
				PressSKey::Object::Create(true);
				phase = Phase::PressSKey;
			}
			break;

			//PressSKeyが点滅している間
		case Phase::PressSKey:
			//PressStartKeyが消えたら画面推移
			if (ge->GetTask<PressSKey::Object>(PressSKey::defGroupName, PressSKey::defName) == nullptr)
			{
				ge->StartCounter("Fadeout", 45); //フェードは90フレームなので半分の45で切り替え
				//フェードのエフェクト
				ge->CreateEffect(99, ML::Vec2(0.f, 0.f));
				phase = Phase::FadeOut;
			}
			break;

			//フェードアウト中
		case Phase::FadeOut:
			if (ge->getCounterFlag("Fadeout") == ge->LIMIT) {
				Kill();
			}
			break;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(0, 0, ge->screen2DWidth, ge->screen2DHeight);
		ML::Box2D src(0, 0, 1920, 1080);
		res->img->Draw(draw, src);
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
	Object::Object() : phase(Phase::None) {	}
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
//-------------------------------------------------------------------
//タイトル画面
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Title.h"
#include "Task_StartGame.h"
#include "Task_PressSKey.h"
#include "sound.h"
#include "easing.h"

namespace Title
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imgLogo = DG::Image::Create("./data/image/TitleLogo.png");
		imgBG = DG::Image::Create("./data/image/Title.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		imgLogo.reset();
		imgBG.reset();
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
		phase = Phase::IsEasing;

		//イージングセット&開始
		easing::Set("タイトル文字", easing::EXPOOUT, -100, 100, 60);
		easing::Start("タイトル文字");

		//BGM
		bgm::LoadFile("Title", "./data/Sound/BGM/Title.mp3");
		bgm::Play("Title");

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		//BGM停止
		bgm::Stop("Title");
		ge->KillAll_G("UI");

		//ゲーム開始時の準備
		ge->stage = 1;
		ge->remaining = 3;
		ge->score = 0;

		if (!ge->QuitFlag() && nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto game = StartGame::Object::Create(true);
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
		case Phase::IsEasing:
			if (easing::GetState("タイトル文字") == easing::EQ_STATE::EQ_END) {
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
		easing::UpDate();
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		DrawBG();
		DrawLogo();
	}

	//-------------------------------------------------------------------
	//背景描画
	void  Object::DrawBG() const
	{
		ML::Box2D src(0, 0, 1920, 1080);
		ML::Box2D draw(0, 0, ge->screen2DWidth, ge->screen2DHeight);
		res->imgBG->Draw(draw, src);
	}

	//-------------------------------------------------------------------
	//ロゴ描画
	void  Object::DrawLogo() const
	{
		ML::Box2D src(0, 0, 1228, 197);
		ML::Box2D draw(ge->screen2DWidth / 4 - 100, 0, src.w, src.h);
		draw.y += static_cast<int>(easing::GetPos("タイトル文字"));//イージングを適用
		res->imgLogo->Draw(draw, src);
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
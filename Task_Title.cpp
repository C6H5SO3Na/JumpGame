//-------------------------------------------------------------------
//タイトル画面
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Title.h"
#include "Task_StartGame.h"
#include "Task_StageEditor.h"
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
		render2D_Priority[1] = 1.0f;
		isFadeout = false;
		//イージングセット
		easing::Set("タイトル文字", easing::EXPOOUT, -100, 100, 60);
		//BGM
		bgm::LoadFile("Title", "./data/Sound/BGM/Title.mp3");
		bgm::Play("Title");
		//★タスクの生成
		auto pressStartKey = PressSKey::Object::Create(true);

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
		if (!ge->QuitFlag() && this->nextTaskCreate) {
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
		case 0:
			easing::Start("タイトル文字");
			++phase;
			break;

		case 1:
			if (easing::GetState("タイトル文字") == easing::EQ_STATE::EQ_END) {
				++phase;
			}
			break;
		case 2:
			//PressStartKeyが消えたら画面推移
			if (ge->GetTask<PressSKey::Object>(PressSKey::defGroupName, PressSKey::defName) == nullptr)
			{
				if (!isFadeout) {
					ge->StartCounter("Fadeout", 45); //フェードは90フレームなので半分の45で切り替え
					ge->CreateEffect(99, ML::Vec2());
					isFadeout = true;
				}
			}

			if (ge->getCounterFlag("Fadeout") == ge->LIMIT) {
				this->Kill();
			}
			break;
		}
		easing::UpDate();
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//
		{
			ML::Box2D draw(0, 0, ge->screen2DWidth, ge->screen2DHeight);
			ML::Box2D src(0, 0, 1920, 1080);
			res->imgBG->Draw(draw, src);
		}

		//テキストボックス
		ML::Box2D src(0, 0, 1228, 197);
		ML::Box2D draw(ge->screen2DWidth / 4 - 100, 0, src.w, src.h);
		string text;
		//段階毎の処理
		switch (phase)
		{
		case 2://fall through
		case 1://fall through
		case 0:
			draw.y += static_cast<int>(easing::GetPos("タイトル文字"));
			res->imgLogo->Draw(draw, src);
			break;
		}
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
	Object::Object():isFadeout() {	}
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
//-------------------------------------------------------------------
//敵マネージャー
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_EnemyManager.h"
#include "Task_Enemy00.h"
#include <assert.h>

namespace EnemyManager
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
		LoadEnemy("./data/enemy/enemyStage" + to_string(ge->stage) + ".csv");

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
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
	}
	//-------------------------------------------------------------------
//敵の配置のロード
	bool Object::LoadEnemy(const string& fpath_)
	{
		ifstream fin(fpath_);
		if (!fin) {
			assert(!"読み込み失敗");
			return false;
		}

		int n;//敵の数
		{
			string lineText;
			getline(fin, lineText);
			istringstream  ss_lt(lineText);
			string  tc;
			getline(ss_lt, tc, ',');

			stringstream ss;
			ss << tc;
			ss >> n;
		}

		for (int i = 0; i < n; ++i) {
			string lineText;
			getline(fin, lineText);
			istringstream  ss_lt(lineText);
			ML::Vec2 pos;
			int enemyKind;
			string  tc;
			{
				stringstream ss;
				getline(ss_lt, tc, ',');
				ss << tc;
				ss >> pos.x;
			}

			{
				stringstream ss;
				getline(ss_lt, tc, ',');
				ss << tc;
				ss >> pos.y;
			}

			{
				stringstream ss;
				getline(ss_lt, tc, ',');
				ss << tc;
				ss >> enemyKind;
			}
			Enemy00::Object::Spawn(pos, enemyKind);
		}

		//ファイルを閉じる
		fin.close();
		return true;
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
	Object::Object() {	}
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
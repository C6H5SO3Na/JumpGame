//-------------------------------------------------------------------
//マップ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Map2D.h"
#include  "Task_Enemy00.h"
#include  "Task_GoalFlag.h"
#include  "Task_Game.h"
#include <assert.h>

namespace Map2D
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		img = DG::Image::Create("./data/image/Tile2.png");
		imgBG = DG::Image::Create("./data/image/GameBG.png");
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
		this->res = Resource::Create();

		//★データ初期化
		chipSize = 64;//チップのサイズ

		render2D_Priority[1] = 0.9f;
		//マップのゼロクリア
		for (int y = 0; y < size(map); ++y) {
			for (int x = 0; x < size(map[y]); ++x) {
				map[y][x] = 0;
			}
		}

		//マップチップ情報の初期化
		for (int c = 0; c < chipKind; ++c) {
			int x = (c % 12);
			int y = (c / 12);
			chip[c] = ML::Box2D(x * chipSize, y * chipSize, chipSize, chipSize);
		}
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放

		if (!ge->QuitFlag() && this->nextTaskCreate) {
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
		for (int i = 0; i < 10; ++i) {
			ML::Box2D src(0, 0, 1920, 1080);
			ML::Box2D draw(i * src.w, 0, src.w, src.h);
			draw = ge->ApplyCamera2D(draw);
			res->imgBG->Draw(draw, src);
		}

		for (int y = 0; y < mapSize[Y]; ++y) {
			for (int x = 0; x < mapSize[X]; ++x) {
				DrawMapChip(map[y][x], x, y);
			}
		}
	}
	//-------------------------------------------------------------------
	//マップチップ描画
	void Object::DrawMapChip(const int& map, const int& x, const int& y)
	{
		if (map == -1) {
			return;//マップ番号が-1(空白)の場合は描画しない
		}
		ML::Box2D draw(x * chipSize, y * chipSize, chipSize, chipSize);
		ML::Box2D src(map % 12 * chipSize, map / 12 * chipSize, chipSize, chipSize);
		draw = ge->ApplyCamera2D(draw);
		res->img->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//マップのロード
	bool Object::LoadMap(const string& fpath_)
	{
		ifstream fin(fpath_);
		if (!fin.is_open()) {
			assert(!"読み込み失敗");
			return false;
		}
		//マップチップの縦横情報を入手
		{
			string lineText;
			getline(fin, lineText);
			istringstream  ss_lt(lineText);
			for (int i = 0; i < 2; ++i) {
				string  tc;
				getline(ss_lt, tc, ',');

				stringstream ss;
				ss << tc;
				ss >> mapSize[i];
			}
		}

		//プレイヤのスポーン地点を入手
		{
			string lineText;
			getline(fin, lineText);
			istringstream  ss_lt(lineText);
			float pos[2] = {};//x:0,y:0
			for (int i = 0; i < 2; ++i) {
				string  tc;
				getline(ss_lt, tc, ',');

				stringstream ss;
				ss << tc;
				ss >> pos[i];
			}
			playerSpawnPos = ML::Vec2(static_cast<float>(pos[X] * chipSize + chipSize / 2),
				static_cast<float>(pos[Y] * chipSize + chipSize / 2));
		}

		//マップの当たり判定を定義
		hitBase = ML::Box2D(0, 0, mapSize[X] * chipSize, mapSize[Y] * chipSize);

		for (int y = 0; y < mapSize[Y]; ++y) {
			string lineText;
			getline(fin, lineText);
			istringstream  ss_lt(lineText);
			for (int x = 0; x < mapSize[X]; ++x) {
				string  tc;
				getline(ss_lt, tc, ',');

				stringstream ss;
				ss << tc;
				ss >> map[y][x];

				if (map[y][x] == 100) {
					shared_ptr<GoalFlag::Object> goalFlag = GoalFlag::Object::Create(true);
					goalFlag->SetPos(ML::Vec2(x * 64.f, y * 64.f));
					map[y][x] = -1;//当たり判定を無効にする
				}
			}
		}

		//ファイルを閉じる
		fin.close();
		return true;
	}
	//-------------------------------------------------------------------
	//敵の配置のロード
	bool Object::LoadEnemy(const string& fpath_)
	{
		auto game = ge->GetTask<Game::Object>("本編統括");
		ifstream fin(fpath_);
		if (!fin.is_open()) {
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
			game->SpawnEnemy(pos, enemyKind);
		}

		//ファイルを閉じる
		fin.close();
		return true;
	}
	//-------------------------------------------------------------------
	//当たり判定
	bool Object::CheckHit(const ML::Box2D& hit_)
	{
		ML::Rect r = { hit_.x,
						hit_.y,
						hit_.x + hit_.w,
						hit_.y + hit_.h };
		//矩形がマップ外に出ていたらサイズを変更する
		ML::Rect m = {
			hitBase.x,
			hitBase.y,
			hitBase.x + hitBase.w,
			hitBase.x + hitBase.h
		};
		//範囲外を壁にする方法
		if (r.left < m.left) { return true; }		//左に飛び出している
		//if (r.top < m.top) { return true; }			//上に飛び出している//上下は判定を無効にする
		if (r.right > m.right) { return true; }		//右に飛び出している
		//if (r.bottom > m.bottom) { return true; }	//下に飛び出している//上下は判定を無効にする
		//範囲外を壁にしない方法
		//r.left = max(r.left, m.left);			//左に飛び出している
		r.top = max(r.top, m.top);				//上に飛び出している//上下は判定を無効にする
		//r.right = min(r.right, m.right);		//右に飛び出している
		r.bottom = min(r.bottom, m.bottom);		//下に飛び出している//上下は判定を無効にする

		//ループ範囲調整
		int sx = r.left / chipSize;
		int sy = r.top / chipSize;
		int ex = (r.right - 1) / chipSize;
		int ey = (r.bottom - 1) / chipSize;

		//範囲内の障害物を探す
		for (int y = sy; y <= ey; ++y) {
			for (int x = sx; x <= ex; ++x) {
				if (map[y][x] != -1) {
					return true;
				}
			}
		}
		return false;
	}
	//-------------------------------------------------------------------
	//マップ外を見せないようにカメラを位置調整する
	void Object::AdjustCameraPos()
	{
		//カメラとマップの範囲を用意
		ML::Rect c = {
			ge->camera2D.x,
			ge->camera2D.y,
			ge->camera2D.x + ge->camera2D.w,
			ge->camera2D.y + ge->camera2D.h
		};
		ML::Rect m = {
			hitBase.x,
			hitBase.y,
			hitBase.x + hitBase.w,
			hitBase.x + hitBase.h
		};
		//カメラの位置を調整
		if (c.right > m.right) {
			ge->camera2D.x = m.right - ge->camera2D.w;
		}

		if (c.left < m.left) {
			ge->camera2D.x = m.left;
		}

		if (ge->qa_Player != nullptr || c.bottom > m.bottom) {
			ge->camera2D.y = m.bottom - ge->camera2D.h;
		}

		if (ge->qa_Player != nullptr || c.top < m.top) {
			ge->camera2D.y = m.top;
		}

		//マップがカメラより小さい場合
		if (hitBase.w < ge->camera2D.w) {
			ge->camera2D.x = m.left;
		}

		if (hitBase.h < ge->camera2D.h) {
			ge->camera2D.y = m.top;
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
	Object::Object() :chipSize(0), map{}, mapSize{} {	}
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
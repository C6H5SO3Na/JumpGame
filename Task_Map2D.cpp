//-------------------------------------------------------------------
//マップ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Map2D.h"
#include  "Task_Enemy00.h"
#include  "Task_GoalFlag.h"
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
		res = Resource::Create();

		//★データ初期化
		chipSize = 64;//チップのサイズ

		render2D_Priority[1] = 0.9f;

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
		DrawBG();
		DrawMapChips();
	}
	//-------------------------------------------------------------------
	//背景を描画
	void Object::DrawBG() const
	{
		//マップチップの長さから何枚描画するかを判断
		for (int i = 0; i <= chipSize * mapSize[X] / 1920; ++i) {
			ML::Box2D src(0, 0, 1920, 1080);
			ML::Box2D draw(i * src.w, 0, src.w, src.h);
			draw = ge->ApplyCamera2D(draw);
			res->imgBG->Draw(draw, src);
		}
	}
	//-------------------------------------------------------------------
	//マップチップを描画
	void Object::DrawMapChips() const
	{
		for (int y = 0; y < mapSize[Y]; ++y) {
			for (int x = 0; x < mapSize[X]; ++x) {
				DrawMapChip(map[y][x], x, y);
			}
		}
	}
	//-------------------------------------------------------------------
	//マップチップを1枚描画
	void Object::DrawMapChip(const int& map, const int& x, const int& y) const
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
	//配列に一行分のデータをインポート
	template<typename T>
	void Object::ImportArray(ifstream& fin, T* const arr, const int& n)
	{
		string lineText;
		getline(fin, lineText);
		istringstream  ss_lt(lineText);
		for (int i = 0; i < n; ++i) {
			string  tc;
			getline(ss_lt, tc, ',');

			stringstream ss;
			ss << tc;
			ss >> *(arr + i);
		}
	}

	//-------------------------------------------------------------------
	//二次元配列に複数行分のデータをインポート
	template<typename T>
	void Object::ImportArray(ifstream& fin, T* const arr, const int& xSize, const int& ySize)
	{
		for (int y = 0; y < ySize; ++y) {
			string lineText;
			getline(fin, lineText);
			istringstream  ss_lt(lineText);
			for (int x = 0; x < xSize; ++x) {
				string  tc;
				getline(ss_lt, tc, ',');

				stringstream ss;
				ss << tc;
				//ss >> map[y][x];
				ss >> *(arr + x + y * 1000);
			}
		}
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
		ImportArray(fin, mapSize, 2);

		//プレイヤのスポーン地点を入手
		float pos[2] = {};//x:0,y:0
		ImportArray(fin, pos, 2);
		playerSpawnPos = ML::Vec2(static_cast<float>(pos[X] * chipSize + chipSize / 2),
			static_cast<float>(pos[Y] * chipSize + chipSize / 2));

		//マップの当たり判定を定義
		hitBase = ML::Box2D(0, 0, mapSize[X] * chipSize, mapSize[Y] * chipSize);

		ImportArray(fin, &map[0][0], mapSize[X], mapSize[Y]);

		for (int y = 0; y < mapSize[Y]; ++y) {
			for (int x = 0; x < mapSize[X]; ++x) {
				if (map[y][x] == 100) {
					GoalFlag::Object::Spawn(ML::Vec2(x * 64.f, y * 64.f));
					map[y][x] = -1;//当たり判定を無効にする
				}
			}
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
						hit_.y + hit_.h
		};
		//矩形がマップ外に出ていたらサイズを変更する
		ML::Rect m = {
			hitBase.x,
			hitBase.y,
			hitBase.x + hitBase.w,
			hitBase.y + hitBase.h
		};
		//範囲外を壁にする方法
		if (r.left < m.left) { return true; }		//左に飛び出している
		if (r.right > m.right) { return true; }		//右に飛び出している

		//範囲外を壁にしない方法
		r.top = max(r.top, m.top);				//上に飛び出している
		r.bottom = min(r.bottom, m.bottom);		//下に飛び出している

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
			hitBase.y + hitBase.h
		};
		//カメラの位置を調整
		if (c.right > m.right) {
			ge->camera2D.x = m.right - ge->camera2D.w;
		}

		if (c.left < m.left) {
			ge->camera2D.x = m.left;
		}

		if (c.bottom > m.bottom) {
			ge->camera2D.y = m.bottom - ge->camera2D.h;
		}

		if (c.top < m.top) {
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
	Resource::~Resource() { Finalize(); }
}
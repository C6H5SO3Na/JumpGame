#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//マップ
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace Map2D
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("フィールド");	//グループ名
	const  string  defName("マップ");	//タスク名
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()	override;
		bool  Finalize()	override;
		Resource();
	public:
		~Resource();
		typedef shared_ptr<Resource>	SP;
		typedef weak_ptr<Resource>		WP;
		static WP  instance;
		static Resource::SP  Create();
		//共有する変数はここに追加する
		DG::Image::SP img, imgBG;
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
		//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//「初期化」タスク生成時に１回だけ行う処理
		void  UpDate()			override;//「実行」１フレーム毎に行う処理
		void  Render2D_AF()		override;//「2D描画」１フレーム毎に行う処理
		bool  Finalize();		//「終了」タスク消滅時に１回だけ行う処理
		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	private:
		//追加したい変数・メソッドはここに追加する
		int mapSize[2];//0:x,1:y
		int chipSize;//チップのサイズ
		ML::Box2D hitBase;//ピクセル単位のマップサイズを持つ
		enum { chipKind = 84 };//マップチップの種類
		enum { X = 0, Y = 1 };//座標用
		ML::Box2D chip[chipKind];
		ML::Vec2 playerSpawnPos;

		void DrawBG() const;//背景描画
		void DrawMapChips() const;//マップチップを描画
		void DrawMapChip(const int& map, const int& x, const int& y) const;//マップチップを1枚描画

		//配列からロード
		template<typename T>
		void ImportArray(ifstream& fin, T* const arr, const int& n);
		template<typename T>
		void ImportArray(ifstream& fin, T* const arr, const int& xSize, const int& ySize);

		int map[1000][1000] = {};//一応1000*1000

	public:
		bool LoadMap(const string& fpath_);
		void AdjustCameraPos();//マップ外を見せないようにカメラを位置調整
		bool CheckHit(const ML::Box2D& hit_);
		ML::Vec2 GetPlayerSpawnpos()const { return playerSpawnPos; }
	};
}
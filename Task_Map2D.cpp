//-------------------------------------------------------------------
//�}�b�v
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
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		img = DG::Image::Create("./data/image/Tile2.png");
		imgBG = DG::Image::Create("./data/image/GameBG.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		img.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		res = Resource::Create();

		//���f�[�^������
		chipSize = 64;//�`�b�v�̃T�C�Y

		render2D_Priority[1] = 0.9f;

		//�}�b�v�`�b�v���̏�����
		for (int c = 0; c < chipKind; ++c) {
			int x = (c % 12);
			int y = (c / 12);
			chip[c] = ML::Box2D(x * chipSize, y * chipSize, chipSize, chipSize);
		}
		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���

		if (!ge->QuitFlag() && nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		DrawBG();
		DrawMapChips();
	}
	//-------------------------------------------------------------------
	//�w�i��`��
	void Object::DrawBG() const
	{
		//�}�b�v�`�b�v�̒������牽���`�悷�邩�𔻒f
		for (int i = 0; i <= chipSize * mapSize[X] / 1920; ++i) {
			ML::Box2D src(0, 0, 1920, 1080);
			ML::Box2D draw(i * src.w, 0, src.w, src.h);
			draw = ge->ApplyCamera2D(draw);
			res->imgBG->Draw(draw, src);
		}
	}
	//-------------------------------------------------------------------
	//�}�b�v�`�b�v��`��
	void Object::DrawMapChips() const
	{
		for (int y = 0; y < mapSize[Y]; ++y) {
			for (int x = 0; x < mapSize[X]; ++x) {
				DrawMapChip(map[y][x], x, y);
			}
		}
	}
	//-------------------------------------------------------------------
	//�}�b�v�`�b�v��1���`��
	void Object::DrawMapChip(const int& map, const int& x, const int& y) const
	{
		if (map == -1) {
			return;//�}�b�v�ԍ���-1(��)�̏ꍇ�͕`�悵�Ȃ�
		}
		ML::Box2D draw(x * chipSize, y * chipSize, chipSize, chipSize);
		ML::Box2D src(map % 12 * chipSize, map / 12 * chipSize, chipSize, chipSize);
		draw = ge->ApplyCamera2D(draw);
		res->img->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//�z��Ɉ�s���̃f�[�^���C���|�[�g
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
	//�񎟌��z��ɕ����s���̃f�[�^���C���|�[�g
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
	//�}�b�v�̃��[�h
	bool Object::LoadMap(const string& fpath_)
	{
		ifstream fin(fpath_);
		if (!fin.is_open()) {
			assert(!"�ǂݍ��ݎ��s");
			return false;
		}
		//�}�b�v�`�b�v�̏c���������
		ImportArray(fin, mapSize, 2);

		//�v���C���̃X�|�[���n�_�����
		float pos[2] = {};//x:0,y:0
		ImportArray(fin, pos, 2);
		playerSpawnPos = ML::Vec2(static_cast<float>(pos[X] * chipSize + chipSize / 2),
			static_cast<float>(pos[Y] * chipSize + chipSize / 2));

		//�}�b�v�̓����蔻����`
		hitBase = ML::Box2D(0, 0, mapSize[X] * chipSize, mapSize[Y] * chipSize);

		ImportArray(fin, &map[0][0], mapSize[X], mapSize[Y]);

		for (int y = 0; y < mapSize[Y]; ++y) {
			for (int x = 0; x < mapSize[X]; ++x) {
				if (map[y][x] == 100) {
					GoalFlag::Object::Spawn(ML::Vec2(x * 64.f, y * 64.f));
					map[y][x] = -1;//�����蔻��𖳌��ɂ���
				}
			}
		}

		//�t�@�C�������
		fin.close();
		return true;
	}
	//-------------------------------------------------------------------
	//�����蔻��
	bool Object::CheckHit(const ML::Box2D& hit_)
	{
		ML::Rect r = { hit_.x,
						hit_.y,
						hit_.x + hit_.w,
						hit_.y + hit_.h
		};
		//��`���}�b�v�O�ɏo�Ă�����T�C�Y��ύX����
		ML::Rect m = {
			hitBase.x,
			hitBase.y,
			hitBase.x + hitBase.w,
			hitBase.y + hitBase.h
		};
		//�͈͊O��ǂɂ�����@
		if (r.left < m.left) { return true; }		//���ɔ�яo���Ă���
		if (r.right > m.right) { return true; }		//�E�ɔ�яo���Ă���

		//�͈͊O��ǂɂ��Ȃ����@
		r.top = max(r.top, m.top);				//��ɔ�яo���Ă���
		r.bottom = min(r.bottom, m.bottom);		//���ɔ�яo���Ă���

		//���[�v�͈͒���
		int sx = r.left / chipSize;
		int sy = r.top / chipSize;
		int ex = (r.right - 1) / chipSize;
		int ey = (r.bottom - 1) / chipSize;

		//�͈͓��̏�Q����T��
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
	//�}�b�v�O�������Ȃ��悤�ɃJ�������ʒu��������
	void Object::AdjustCameraPos()
	{
		//�J�����ƃ}�b�v�͈̔͂�p��
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
		//�J�����̈ʒu�𒲐�
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

		//�}�b�v���J������菬�����ꍇ
		if (hitBase.w < ge->camera2D.w) {
			ge->camera2D.x = m.left;
		}

		if (hitBase.h < ge->camera2D.h) {
			ge->camera2D.y = m.top;
		}
	}
	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^

			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
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
	//���\�[�X�N���X�̐���
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
//-------------------------------------------------------------------
//�}�b�v
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Map2D.h"
#include  "Task_Enemy00.h"
#include "assert.h"

namespace Map2D
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		img = DG::Image::Create("./data/image/Dark_lvl0.png");
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
		this->res = Resource::Create();

		//���f�[�^������
		chipSize = 64;//�`�b�v�̃T�C�Y
		sizeX = 41;
		sizeY = 16;
		render2D_Priority[1] = 0.9f;
		//�}�b�v�̃[���N���A
		for (int y = 0; y < size(map); ++y) {
			for (int x = 0; x < size(map[y]); ++x) {
				map[y][x] = 0;
			}
		}
		hitBase = ML::Box2D(0, 0, sizeX * chipSize, sizeY * chipSize);

		//�}�b�v�`�b�v���̏�����
		for (int c = 0; c < chipKind; ++c) {
			int x = (c % 8);
			int y = (c / 8);
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

		if (!ge->QuitFlag() && this->nextTaskCreate) {
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
		for (int y = 0; y < sizeY; ++y) {
			for (int x = 0; x < sizeX; ++x) {
				DrawMapChip(map[y][x], x, y);
			}
		}
	}
	//-------------------------------------------------------------------
	//�}�b�v�`�b�v�`��
	void Object::DrawMapChip(int map, int x, int y)
	{
		if (map < 0) {
			return;//�}�b�v�ԍ���0����(��,�I�u�W�F�N�g�Ȃ�)�̏ꍇ�͕`�悵�Ȃ�
		}
		ML::Box2D draw(x * chipSize, y * chipSize, chipSize, chipSize);
		ML::Box2D src(map % 10 * chipSize, map / 10 * chipSize, chipSize, chipSize);
		ge->ApplyCamera2D(draw);
		res->img->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//�}�b�v�̃��[�h
	bool Object::LoadMap(const string& fpath_)
	{
		ifstream fin(fpath_);
		if (!fin) {
			assert(!"�ǂݍ��ݎ��s");
			return false;
		}


		for (int y = 0; y < sizeY; ++y) {
			string lineText;
			getline(fin, lineText);
			istringstream  ss_lt(lineText);
			for (int x = 0; x < sizeX; ++x) {
				string  tc;
				getline(ss_lt, tc, ',');
				stringstream ss;
				ss << tc;
				ss >> map[y][x];
				if (map[y][x] == -2) {
					auto enemy = Enemy00::Object::Create(true);//�G�̐���
					enemy->pos = ML::Vec2(float(x * chipSize + chipSize / 2), float(y * chipSize + chipSize / 2));
					map[y][x] = -1;//���Z�b�g
				}
			}
		}

		//�t�@�C�������
		fin.close();
		return true;
	}
	//-------------------------------------------------------------------
	//�����蔻��
	bool Object::CheckHit(ML::Box2D hit_)
	{
		ML::Rect r = { hit_.x,
						hit_.y,
						hit_.x + hit_.w,
						hit_.y + hit_.h };
		//��`���}�b�v�O�ɏo�Ă�����T�C�Y��ύX����
		ML::Rect m = {
			hitBase.x,
			hitBase.y,
			hitBase.x + hitBase.w,
			hitBase.x + hitBase.h
		};
		r.left = max(r.left, m.left);			//���ɔ�яo���Ă���
		r.top = max(r.top, m.top);				//��ɔ�яo���Ă���
		r.right = min(r.right, m.right);		//�E�ɔ�яo���Ă���
		r.bottom = min(r.bottom, m.bottom);		//���ɔ�яo���Ă���

		//���[�v�͈͒���
		int sx, sy, ex, ey;
		sx = r.left / chipSize;
		sy = r.top / chipSize;
		ex = (r.right - 1) / chipSize;
		ey = (r.bottom - 1) / chipSize;

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
			hitBase.x + hitBase.h
		};
		//�J�����̈ʒu�𒲐�
		if (c.right > m.right) {
			ge->camera2D.x = m.right - ge->camera2D.w;
		}

		if (c.bottom > m.bottom) {
			ge->camera2D.y = m.bottom - ge->camera2D.h;
		}
		if (c.left < m.left) {
			ge->camera2D.x = m.left;
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
	Object::Object() {	}
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
	Resource::~Resource() { this->Finalize(); }
}
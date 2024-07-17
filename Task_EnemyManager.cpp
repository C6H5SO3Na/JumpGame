//-------------------------------------------------------------------
//�G�}�l�[�W���[
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_EnemyManager.h"
#include "Task_Enemy00.h"
#include <assert.h>

namespace EnemyManager
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
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
		LoadEnemy("./data/enemy/enemyStage" + to_string(ge->stage) + ".csv");

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
	}
	//-------------------------------------------------------------------
//�G�̔z�u�̃��[�h
	bool Object::LoadEnemy(const string& fpath_)
	{
		ifstream fin(fpath_);
		if (!fin) {
			assert(!"�ǂݍ��ݎ��s");
			return false;
		}

		int n;//�G�̐�
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

		//�t�@�C�������
		fin.close();
		return true;
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
	Resource::~Resource() { Finalize(); }
}
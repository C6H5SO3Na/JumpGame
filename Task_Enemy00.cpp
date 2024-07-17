//-------------------------------------------------------------------
//�G00
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Enemy00.h"
#include "Task_Player.h"
#include "Task_Map2D.h"
#include "sound.h"
#include <assert.h>

namespace Enemy00
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		img[0] = DG::Image::Create("data/image/enemy1.png");
		img[1] = DG::Image::Create("data/image/enemy2.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		for (DG::Image::SP i : img) {
			i.reset();
		}
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
		render2D_Priority[1] = 0.5f;
		render2D_Priority[1] = 0.5f;
		angle = Angle_LR::Left;
		hitBase = CenterBox(64, 64);
		moveVec = ML::Vec2(5.f, 5.f);
		src = ML::Box2D(0, 0, 64, 64);
		score = 100;
		jumpPow = -10.f;
		attackPower = 1;

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
		Think();
		Move();
		if (ge->qa_Player != nullptr) {
			if (!ge->qa_Player->CheckHitEnemyHead()) {
				CheckHit(ge->qa_Player);
			}
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		Animation();
		Draw();
	}
	//-------------------------------------------------------------------
	//�v�l
	void Object::Think()
	{
		//�����
	}
	//-------------------------------------------------------------------
	//�s��
	void Object::Move()
	{
		if (!CheckHitCamera2D()) { return; }//�J�����͈̔͊O�̂Ƃ��A���삵�Ȃ�
		ML::Vec2 est;
		//���E�ړ�
		est.x = moveVec.x * angle;
		//�W�����v
		if (type == Type::Jumping) {
			if (isHitFloor) {//���n���̂݃W�����v�J�n�ł���
				fallSpeed = jumpPow;
			}
		}
		est.y += fallSpeed;
		CheckMove(est);

		//�����]��
		if (CheckLeftSide()) {
			angle = Angle_LR::Right;
		}
		if (CheckRightSide()) {
			angle = Angle_LR::Left;
		}

		//�����ڐG����
		isHitFloor = CheckFoot();
		if (isHitFloor) {
			fallSpeed = 0.f;//�������x0
		}
		else {
			fallSpeed += ML::Gravity(32) * 6.f;//�d�͉���
		}

		//���㔻��
		if (fallSpeed < 0) {//�㏸��
			if (CheckHead()) {
				fallSpeed = 0;//�㏸�͂𖳌��ɂ���
			}
		}

		//���ɗ���������ł�����
		if (CheckFallHole()) {
			Dead();
		}
		++moveCnt;
		++animCnt;
	}
	//-------------------------------------------------------------------
	//�A�j���[�V����
	void Object::Animation()
	{
		int animAngleTmp = 64;//draw�ɂ�����h���W ���̏ꍇ32
		if (angle == Angle_LR::Right) {
			animAngleTmp = 128;
		}

		int frameInterval = 0;//�A�j���[�V�����̊Ԋu�t���[��
		switch (animKind) {
		case Anim::Move:
			frameInterval = 8;
			drawBase = CenterBox(64, 64);
			src = ML::Box2D((animCnt / frameInterval) % 3 * drawBase.w, animAngleTmp, drawBase.w, drawBase.h);
			break;
		}
	}
	//-------------------------------------------------------------------
	//�X�v���C�g�`��
	void Object::Draw()
	{
		ML::Box2D draw = drawBase.OffsetCopy(pos);
		draw = ge->ApplyCamera2D(draw);
		res->img[static_cast<int>(type)]->Draw(draw, src);
	}

	//-------------------------------------------------------------------
	//�󂯐g�̏���
	void Object::Recieved(const int& power)
	{
		Dead();
		ge->score += GetScore();
		se::Play("Explosion");
		ge->CreateEffect(8, pos);//�����G�t�F�N�g
	}
	//-------------------------------------------------------------------
	//�J�����Ƃ̓����蔻��
	bool Object::CheckHitCamera2D() const
	{
		//�v���C���Ɠ����蔻��
		ML::Box2D  me = hitBase.OffsetCopy(pos);
		int n = 400;//�J������`���w�肵�������[�ɍL����
		ML::Box2D  you(
			ge->camera2D.x - n,
			ge->camera2D.y,
			ge->camera2D.w + n * 2,
			ge->camera2D.h
		);
		if (you.Hit(me)) {
			return true;
		}
		return false;
	}
	//-------------------------------------------------------------------
	//�󂯐g�̏���
	void Object::Dead()
	{
		Kill();
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
	//-------------------------------------------------------------------
	//�^�X�N����&�p�����[�^�[�w��
	void Object::Spawn(const ML::Vec2& pos, const int& kind)
	{
		auto enemy = Enemy00::Object::Create(true);
		enemy->pos = pos;
		enemy->type = static_cast<Type>(kind);
	}
}
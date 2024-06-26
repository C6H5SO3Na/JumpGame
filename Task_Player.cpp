//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
//#define DEBUG
#include "MyPG.h"
#include "Task_Player.h"
#include "Task_Enemy00.h"
#include "Task_Map2D.h"
#include "Task_Effect00.h"
#include "Task_GoalFlag.h"
#include <assert.h>
#include "sound.h"
#include "randomLib.h"

namespace Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		img = DG::Image::Create("./data/image/player.png");
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
		render2D_Priority[1] = 0.5f;
		angle = Angle_LR::Right;
		hitBase = CenterBox(56, 128);
		maxSpeed = 8.f;
		jumpPow = -17.f;
		attackPower = 1;

		//SE
		se::LoadFile("Jump", "./data/sound/SE/Jump.wav");
		se::LoadFile("Damage", "./data/sound/SE/Damage.wav");
		se::LoadFile("Explosion", "./data/sound/SE/Explosion.wav");
		se::LoadFile("Dead", "./data/sound/SE/Dead.wav");
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
		input = ge->in1->GetState();
		Operation();

		//���ɗ���������ł�����
		if (CheckFallHole()) {
			Dead();
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (invincible.isFlash() && moveCnt % 2 == 0) { return; }
		Animation();
		ML::Box2D draw = drawBase.OffsetCopy(pos);
		draw = ge->ApplyCamera2D(draw);
		res->img->Draw(draw, src);
		//�f�o�b�O�p��`
#if defined(DEBUG)
		ML::Box2D  me = hitBase.OffsetCopy(pos);
		me = ge->ApplyCamera2D(me);
		ge->debugRect(me);
#endif
	}

	//-------------------------------------------------------------------
	//�v���C���̑��쏈��
	void Object::Operation()
	{
		Think();
		Move();
		AdjustCameraPlayer();//�J�����̈ʒu���Ē���
		++moveCnt;
		++animCnt;
		invincible.operation();
	}
	//-------------------------------------------------------------------
	//�J�����̈ʒu���Ē���
	void Object::AdjustCameraPlayer() const
	{
		//�v���C������ʂ̂ǂ��ɒu�����i����͉�ʒ����j
		int px = ge->camera2D.w / 2;
		int py = ge->camera2D.h / 2;
		//�v���C������ʒ����ɒu�����Ƃ��̃J�����̍�����W�����߂�
		int cpx = static_cast<int>(pos.x) - px;
		int cpy = static_cast<int>(pos.y) - py;
		//�J�����̍��W���X�V
		ge->camera2D.x = cpx;
		ge->camera2D.y = cpy;
		//�}�b�v�̊O�����f��Ȃ��悤�ɃJ�����𒲐�����
		Map2D::Object::SP map = ge->qa_Map;
		if (map != nullptr) {
			map->AdjustCameraPos();
		}
	}
	//-------------------------------------------------------------------
	//�v�l
	void Object::Think()
	{
		state->think();
	}
	//-------------------------------------------------------------------
	//�s��
	void Object::Move()
	{
		if (moveVec.y < 0 || !CheckFoot()) {
			moveVec.y += ML::Gravity(32) * 6.f;//�d�͉���
		}
		else {
			moveVec.y = 0;
		}

		if (moveVec.x < 0) {
			moveVec.x = min(moveVec.x + 1.f, 0);
		}
		else {
			moveVec.x = max(moveVec.x - 1.f, 0);
		}
		state->move();

		//�ړ�����
		ML::Vec2 est = moveVec;
		CheckMove(est);
	}
	//�|�����[�t�B�Y��
	//�~�܂��Ă�����
	//-------------------------------------------------------------------
	//�v�l
	void Object::IdleState::think()
	{
		//�����ڐG����
		owner_->isHitFloor = owner_->CheckFoot();
		//�W�����v
		if (owner_->input.B1.down) {
			if (owner_->isHitFloor) {//���n���̂݃W�����v�J�n�ł���
				owner_->ChangeState(new JumpState(owner_));
				return;
			}
		}
		//���E�ړ�
		if (owner_->input.LStick.BL.on ||
			owner_->input.LStick.BR.on) {
			owner_->ChangeState(new WalkState(owner_));
			return;
		}

		//�S�[�����̔���
		if (owner_->CheckHit(ge->GetTask<GoalFlag::Object>("�I�u�W�F�N�g", "�S�[����"))) {
			owner_->ChangeState(new ClearState(owner_));
			return;
		}

		//���ꂪ�Ȃ������痎����Ԃ�
		if (!owner_->isHitFloor) {
			owner_->ChangeState(new FallState(owner_));
			return;
		}
	}
	//-------------------------------------------------------------------
	//�s��
	void Object::IdleState::move()
	{
	}
	//-------------------------------------------------------------------
	//�A�j���[�V����
	void Object::IdleState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->src = ML::Box2D(
			(owner_->animCnt / frameInterval) % 4 * owner_->drawBase.w,
			0,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//�|�����[�t�B�Y��
	//���s���
	//-------------------------------------------------------------------
	//�v�l
	void Object::WalkState::think()
	{
		//�����ڐG����
		owner_->isHitFloor = owner_->CheckFoot();

		//���E���͂��Ȃ��Ƃ���Idle�X�e�[�g��
		if (owner_->input.LStick.BL.off &&
			owner_->input.LStick.BR.off) {
			owner_->ChangeState(new IdleState(owner_));
			return;
		}

		//�W�����v
		if (owner_->input.B1.down) {
			if (owner_->isHitFloor) {//���n���̂݃W�����v�J�n�ł���
				owner_->ChangeState(new JumpState(owner_));
				return;
			}
		}

		//�S�[�����̔���
		if (owner_->CheckHit(ge->GetTask<GoalFlag::Object>("�I�u�W�F�N�g", "�S�[����"))) {
			owner_->ChangeState(new ClearState(owner_));
			return;
		}

		//������Ԃ�
		if (!owner_->isHitFloor) {
			owner_->ChangeState(new FallState(owner_));
			return;
		}
	}
	//-------------------------------------------------------------------
	//�s��
	void Object::WalkState::move()
	{
		//���E�ړ�
		if (owner_->input.LStick.BL.on) {
			owner_->moveVec.x = -owner_->maxSpeed;
			owner_->angle = Angle_LR::Left;
		}

		if (owner_->input.LStick.BR.on) {
			owner_->moveVec.x = +owner_->maxSpeed;
			owner_->angle = Angle_LR::Right;
		}
	}
	//-------------------------------------------------------------------
	//�A�j���[�V����
	void Object::WalkState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->src = ML::Box2D(
			(owner_->animCnt / frameInterval) % 7 * owner_->drawBase.w,
			owner_->drawBase.h,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//�|�����[�t�B�Y��
	//�W�����v���
	//-------------------------------------------------------------------
	//�v�l
	void Object::JumpState::think()
	{
		//�S�[������
		if (owner_->CheckHit(ge->GetTask<GoalFlag::Object>("�I�u�W�F�N�g", "�S�[����"))) {
			owner_->ChangeState(new ClearState(owner_));
			return;
		}

		//�����ڐG����
		owner_->isHitFloor = owner_->CheckFoot();
		if (owner_->moveCnt > 0.f && owner_->isHitFloor) {
			owner_->ChangeState(new IdleState(owner_));
			return;
		}

		//������Ԃ�
		if (owner_->moveVec.y >= 0.f) {
			owner_->ChangeState(new FallState(owner_));
			return;
		}
	}
	//-------------------------------------------------------------------
	//�s��
	void Object::JumpState::move()
	{
		//�����ݒ�&SE
		if (owner_->moveCnt == 0) {
			owner_->moveVec.y = owner_->jumpPow;
			se::Play("Jump");
		}
		//���E�ړ�
		if (owner_->input.LStick.BL.on) {
			owner_->moveVec.x = -owner_->maxSpeed;
			owner_->angle = Angle_LR::Left;
		}

		if (owner_->input.LStick.BR.on) {
			owner_->moveVec.x = +owner_->maxSpeed;
			owner_->angle = Angle_LR::Right;
		}

		//�W�����v����
		if (owner_->input.B1.up && owner_->moveVec.y < 0) {//�W�����v�{�^�������ꂽ�u�Ԃ��㏸���̏ꍇ
			owner_->moveVec.y = 0.f;//�������x0(���W�����v)
		}

		//����ڐG����
		if (owner_->CheckHead()) {
			owner_->moveVec.y = 0.f;//�W�����v����
		}
	}

	//-------------------------------------------------------------------
	//�A�j���[�V����
	void Object::JumpState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->src = ML::Box2D(
			(owner_->animCnt / frameInterval) % 6 * owner_->drawBase.w,
			owner_->drawBase.h * 2,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//�|�����[�t�B�Y��
	//�������
	//-------------------------------------------------------------------
	//�v�l
	void Object::FallState::think()
	{
		//�S�[������
		if (owner_->CheckHit(ge->GetTask<GoalFlag::Object>("�I�u�W�F�N�g", "�S�[����"))) {
			owner_->ChangeState(new ClearState(owner_));
			return;
		}

		//�����ڐG����
		owner_->isHitFloor = owner_->CheckFoot();
		if (owner_->isHitFloor) {
			owner_->ChangeState(new IdleState(owner_));
			return;
		}
	}
	//-------------------------------------------------------------------
	//�s��
	void Object::FallState::move()
	{
		//���E�ړ�
		if (owner_->input.LStick.BL.on) {
			owner_->moveVec.x = -owner_->maxSpeed;
			owner_->angle = Angle_LR::Left;
		}

		if (owner_->input.LStick.BR.on) {
			owner_->moveVec.x = +owner_->maxSpeed;
			owner_->angle = Angle_LR::Right;
		}
	}

	//-------------------------------------------------------------------
	//�A�j���[�V����
	void Object::FallState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->src = ML::Box2D(
			(owner_->animCnt / frameInterval) % 3 * owner_->drawBase.w + owner_->drawBase.w * 4,
			0,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//�|�����[�t�B�Y��
	//��e���
	//-------------------------------------------------------------------
	//�v�l
	void Object::HurtState::think()
	{
		if (owner_->animCnt > 24) {
			owner_->invincible.startFlash();
			owner_->ChangeState(new IdleState(owner_));
		}
	}
	//-------------------------------------------------------------------
	//�s��
	void Object::HurtState::move()
	{
		//�����
	}
	//-------------------------------------------------------------------
	//�A�j���[�V����
	void Object::HurtState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->src = ML::Box2D((owner_->animCnt / frameInterval) % 4 * owner_->drawBase.w,
			owner_->drawBase.h * 3,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//�|�����[�t�B�Y��
	//�X�e�[�W�N���A���
	//-------------------------------------------------------------------
	//�v�l
	void Object::ClearState::think()
	{
		//�����
	}
	//-------------------------------------------------------------------
	//�s��
	void Object::ClearState::move()
	{
		ge->isClear = true;
	}
	//-------------------------------------------------------------------
	//�A�j���[�V����
	void Object::ClearState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->animCnt = min(owner_->animCnt, 5 * frameInterval - 1);
		owner_->src = ML::Box2D(
			(owner_->animCnt / frameInterval) % 5 * owner_->drawBase.w,
			owner_->drawBase.h * 5,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//�|�����[�t�B�Y��
	//���S���
	//-------------------------------------------------------------------
	//�v�l
	void Object::DeadState::think()
	{
		//�����
	}
	//-------------------------------------------------------------------
	//�s��
	void Object::DeadState::move()
	{
		//���S���ɂ͏�ɂ����Ȃ��悤�ɂ���
		owner_->moveVec.y = max(0.f, owner_->moveVec.y);
	}

	//-------------------------------------------------------------------
	//�A�j���[�V����
	void Object::DeadState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->animCnt = min(owner_->animCnt, 5 * frameInterval - 1);
		owner_->src = ML::Box2D(
			(owner_->animCnt / frameInterval) % 5 * owner_->drawBase.w,
			owner_->drawBase.h * 4,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//-------------------------------------------------------------------
	//�A�j���[�V����
	void Object::Animation()
	{
		state->anim();
		//���������]
		if (angle == Angle_LR::Left &&
			drawBase.w >= 0) {
			drawBase.x = -drawBase.x;
			drawBase.w = -drawBase.w;
		}
	}
	//-------------------------------------------------------------------
	//�G�̓��Ƃ̓����蔻��
	bool Object::CheckHitEnemyHead()
	{
		bool rtv = false;
		//�����蔻�����ɂ��đ�����`�𐶐�
		ML::Box2D playerFoot(
			hitBase.x,
			hitBase.y + hitBase.h,
			hitBase.w,
			10);
		//�G�̓���Ɠ����蔻��
		ML::Box2D  me = playerFoot.OffsetCopy(pos);
		//�f�o�b�O�p��`
#if defined(DEBUG)
		ML::Box2D debugMe = ge->ApplyCamera2D(me);
		ge->debugRect(debugMe, ge->DEBUGRECTMODE::GREEN);
#endif
		auto enemies = ge->qa_Enemies;
		for_each(enemies->begin(), enemies->end(), [&](auto iter) {
			//�����蔻�����ɂ��ē����`�𐶐�
			ML::Box2D enemyHead(
				iter->GetHitBase().x,
				iter->GetHitBase().y - 1,
				iter->GetHitBase().w,
				10);
			ML::Box2D  you = enemyHead.OffsetCopy(iter->GetPos());
			//�f�o�b�O�p��`
#if defined(DEBUG)
			ML::Box2D debugYou = ge->ApplyCamera2D(you);
			ge->debugRect(debugYou, ge->DEBUGRECTMODE::RED);
#endif
			if (you.Hit(me)) {
				if (moveVec.y <= 0.f) { return; }//�v���C�����������łȂ���Ζ���
				iter->Recieved(attackPower);
				moveVec.y = jumpPow / 2.f;//�G�𓥂񂾂玩���I�ɃW�����v����
				ChangeState(new JumpState(this));
				rtv = true;
				return;
	}

});
		return rtv;
	}
	//-------------------------------------------------------------------
	//��ԕύX
	void Object::ChangeState(StateBase* const state_)
	{
		delete this->state;
		this->state = state_;
		moveCnt = 0;
		animCnt = 0;
	}
	//-------------------------------------------------------------------
	//�_���[�W���󂯂����̏���
	void Object::DamageOperation()
	{
		ChangeState(new HurtState(this));
		moveCnt = 0;
		animCnt = 0;
		invincible.start();
		se::Play("Damage");
	}
	//-------------------------------------------------------------------
	//���C�t�̑���
	void Object::LifeOperation(const int& addLife)
	{
		life.addNow(addLife);
		if (life.getNow() <= 0) {
			Dead();
		}
	}
	//-------------------------------------------------------------------
	//�󂯐g�̏���
	void Object::Recieved(const int& power)
	{
		if (invincible.isInvincible()) { return; }
		LifeOperation(-power);
		if (life.getNow() >= 0) {//����ł��Ȃ��Ƃ��̓_���[�W���󂯂鉉�o������
			DamageOperation();
		}
	}
	//-------------------------------------------------------------------
	//���S����
	void Object::Dead()
	{
		Kill();
		ge->isDead = true;
		bgm::Stop("Main");
		se::Play("Dead");
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
	Object::Object() :
		state(new IdleState(this)),
		jumpPow(),
		invincible(),
		life(5, 5) {	}
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
	//-------------------------------------------------------------------
	//�^�X�N����&�p�����[�^�[�w��
	void  Object::Spawn(const ML::Vec2& pos)
	{
		auto enemy = Create(true);
		enemy->pos = pos;
	}
}
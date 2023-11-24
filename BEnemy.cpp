//-------------------------------------------------------------------
//�G�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include  "BEnemy.h"
#include  "MyPG.h"
#include  "Task_Map2D.h"

//-------------------------------------------------------------------
//�J�����Ƃ̓����蔻��
bool BEnemy::CheckHitCamera2D()
{
	if (state == State::Non) { return false; }
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
//�v���C���Ƃ̓����蔻��
void BEnemy::CheckHitPlayer()
{
	if (state == State::Non) { return; }
	//�v���C���Ɠ����蔻��
	ML::Box2D  me = hitBase.OffsetCopy(pos);
	auto player = ge->qa_Player;

	if (player->state != State::Normal || player->isInvincible) { return; }

	ML::Box2D  you = player->hitBase.OffsetCopy(player->pos);
	if (you.Hit(me)) {
		player->LifeOperation(-1);
		if (player->state != State::Dead) {//����ł��Ȃ��Ƃ��̓_���[�W���󂯂鉉�o������
			player->DamageOperation();
		}
	}
}
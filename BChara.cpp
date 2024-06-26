//-------------------------------------------------------------------
//�L�����N�^�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include  "BChara.h"
#include  "MyPG.h"
#include  "Task_Map2D.h"

bool BChara::CheckFoot() const
{
	//�����蔻�����ɂ��đ�����`�𐶐�
	ML::Box2D foot(
		hitBase.x,
		hitBase.y + hitBase.h,
		hitBase.w,
		1);
	foot.Offset(pos);

	Map2D::Object::SP map = ge->qa_Map;
	if (map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)
	//�}�b�v�ƐڐG����
	return map->CheckHit(foot);
}

//-------------------------------------------------------------------
//����̓����蔻��
bool BChara::CheckHead() const
{
	Map2D::Object::SP map = ge->qa_Map;
	if (map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)
	//�����蔻�����ɂ��ē����`�𐶐�
	ML::Box2D head(
		hitBase.x,
		hitBase.y - 1,
		hitBase.w,
		1);
	head.Offset(pos);

	//�}�b�v�ƐڐG����
	return  map->CheckHit(head);
}
//-------------------------------------------------------------------
//�����̓����蔻��
bool  BChara::CheckLeftSide() const
{

	Map2D::Object::SP map = ge->qa_Map;
	if (map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)
	//�����蔻�����ɂ��č�1�}�X�̋�`�𐶐�
	ML::Box2D leftSide(
		hitBase.x - 1,
		hitBase.y,
		1,
		hitBase.h);
	leftSide.Offset(pos);

	//�}�b�v�ƐڐG����
	return  map->CheckHit(leftSide);
}
//-------------------------------------------------------------------
//�E���̓����蔻��
bool  BChara::CheckRightSide() const
{
	Map2D::Object::SP map = ge->qa_Map;
	if (map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)

	//�����蔻�����ɂ��ĉE1�}�X�̋�`�𐶐�
	ML::Box2D rightSide(
		hitBase.w + hitBase.x,
		hitBase.y,
		1,
		hitBase.h);
	rightSide.Offset(pos);

	//�}�b�v�ƐڐG����
	return  map->CheckHit(rightSide);
}
//-------------------------------------------------------------------
//���ɗ��������̔���
bool BChara::CheckFallHole() const
{
	if (ge->qa_Map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)
	return pos.y > ge->screen2DHeight - hitBase.y;//�L�����N�^�㕔�̍��W����ʉ��̍��W�𒴂����Ƃ�true��Ԃ�
}
//-------------------------------------------------------------------
//��`�̍��W�̒��S�𒆉��ɂ��Ē�`����
ML::Box2D BChara::CenterBox(const int& w, const int& h)
{
	return ML::Box2D(-w / 2, -h / 2, w, h);
}
//-------------------------------------------------------------------
//�߂荞�܂Ȃ��ړ�����
void BChara::CheckMove(ML::Vec2& e_)
{
	shared_ptr<Map2D::Object> map = ge->qa_Map;
	//�}�b�v�����݂��邩���ׂĂ���A�N�Z�X
	if (map == nullptr) { return; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)

	//�����ɑ΂���ړ�
	while (e_.x != 0.f) {
		float preX = pos.x;
		if (e_.x >= +1.f) {
			pos.x += 1.f;
			e_.x -= 1.f;
		}
		else if (e_.x <= -1.f) {
			pos.x -= 1.f;
			e_.x += 1.f;
		}
		else {
			pos.x += e_.x;
			e_.x = 0.f;
		}
		ML::Box2D hit = hitBase.OffsetCopy(pos);
		if (map->CheckHit(hit)) {
			pos.x = preX;//�ړ��L�����Z��
			break;
		}
	}

	//�c���ɑ΂���ړ�
	while (e_.y != 0.f) {
		float preY = pos.y;
		if (e_.y >= +1.f) {
			pos.y += 1.f;
			e_.y -= 1.f;
		}
		else if (e_.y <= -1.f) {
			pos.y -= 1.f;
			e_.y += 1.f;
		}
		else {
			pos.y += e_.y;
			e_.y = 0.f;
		}
		ML::Box2D hit = hitBase.OffsetCopy(pos);
		if (map->CheckHit(hit)) {
			pos.y = preY;//�ړ��L�����Z��
			break;
		}
	}

}
//-------------------------------------------------------------------
//�����蔻��(�P��)
bool  BChara::CheckHit(BChara::SP ptr)
{
	if (ptr == nullptr) { return false; }
	ML::Box2D me = this->GetHitBase().OffsetCopy(this->GetPos());
	ML::Box2D you = ptr->GetHitBase().OffsetCopy(ptr->GetPos());
#if defined(DEBUG)
	ML::Box2D debugYou = ge->ApplyCamera2D(you);
	ge->debugRect(debugYou, ge->DEBUGRECTMODE::RED);
#endif
	if (you.Hit(me)) {
		ptr->Recieved(this->attackPower);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------
//�����蔻��(����)
bool  BChara::CheckHit(shared_ptr<vector<BChara::SP>> iters)
{
	bool rtv = false;
	for_each(iters->begin(), iters->end(),
		[&](auto iter) {
			ML::Box2D me = this->GetHitBase().OffsetCopy(this->GetPos());
			ML::Box2D you = iter->GetHitBase().OffsetCopy(iter->GetPos());
#if defined(DEBUG)
			ML::Box2D debugYou = ge->ApplyCamera2D(you);
			ge->debugRect(debugYou, ge->DEBUGRECTMODE::RED);
#endif
			if (you.Hit(me)) {
				iter->Recieved(this->attackPower);
				rtv = true;
				return;
			}
		});
	return rtv;
}
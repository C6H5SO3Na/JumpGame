#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�L�����N�^�i�������́j�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

class BChara : public BTask
{
	//�ύX�s����������������������������������������������������
public:
	typedef  shared_ptr<BChara>	SP;
	typedef  weak_ptr<BChara>	WP;

	//�L�����̏��
	enum class State {
		Normal, Hit, Clear, Dead, Non
	};

	//���E�̌���(2D�����_�Q�[����p)
	enum Angle_LR {
		Right = 1, Left = -1
	};
protected:
	ML::Box2D src;//�摜�̐؂���͈�
	ML::Box2D drawBase;//�`��͈�

	bool isHitFloor;//���Ɠ����������ۂ�
	float fallSpeed;//�������x

	ML::Vec2 pos;//�v���C�����W
	ML::Box2D hitBase;//�����蔻��͈�

	State state_;
	int	moveCnt;//�s�������p�J�E���^
	int	animCnt;//�A�j���[�V���������p�J�E���^
	int score;//�X�R�A
	int attackPower;
	float maxSpeed;

	ML::Vec2 moveVec;//�ړ����x

		//�߂荞�܂Ȃ��ړ�����
	void CheckMove(ML::Vec2& e_);

	//�����蔻��
	bool  CheckHit(BChara::SP ptr);
	bool  CheckHit(shared_ptr<vector<BChara::SP>> ptr);
	//�����ڐG����
	bool CheckFoot() const;
	//����ڐG����
	bool CheckHead() const;
	//������
	bool CheckLeftSide() const;
	//�E����
	bool CheckRightSide() const;
	//���ɗ��������̔���
	bool CheckFallHole() const;
	//��`�̍��W�̒��S�𒆉��ɂ��Ē�`����
	ML::Box2D CenterBox(const int& w, const int& h);
	//Box2D�^�̊e�v�f�Ɉ��̒l���|����(�g��p)
	ML::Box2D MultiplyBox2D(const ML::Box2D& box2D, const float& n);
	//�X�R�A�擾
	int GetScore()const { return score; }

	virtual void Dead() = 0;
public:
	//�ύX������������������������������������������������������
	//�L�����̏��
	Angle_LR angle;

	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����邱�Ɓ���
	BChara()//�R���X�g���N�^
		:pos()
		, moveCnt(0)
		, animCnt(0)
		, fallSpeed(0.f)
		, isHitFloor(false)
		, state_(State::Non)
		, score(0)
		, attackPower(0)
		, maxSpeed(0.f)
		, angle(Angle_LR::Right)
	{
	}
	virtual ~BChara() {}//�f�X�g���N�^

	//�L�����N�^���ʃ��\�b�h

	virtual void Recieved(const int& power) = 0;

	//���W�擾
	ML::Vec2 GetPos() const { return pos; }
	//�����蔻��擾
	ML::Box2D GetHitBase() const { return hitBase; }
	//��Ԏ擾
	State GetState() const { return state_; }
};
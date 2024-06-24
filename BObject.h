#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�L�����N�^�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

class BObject : public BTask
{
	//�ύX�s����������������������������������������������������
public:
	typedef  shared_ptr<BObject>	SP;
	typedef  weak_ptr<BObject>	WP;

	//�ύX������������������������������������������������������
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

	State state;
	int	moveCnt;//�s�������p�J�E���^
	int	animCnt;//�A�j���[�V���������p�J�E���^
	int score;//�X�R�A

public:
	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����邱�Ɓ���
	BObject()//�R���X�g���N�^
		:pos()
		, moveCnt()
		, animCnt()
		, fallSpeed()
		, isHitFloor()
		, state(State::Non)
		, score()
	{
	}
	virtual ~BObject() {}//�f�X�g���N�^

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
	//���W�Z�b�g
	void SetPos(const ML::Vec2& p) { pos = p; }
	//���W�擾
	ML::Vec2 GetPos() const { return pos; }
	//�����蔻��擾
	ML::Box2D GetHitBase() const { return hitBase; }
	//��Ԏ擾
	State GetState() const { return state; }
	//�X�R�A�擾
	int GetScore()const { return score; }
};
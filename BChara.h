#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�L�����N�^�i�������́j�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include "BObject.h"

class BChara : public BObject
{
	//�ύX�s����������������������������������������������������
public:
	typedef  shared_ptr<BChara>	SP;
	typedef  weak_ptr<BChara>	WP;

protected:
	ML::Vec2 moveVec;//�ړ����x

public:
	//�ύX������������������������������������������������������
	//�L�����̏��
	Angle_LR angle;

	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����邱�Ɓ���
	BChara()//�R���X�g���N�^
		:angle(Angle_LR::Right)
	{
	}
	virtual ~BChara() {}//�f�X�g���N�^

	//�L�����N�^���ʃ��\�b�h
	//�߂荞�܂Ȃ��ړ�����
	void CheckMove(ML::Vec2& e_);
};
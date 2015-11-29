#pragma once
#include "cocos2d.h"
USING_NS_CC;

class GameScene : public LayerColor {
public:
	GameScene();
	~GameScene();

	static Scene *createScene();
	CREATE_FUNC(GameScene);
	virtual bool init() override;
private:
	//������и���
	LayerColor ** m_blocks;
	//��¼�հ׸�����
	int m_spaceBlockCount;
	//��ʼ������
	Vec2 m_startPos;
	//һ�λ����Ƿ������ƶ���ϲ�
	bool m_isMoved;
	//�����Ƿ���Ի���
	bool m_canMove;

	//��ʼ����
	void initBlock(int);
	//���ÿ�ı�ǩֵ
	void createBlock();
	//�ƶ�������
	void moveBlock(Touch *, Event *);
	//�����ƶ�
	void down();
	//�����ƶ�
	void up();
	//�����ƶ�
	void left();
	//�����ƶ�
	void right();
	//�ɹ�
	void uWin();
	//��Ϸ����
	void over();
	//���¿�ʼ
	void reStart(Ref *);
};

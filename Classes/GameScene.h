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
	//存放所有格子
	LayerColor ** m_blocks;
	//记录空白格数量
	int m_spaceBlockCount;
	//起始触摸点
	Vec2 m_startPos;
	//一次滑屏是否发生了移动或合并
	bool m_isMoved;
	//控制是否可以滑屏
	bool m_canMove;

	//初始化块
	void initBlock(int);
	//设置块的标签值
	void createBlock();
	//移动并新增
	void moveBlock(Touch *, Event *);
	//向下移动
	void down();
	//向上移动
	void up();
	//向左移动
	void left();
	//向右移动
	void right();
	//成功
	void uWin();
	//游戏结束
	void over();
	//重新开始
	void reStart(Ref *);
};

#include "GameScene.h"
#include "CommonData.h"

GameScene::GameScene() {
	//最初的空白格有十六个
	this->m_spaceBlockCount = BLOCK_COUNT;

	//计算十六个指针空间
	auto size = sizeof(LayerColor *) * BLOCK_COUNT;
	//分配矩阵指针空间
	this->m_blocks = (LayerColor **)malloc(size);
	//初始化空指针
	memset(this->m_blocks, 0, size);

	this->m_startPos = Vec2::ZERO;
	this->m_isMoved = false;
	this->m_canMove = true;
}

GameScene::~GameScene() {
	//析构中释放矩阵空间
	CC_SAFE_FREE(this->m_blocks);
}

Scene *GameScene::createScene() {
	auto layer = GameScene::create();
	auto scene = Scene::create();
	scene->addChild(layer);
	return scene;
}

bool GameScene::init() {
	do
	{
		CC_BREAK_IF(!LayerColor::initWithColor(Color4B(180, 170, 160, 255)));

		srand(time(nullptr));
		auto visibleSize = Director::getInstance()->getVisibleSize();
		auto origin = Director::getInstance()->getVisibleOrigin();

		//背景
		auto bg = LayerColor::create(Color4B::ORANGE, BACKGROUND_LENGTH, BACKGROUND_LENGTH);
		bg->setPosition(origin + Vec2(visibleSize)/2);
		//layer默认锚点不会影响坐标
		bg->ignoreAnchorPointForPosition(false);
		this->addChild(bg, BACKGROUND_ZORDER, BACKGROUND_TAG);

		//循环添加格子背景
		for (auto i = 0; i < BLOCK_COUNT; i++)
		{
			initBlock(i);
		}
			
		//随机产生两个块
		createBlock();
		createBlock();

		//触摸事件
		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = [&](Touch *touch, Event *) {
			m_startPos = touch->getLocation();
			return m_canMove;
		};
		listener->onTouchEnded = CC_CALLBACK_2(GameScene::moveBlock, this);

		//第二个参数表示：以谁为事件优先级的参照，它的优先级最高
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		//创建遮罩层
		auto propLayer = LayerColor::create(Color4B(100, 100, 100, 230));
		
		auto lblMessage = Label::createWithTTF("", "Marker Felt.ttf", 48);
		lblMessage->setPosition(Vec2(propLayer->getContentSize())/2);
		lblMessage->setColor(Color3B::RED);
		propLayer->addChild(lblMessage, PROP_LAYER_LABEL_MESSAGE_ZORDER, PROP_LAYER_LABEL_MESSAGE_TAG);

		auto itemRestart = MenuItemLabel::create(Label::createWithTTF("RESTART", "Marker Felt.ttf", 36), CC_CALLBACK_1(GameScene::reStart, this));
		itemRestart->setPosition(Vec2(propLayer->getContentSize()) / 2 + Vec2(0, -100));
		itemRestart->setColor(Color3B::BLACK);
		auto menu = Menu::create(itemRestart, nullptr);
		menu->setPosition(Vec2::ZERO);
		propLayer->addChild(menu);

		this->addChild(propLayer, PROP_LAYER_ZORDER, PROP_LAYER_TAG);
		propLayer->setVisible(false);

		return true;
	} while (0);

	return false;
}

void GameScene::initBlock(int index) {
	auto bg = this->getChildByTag(BACKGROUND_TAG);

	auto block = LayerColor::create(Color4B::WHITE, BLOCK_LENGTH, BLOCK_LENGTH);
	//添加标签
	auto lblNumber = Label::createWithTTF("", "Marker Felt.ttf", 36);
	lblNumber->setPosition(Vec2(block->getContentSize()) / 2);
	block->addChild(lblNumber, BLOCK_LABEL_ZORDER, BLOCK_LABEL_TAG);

	//定位
	auto x = SPACE_LENGTH + (BLOCK_LENGTH + SPACE_LENGTH) * (index % ROW_COUNT);
	auto y = SPACE_LENGTH + (BLOCK_LENGTH + SPACE_LENGTH) * (index / ROW_COUNT);
	block->setPosition(x, y);
	bg->addChild(block);

	//每创建一块就保存到矩阵中，方便后续访问这些块
	m_blocks[index] = block;
}

void GameScene::createBlock() {
	//还有空白格
	if (this->m_spaceBlockCount <= 0) return;

	//随机产生位置并添加到矩阵中
	do
	{
		//随机产生位置下标
		auto index = rand() % BLOCK_COUNT;
		//如果矩阵中这个下标位置已经有块了就重新产生下标
		//获取随机位置块的标签
		auto lblNumber = dynamic_cast<Label *>(m_blocks[index]->getChildByTag(BLOCK_LABEL_TAG));
		//如果这个位置的块的标签有内容则重新获取
		if (lblNumber->getString() != "") continue;

		//随机产生数字（2或4）
		auto number = rand() % 10 < 9 ? 2 : 4;
		//更新Label显示
		lblNumber->setString(StringUtils::format("%d", number));
		//根据数字决定颜色
		m_blocks[index]->setColor(Color3B(250-sqrt(number)*4, 250-sqrt(number)*4, 150+sqrt(number)*3));
		//放大动画
		m_blocks[index]->setScale(0.5f);
		m_blocks[index]->runAction(ScaleTo::create(0.1f, 1));
		//正确改变标签后退出循环
		break;
	} while (1);

	//产生一块就少一块
	this->m_spaceBlockCount--;
}

void GameScene::moveBlock(Touch *touch, Event *) {
	auto endPos = touch->getLocation();
	auto vec = endPos - m_startPos;

	//如果向量的两个方向均小于最小响应距离，就不响应
	if (fabs(vec.x) < MIN_STEP && fabs(vec.y) < MIN_STEP) return;
	//分出是水平响应还是垂直方向响应
	auto isH = fabs(vec.x) > fabs(vec.y);
	if (isH)
	{
		if (vec.x > 0)
		{
			right();
		}
		else {
			left();
		}
	}
	else {
		if (vec.y > 0)
		{
			up();
		}
		else {
			down();
		}
	}

	//产生新块
	if (m_isMoved)
	{
		createBlock();
		//重置滑屏移动变量
		m_isMoved = false;
	}
	else if (m_spaceBlockCount <= 0)
	{
		over();
	}
}

void GameScene::down() {
	//log("=====down");
	//对于下来说外循环是列，内循环是行
	for (int col = 0; col < BLOCK_COUNT/ROW_COUNT; col++)
	{
		//记录空白格数量
		int spaceBlockCount = 0;
		//前一个格子的标签值
		std::string preString = "";

		for (int row = 0; row < ROW_COUNT; row++)
		{
			//得到具体的格子
			auto block = m_blocks[row * COL_COUNT + col];
			//得到格子中的标签
			auto lblNumber = dynamic_cast<Label *>(block->getChildByTag(BLOCK_LABEL_TAG));
			//取出自己标签的值
			auto lblString = lblNumber->getString();
			//标签值为空，则累计数量
			if (lblString == "")
			{
				spaceBlockCount++;
			}
			else {
				//计算格子的新位置
				auto newRow = row - spaceBlockCount;
				auto newBlock = m_blocks[newRow * COL_COUNT + col];
				auto newLabel = dynamic_cast<Label *>(newBlock->getChildByTag(BLOCK_LABEL_TAG));

				if (spaceBlockCount > 0)	//标签有值且有空白格，就要【移位】
				{
					auto other = m_blocks[newRow * COL_COUNT + col];
					auto otherLabel = dynamic_cast<Label *>(other->getChildByTag(BLOCK_LABEL_TAG));
					//更新双方的标签值
					otherLabel->setString(lblString);
					lblNumber->setString("");
					//更新双方的颜色
					other->setColor(block->getColor());
					block->setColor(Color3B::WHITE);

					//移动了
					m_isMoved = true;
				}
				//合并
				if (preString != "" && preString == newLabel->getString())
				{
					//获取前一块
					auto preRow = newRow - 1;
					auto preBlock = m_blocks[preRow * COL_COUNT + col];
					auto preLabel = dynamic_cast<Label *>(preBlock->getChildByTag(BLOCK_LABEL_TAG));
					auto number = atoi(preString.c_str()) * 2;
					//前一块值double，当前块清空
					preLabel->setString(StringUtils::format("%d", number));
					newLabel->setString("");
					//重新改变颜色
					preBlock->setColor(Color3B(250 - sqrt(number) * 4, 250 - sqrt(number) * 4, 150 + sqrt(number) * 3));
					newBlock->setColor(Color3B::WHITE);
					//产生合并，移位空白格多一个，所有的空白格也多一个
					spaceBlockCount++;
					m_spaceBlockCount++;

					//前一个块的值已经被使用，清空
					preString = "";

					//合并发生
					m_isMoved = true;

					if (number >= 2048)
					{
						uWin();
					}
				}
				else {
					//用当前块的值替换，因为只有相邻且值相等的非空白块才可以合并
					preString = newLabel->getString();
				}
			}
		}
	}
}

void GameScene::up() {
	//log("=====up");
	for (int col = 0; col < COL_COUNT; col++)
	{
		int spaceBlockCount = 0;
		std::string preString = "";
		for (int row = ROW_COUNT - 1; row >= 0; row--)
		{
			//得到具体的格子
			auto block = m_blocks[row * COL_COUNT + col];
			//得到格子中的标签
			auto lblNumber = dynamic_cast<Label *>(block->getChildByTag(BLOCK_LABEL_TAG));
			//标签值为空，则累计数量
			if (lblNumber->getString() == "")
			{
				spaceBlockCount++;
			}
			else {
				//得到新位置的格子
				auto newRow = row + spaceBlockCount;
				auto newBlock = m_blocks[newRow * COL_COUNT + col];
				auto newLabel = dynamic_cast<Label *>(newBlock->getChildByTag(BLOCK_LABEL_TAG));

				if (spaceBlockCount > 0)	//标签有值且有空白格
				{
					//更新双方的标签值
					newLabel->setString(lblNumber->getString());
					lblNumber->setString("");
					//更新双方的颜色
					newBlock->setColor(block->getColor());
					block->setColor(Color3B::WHITE);

					m_isMoved = true;
				}

				if (preString != "" && preString == newLabel->getString())
				{
					auto preRow = newRow + 1;
					auto preBlock = m_blocks[preRow * COL_COUNT + col];
					auto preLabel = dynamic_cast<Label *>(preBlock->getChildByTag(BLOCK_LABEL_TAG));

					auto number = atoi(preLabel->getString().c_str()) * 2;
					preLabel->setString(StringUtils::format("%d", number));
					newLabel->setString("");

					preBlock->setColor(Color3B(250 - sqrt(number) * 4, 250 - sqrt(number) * 4, 150 + sqrt(number) * 3));
					newBlock->setColor(Color3B::WHITE);

					spaceBlockCount++;
					m_spaceBlockCount++;

					preString = "";
					m_isMoved = true;
					if (number >= 2048)
					{
						uWin();
					}
				}
				else {
					preString = newLabel->getString();
				}
			}
		}
	}
}

void GameScene::left() {
	//log("=====left");
	for (int row = 0; row < ROW_COUNT; row++)
	{
		int spaceBlockCount = 0;
		std::string preString = "";
		for (int col = 0; col < COL_COUNT; col++)
		{
			//得到具体的格子
			auto block = m_blocks[row * COL_COUNT + col];
			//得到格子中的标签
			auto lblNumber = dynamic_cast<Label *>(block->getChildByTag(BLOCK_LABEL_TAG));
			//标签值为空，则累计数量
			if (lblNumber->getString() == "")
			{
				spaceBlockCount++;
			}
			else {
				auto newCol = col - spaceBlockCount;
				auto newBlock = m_blocks[row * COL_COUNT + newCol];
				auto newLabel = dynamic_cast<Label *>(newBlock->getChildByTag(BLOCK_LABEL_TAG));
				if (spaceBlockCount > 0)	//标签有值且有空白格
				{
					//更新双方的标签值
					newLabel->setString(lblNumber->getString());
					lblNumber->setString("");
					//更新双方的颜色
					newBlock->setColor(block->getColor());
					block->setColor(Color3B::WHITE);

					m_isMoved = true;
				}
				if (preString != "" && preString == newLabel->getString())
				{
					auto preCol = newCol - 1;
					auto preBlock = m_blocks[row * COL_COUNT + preCol];
					auto preLabel = dynamic_cast<Label *>(preBlock->getChildByTag(BLOCK_LABEL_TAG));

					auto number = atoi(preLabel->getString().c_str()) * 2;
					preLabel->setString(StringUtils::format("%d", number));
					newLabel->setString("");

					preBlock->setColor(Color3B(250 - sqrt(number) * 4, 250 - sqrt(number) * 4, 150 + sqrt(number) * 3));
					newBlock->setColor(Color3B::WHITE);

					spaceBlockCount++;
					m_spaceBlockCount++;

					preString = "";
					m_isMoved = true;
					if (number >= 2048)
					{
						uWin();
					}
				}
				else {
					preString = newLabel->getString();
				}
			}
		}
	}
}

void GameScene::right() {
	//log("=====right");
	for (int row = 0; row < ROW_COUNT; row++)
	{
		int spaceBlockCount = 0;
		std::string preString = "";
		for (int col = COL_COUNT - 1; col >= 0; col--)
		{
			//得到具体的格子
			auto block = m_blocks[row * COL_COUNT + col];
			//得到格子中的标签
			auto lblNumber = dynamic_cast<Label *>(block->getChildByTag(BLOCK_LABEL_TAG));
			//标签值为空，则累计数量
			if (lblNumber->getString() == "")
			{
				spaceBlockCount++;
			}
			else {
				auto newCol = col + spaceBlockCount;
				auto newBlock = m_blocks[row * COL_COUNT + newCol];
				auto newLabel = dynamic_cast<Label *>(newBlock->getChildByTag(BLOCK_LABEL_TAG));
				if (spaceBlockCount > 0)	//标签有值且有空白格
				{
					//更新双方的标签值
					newLabel->setString(lblNumber->getString());
					lblNumber->setString("");
					//更新双方的颜色
					newBlock->setColor(block->getColor());
					block->setColor(Color3B::WHITE);
					m_isMoved = true;
				}
				if (preString != "" && preString == newLabel->getString())
				{
					auto preCol = newCol + 1;
					auto preBlock = m_blocks[row * COL_COUNT + preCol];
					auto preLabel = dynamic_cast<Label *>(preBlock->getChildByTag(BLOCK_LABEL_TAG));

					auto number = atoi(preLabel->getString().c_str()) * 2;
					preLabel->setString(StringUtils::format("%d", number));
					newLabel->setString("");

					preBlock->setColor(Color3B(250 - sqrt(number) * 4, 250 - sqrt(number) * 4, 150 + sqrt(number) * 3));
					newBlock->setColor(Color3B::WHITE);

					spaceBlockCount++;
					m_spaceBlockCount++;

					preString = "";
					m_isMoved = true;
					if (number >= 2048)
					{
						uWin();
					}
				}
				else {
					preString = newLabel->getString();
				}
			}
		}
	}
}

void GameScene::uWin() {
	auto prop = this->getChildByTag(PROP_LAYER_TAG);
	auto lbl = dynamic_cast<Label *>(prop->getChildByTag(PROP_LAYER_LABEL_MESSAGE_TAG));

	lbl->setString("You Win!");
	lbl->runAction(Spawn::create(RotateBy::create(0.5f, 360), ScaleTo::create(0.5f, 2), nullptr));
	prop->setVisible(true);
	//禁止滑屏
	m_canMove = false;
}

void GameScene::over() {
	auto prop = this->getChildByTag(PROP_LAYER_TAG);
	auto lbl = dynamic_cast<Label *>(prop->getChildByTag(PROP_LAYER_LABEL_MESSAGE_TAG));

	lbl->setString("Game Over!");
	lbl->runAction(Spawn::create(RotateBy::create(0.5f, 360), ScaleTo::create(0.5f, 2), nullptr));
	prop->setVisible(true);
	//禁止滑屏
	m_canMove = false;
}

void GameScene::reStart(Ref *) {
	auto scene = GameScene::createScene();
	auto transition = TransitionTurnOffTiles::create(4, scene);
	Director::getInstance()->pushScene(transition);
}
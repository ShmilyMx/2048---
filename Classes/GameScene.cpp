#include "GameScene.h"
#include "CommonData.h"

GameScene::GameScene() {
	//����Ŀհ׸���ʮ����
	this->m_spaceBlockCount = BLOCK_COUNT;

	//����ʮ����ָ��ռ�
	auto size = sizeof(LayerColor *) * BLOCK_COUNT;
	//�������ָ��ռ�
	this->m_blocks = (LayerColor **)malloc(size);
	//��ʼ����ָ��
	memset(this->m_blocks, 0, size);

	this->m_startPos = Vec2::ZERO;
	this->m_isMoved = false;
	this->m_canMove = true;
}

GameScene::~GameScene() {
	//�������ͷž���ռ�
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

		//����
		auto bg = LayerColor::create(Color4B::ORANGE, BACKGROUND_LENGTH, BACKGROUND_LENGTH);
		bg->setPosition(origin + Vec2(visibleSize)/2);
		//layerĬ��ê�㲻��Ӱ������
		bg->ignoreAnchorPointForPosition(false);
		this->addChild(bg, BACKGROUND_ZORDER, BACKGROUND_TAG);

		//ѭ����Ӹ��ӱ���
		for (auto i = 0; i < BLOCK_COUNT; i++)
		{
			initBlock(i);
		}
			
		//�������������
		createBlock();
		createBlock();

		//�����¼�
		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = [&](Touch *touch, Event *) {
			m_startPos = touch->getLocation();
			return m_canMove;
		};
		listener->onTouchEnded = CC_CALLBACK_2(GameScene::moveBlock, this);

		//�ڶ���������ʾ����˭Ϊ�¼����ȼ��Ĳ��գ��������ȼ����
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		//�������ֲ�
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
	//��ӱ�ǩ
	auto lblNumber = Label::createWithTTF("", "Marker Felt.ttf", 36);
	lblNumber->setPosition(Vec2(block->getContentSize()) / 2);
	block->addChild(lblNumber, BLOCK_LABEL_ZORDER, BLOCK_LABEL_TAG);

	//��λ
	auto x = SPACE_LENGTH + (BLOCK_LENGTH + SPACE_LENGTH) * (index % ROW_COUNT);
	auto y = SPACE_LENGTH + (BLOCK_LENGTH + SPACE_LENGTH) * (index / ROW_COUNT);
	block->setPosition(x, y);
	bg->addChild(block);

	//ÿ����һ��ͱ��浽�����У��������������Щ��
	m_blocks[index] = block;
}

void GameScene::createBlock() {
	//���пհ׸�
	if (this->m_spaceBlockCount <= 0) return;

	//�������λ�ò���ӵ�������
	do
	{
		//�������λ���±�
		auto index = rand() % BLOCK_COUNT;
		//�������������±�λ���Ѿ��п��˾����²����±�
		//��ȡ���λ�ÿ�ı�ǩ
		auto lblNumber = dynamic_cast<Label *>(m_blocks[index]->getChildByTag(BLOCK_LABEL_TAG));
		//������λ�õĿ�ı�ǩ�����������»�ȡ
		if (lblNumber->getString() != "") continue;

		//����������֣�2��4��
		auto number = rand() % 10 < 9 ? 2 : 4;
		//����Label��ʾ
		lblNumber->setString(StringUtils::format("%d", number));
		//�������־�����ɫ
		m_blocks[index]->setColor(Color3B(250-sqrt(number)*4, 250-sqrt(number)*4, 150+sqrt(number)*3));
		//�Ŵ󶯻�
		m_blocks[index]->setScale(0.5f);
		m_blocks[index]->runAction(ScaleTo::create(0.1f, 1));
		//��ȷ�ı��ǩ���˳�ѭ��
		break;
	} while (1);

	//����һ�����һ��
	this->m_spaceBlockCount--;
}

void GameScene::moveBlock(Touch *touch, Event *) {
	auto endPos = touch->getLocation();
	auto vec = endPos - m_startPos;

	//������������������С����С��Ӧ���룬�Ͳ���Ӧ
	if (fabs(vec.x) < MIN_STEP && fabs(vec.y) < MIN_STEP) return;
	//�ֳ���ˮƽ��Ӧ���Ǵ�ֱ������Ӧ
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

	//�����¿�
	if (m_isMoved)
	{
		createBlock();
		//���û����ƶ�����
		m_isMoved = false;
	}
	else if (m_spaceBlockCount <= 0)
	{
		over();
	}
}

void GameScene::down() {
	//log("=====down");
	//��������˵��ѭ�����У���ѭ������
	for (int col = 0; col < BLOCK_COUNT/ROW_COUNT; col++)
	{
		//��¼�հ׸�����
		int spaceBlockCount = 0;
		//ǰһ�����ӵı�ǩֵ
		std::string preString = "";

		for (int row = 0; row < ROW_COUNT; row++)
		{
			//�õ�����ĸ���
			auto block = m_blocks[row * COL_COUNT + col];
			//�õ������еı�ǩ
			auto lblNumber = dynamic_cast<Label *>(block->getChildByTag(BLOCK_LABEL_TAG));
			//ȡ���Լ���ǩ��ֵ
			auto lblString = lblNumber->getString();
			//��ǩֵΪ�գ����ۼ�����
			if (lblString == "")
			{
				spaceBlockCount++;
			}
			else {
				//������ӵ���λ��
				auto newRow = row - spaceBlockCount;
				auto newBlock = m_blocks[newRow * COL_COUNT + col];
				auto newLabel = dynamic_cast<Label *>(newBlock->getChildByTag(BLOCK_LABEL_TAG));

				if (spaceBlockCount > 0)	//��ǩ��ֵ���пհ׸񣬾�Ҫ����λ��
				{
					auto other = m_blocks[newRow * COL_COUNT + col];
					auto otherLabel = dynamic_cast<Label *>(other->getChildByTag(BLOCK_LABEL_TAG));
					//����˫���ı�ǩֵ
					otherLabel->setString(lblString);
					lblNumber->setString("");
					//����˫������ɫ
					other->setColor(block->getColor());
					block->setColor(Color3B::WHITE);

					//�ƶ���
					m_isMoved = true;
				}
				//�ϲ�
				if (preString != "" && preString == newLabel->getString())
				{
					//��ȡǰһ��
					auto preRow = newRow - 1;
					auto preBlock = m_blocks[preRow * COL_COUNT + col];
					auto preLabel = dynamic_cast<Label *>(preBlock->getChildByTag(BLOCK_LABEL_TAG));
					auto number = atoi(preString.c_str()) * 2;
					//ǰһ��ֵdouble����ǰ�����
					preLabel->setString(StringUtils::format("%d", number));
					newLabel->setString("");
					//���¸ı���ɫ
					preBlock->setColor(Color3B(250 - sqrt(number) * 4, 250 - sqrt(number) * 4, 150 + sqrt(number) * 3));
					newBlock->setColor(Color3B::WHITE);
					//�����ϲ�����λ�հ׸��һ�������еĿհ׸�Ҳ��һ��
					spaceBlockCount++;
					m_spaceBlockCount++;

					//ǰһ�����ֵ�Ѿ���ʹ�ã����
					preString = "";

					//�ϲ�����
					m_isMoved = true;

					if (number >= 2048)
					{
						uWin();
					}
				}
				else {
					//�õ�ǰ���ֵ�滻����Ϊֻ��������ֵ��ȵķǿհ׿�ſ��Ժϲ�
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
			//�õ�����ĸ���
			auto block = m_blocks[row * COL_COUNT + col];
			//�õ������еı�ǩ
			auto lblNumber = dynamic_cast<Label *>(block->getChildByTag(BLOCK_LABEL_TAG));
			//��ǩֵΪ�գ����ۼ�����
			if (lblNumber->getString() == "")
			{
				spaceBlockCount++;
			}
			else {
				//�õ���λ�õĸ���
				auto newRow = row + spaceBlockCount;
				auto newBlock = m_blocks[newRow * COL_COUNT + col];
				auto newLabel = dynamic_cast<Label *>(newBlock->getChildByTag(BLOCK_LABEL_TAG));

				if (spaceBlockCount > 0)	//��ǩ��ֵ���пհ׸�
				{
					//����˫���ı�ǩֵ
					newLabel->setString(lblNumber->getString());
					lblNumber->setString("");
					//����˫������ɫ
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
			//�õ�����ĸ���
			auto block = m_blocks[row * COL_COUNT + col];
			//�õ������еı�ǩ
			auto lblNumber = dynamic_cast<Label *>(block->getChildByTag(BLOCK_LABEL_TAG));
			//��ǩֵΪ�գ����ۼ�����
			if (lblNumber->getString() == "")
			{
				spaceBlockCount++;
			}
			else {
				auto newCol = col - spaceBlockCount;
				auto newBlock = m_blocks[row * COL_COUNT + newCol];
				auto newLabel = dynamic_cast<Label *>(newBlock->getChildByTag(BLOCK_LABEL_TAG));
				if (spaceBlockCount > 0)	//��ǩ��ֵ���пհ׸�
				{
					//����˫���ı�ǩֵ
					newLabel->setString(lblNumber->getString());
					lblNumber->setString("");
					//����˫������ɫ
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
			//�õ�����ĸ���
			auto block = m_blocks[row * COL_COUNT + col];
			//�õ������еı�ǩ
			auto lblNumber = dynamic_cast<Label *>(block->getChildByTag(BLOCK_LABEL_TAG));
			//��ǩֵΪ�գ����ۼ�����
			if (lblNumber->getString() == "")
			{
				spaceBlockCount++;
			}
			else {
				auto newCol = col + spaceBlockCount;
				auto newBlock = m_blocks[row * COL_COUNT + newCol];
				auto newLabel = dynamic_cast<Label *>(newBlock->getChildByTag(BLOCK_LABEL_TAG));
				if (spaceBlockCount > 0)	//��ǩ��ֵ���пհ׸�
				{
					//����˫���ı�ǩֵ
					newLabel->setString(lblNumber->getString());
					lblNumber->setString("");
					//����˫������ɫ
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
	//��ֹ����
	m_canMove = false;
}

void GameScene::over() {
	auto prop = this->getChildByTag(PROP_LAYER_TAG);
	auto lbl = dynamic_cast<Label *>(prop->getChildByTag(PROP_LAYER_LABEL_MESSAGE_TAG));

	lbl->setString("Game Over!");
	lbl->runAction(Spawn::create(RotateBy::create(0.5f, 360), ScaleTo::create(0.5f, 2), nullptr));
	prop->setVisible(true);
	//��ֹ����
	m_canMove = false;
}

void GameScene::reStart(Ref *) {
	auto scene = GameScene::createScene();
	auto transition = TransitionTurnOffTiles::create(4, scene);
	Director::getInstance()->pushScene(transition);
}
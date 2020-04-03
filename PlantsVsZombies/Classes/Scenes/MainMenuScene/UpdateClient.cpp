/**
 *Copyright (c) 2020 LZ.All Right Reserved
 *Author : LZ
 *Date: 2020.4.2
 *Email: 2117610943@qq.com
 */

#include "UpdateClient.h"
#include "spine/spine-cocos2dx.h"

using namespace spine;

UpdateClient::UpdateClient() :
	_dialog(nullptr)
	, _remindText(nullptr)
	, _progressText(nullptr)
	, _explanText(nullptr)
	, _loadBarBackground(nullptr)
	, _loadingBar(nullptr)
{
	_downloader.reset(new network::Downloader());
}

bool UpdateClient::init()
{
	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 180)))return false;
	createShieldLayer(this);

	createDiglog();
	return true;
}

void UpdateClient::createDiglog()
{
	_dialog = Sprite::createWithSpriteFrameName("LevelObjiectivesBg.png");
	_dialog->setPosition(_director->getWinSize() / 2);
	_dialog->setScale(0.9f);
	this->addChild(_dialog);

	auto PauseAnimation = SkeletonAnimation::createWithData(_global->userInformation->getAnimationData().find("PauseAnimation")->second);
	PauseAnimation->setAnimation(0, "animation", true);
	PauseAnimation->setPosition(Vec2(530, 650));
	_dialog->addChild(PauseAnimation);

	showText();

	createButton(_global->userInformation->getGameText().find("�ٶ���������")->second, Vec2(165, 100), Update_Button::�ٶ���������);
	createButton(_global->userInformation->getGameText().find("��Ѷ΢������")->second, Vec2(405, 100), Update_Button::��Ѷ΢������);
	createButton(_global->userInformation->getGameText().find("ֱ������")->second, Vec2(645, 100), Update_Button::ֱ������);
	createButton(_global->userInformation->getGameText().find("�ر���Ϸ")->second, Vec2(885, 100), Update_Button::�˳���Ϸ);
	createButton(_global->userInformation->getGameText().find("ȷ��")->second, Vec2(520, 100), Update_Button::ȷ��);

	/* ������������ */
	createTouchtListener(_dialog);
}

void UpdateClient::createButton(const std::string& name, Vec2& vec2, Update_Button buttonType)
{
	/* �����������˵���ť */
	auto button = ui::Button::create("ButtonNew2.png", "ButtonNew.png", "", TextureResType::PLIST);
	auto label = Label::createWithTTF(name, GAME_FONT_NAME_1, 35);
	label->enableShadow(Color4B(0, 0, 0, 200));//������Ӱ
	label->setScale(2.0f);
	button->setTitleLabel(label);
	button->setTitleColor(Color3B::WHITE);
	button->setPosition(vec2);
	button->setScale(0.5f);
	button->setName(to_string(static_cast<int>(buttonType)));
	_dialog->addChild(button);

	if (buttonType == Update_Button::ȷ��)
	{
		button->setVisible(false);
	}

	button->addTouchEventListener([=](Ref* sender, ui::Widget::TouchEventType type)
		{
			switch (type)
			{
			case ui::Widget::TouchEventType::BEGAN:
				AudioEngine::setVolume(AudioEngine::play2d(_global->userInformation->getMusicPath().find("gravebutton")->second), _global->userInformation->getSoundEffectVolume());
				break;
			case ui::Widget::TouchEventType::ENDED:
				switch (buttonType)
				{
				case Update_Button::�ٶ���������:
					Application::getInstance()->openURL(_global->userInformation->getGameText().find("�ٶ�������ַ")->second);
					break;
				case Update_Button::��Ѷ΢������:
					Application::getInstance()->openURL(_global->userInformation->getGameText().find("��Ѷ΢����ַ")->second);
					break;
				case Update_Button::ֱ������:
					_remindText->setString(_global->userInformation->getGameText().find("���������У����Եȣ�")->second);
					_remindText->setPosition(Vec2(_dialog->getContentSize().width / 2.f, _dialog->getContentSize().height / 2.f + 200));
					button->setEnabled(false);
					((Button*)_dialog->getChildByName("3"))->setEnabled(false);
					downloadData();
					break;
				case Update_Button::�˳���Ϸ:
				case Update_Button::ȷ��:
					Director::getInstance()->end();
					break;
				}
			}
		});
}

void UpdateClient::showText()
{
	/* ���� */
	_remindText = Label::createWithTTF(_global->userInformation->getGameText().find("��⵽���°汾����ѡ����·�ʽ��")->second, GAME_FONT_NAME_1, 50);
	_remindText->setPosition(Vec2(_dialog->getContentSize().width / 2.f, _dialog->getContentSize().height / 2.f));
	_remindText->setColor(Color3B::BLACK);
	_remindText->setMaxLineWidth(900);
	_remindText->setName("Update");
	_dialog->addChild(_remindText);

	/* �������� */
	_progressText = Label::createWithTTF("", GAME_FONT_NAME_1, 30);
	_progressText->setMaxLineWidth(900);
	_progressText->setPosition(Vec2(_dialog->getContentSize().width / 2.f, _dialog->getContentSize().height / 2.f));
	_dialog->addChild(_progressText);

	/* ˵������ */
    _explanText = Label::createWithTTF("", GAME_FONT_NAME_1, 30);
	_explanText->setPosition(Vec2(_dialog->getContentSize().width / 2.f, _dialog->getContentSize().height / 2.f + 100));
	_explanText->setColor(Color3B::BLACK);
	_explanText->setMaxLineWidth(900);
	_dialog->addChild(_explanText);
}

void UpdateClient::downloadData()
{
	if (!_loadBarBackground)
	{
		_loadBarBackground = Sprite::createWithSpriteFrameName("bgFile.png");
		_loadBarBackground->setPosition(Vec2(_dialog->getContentSize().width / 2.f, _dialog->getContentSize().height / 2.f - 100));
		_loadBarBackground->setScale(1.5f);
		_dialog->addChild(_loadBarBackground);
	}

	if (!_loadingBar)
	{
		_loadingBar = ui::LoadingBar::create();
		_loadingBar->loadTexture("progressFile.png", TextureResType::PLIST);
		_loadingBar->setDirection(LoadingBar::Direction::LEFT); /* ���ü��ط��� */
		_loadingBar->setPercent(0);
		_loadingBar->setScale(1.5f);
		_loadingBar->setPosition(Vec2(_dialog->getContentSize().width / 2.f, _dialog->getContentSize().height / 2.f - 100));
		_dialog->addChild(_loadingBar);
	}

	_explanText->setString(_global->userInformation->getGameText().find("����˵��")->second);
	_explanText->setPosition(Vec2(_dialog->getContentSize().width / 2.f, _dialog->getContentSize().height / 2.f + 100));
	_explanText->setColor(Color3B::BLACK);

	const static string sNameList = _global->userInformation->getGameText().find("��Դ����")->second + UserInformation::getNewEditionName() + ".rar";
	const static string path = _global->userInformation->getGameText().find("���·��")->second + sNameList;

	_downloader->createDownloadFileTask(_global->userInformation->getGameText().find("��Դ��ַ")->second, path, sNameList);
	_downloader->onTaskProgress = [=](const network::DownloadTask& task,
		int64_t bytesReceived,
		int64_t totalBytesReceived,
		int64_t totalBytesExpected)
	{
		float percent = float(totalBytesReceived * 100) / totalBytesExpected;
		_loadingBar->setPercent(percent);

		char buf[64];
		snprintf(buf, 64, "[������ %d KB] %.2f %%[�ܹ� %d KB]", int(totalBytesReceived / 1024), percent, int(totalBytesExpected / 1024));
		_progressText->setString(buf);

		_remindText->setString(_global->userInformation->getGameText().find("�ļ����������У����Եȣ�")->second);
	};

	_downloader->onFileTaskSuccess = [this](const cocos2d::network::DownloadTask& task)
	{
		char buf[128];
		snprintf(buf, 128, _global->userInformation->getGameText().find("���سɹ�")->second.c_str(),
			task.identifier.c_str(), _global->userInformation->getGameText().find("���·��")->second);
		_progressText->setString(buf);
		_remindText->setString(_global->userInformation->getGameText().find("���ȷ���˳���Ϸ��")->second);

		((Button*)_dialog->getChildByName("0"))->setVisible(false);
		((Button*)_dialog->getChildByName("1"))->setVisible(false);
		((Button*)_dialog->getChildByName("2"))->setVisible(false);
		((Button*)_dialog->getChildByName("3"))->setVisible(false);
		((Button*)_dialog->getChildByName("4"))->setVisible(true);
	};

	_downloader->onTaskError = [this](const cocos2d::network::DownloadTask& task,
		int errorCode,
		int errorCodeInternal,
		const std::string& errorStr)
	{
		_remindText->setString(_global->userInformation->getGameText().find("����ʧ��")->second);
		_remindText->setPosition(Vec2(_dialog->getContentSize().width / 2.f, _dialog->getContentSize().height / 2.f + 200));
		((Button*)_dialog->getChildByName("2"))->setEnabled(true);
		((Button*)_dialog->getChildByName("3"))->setEnabled(true);

		char str[256];
		snprintf(str, 256, "Failed to download : ��Դ�ļ�, identifier(%s) error code(%d), internal error code(%d) desc(%s) �������������Ƿ�����!�������������������Լ���!�����������ʽ����!"
			, task.identifier.c_str()
			, errorCode
			, errorCodeInternal
			, errorStr.c_str());
		_explanText->setString(str);
		_explanText->setColor(Color3B::RED);
#ifdef DEBUG
		log("Failed to download : %s, identifier(%s) error code(%d), internal error code(%d) desc(%s)"
			, task.requestURL.c_str()
			, task.identifier.c_str()
			, errorCode
			, errorCodeInternal
			, errorStr.c_str());
#endif // DEBUG
	};
}

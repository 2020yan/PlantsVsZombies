/**
 *Copyright (c) 2020 LZ.All Right Reserved
 *Author : LZ
 *Date: 2020.2.19
 *Email: 2117610943@qq.com
 */

#include "FirePea.h"

#include "Zombies/Zombies.h"
#include "Scenes/GameScene/GSData.h"

FirePea::FirePea(Node* node)
{
    _node = node;
    _attack = 40;
    _bulletType = BulletType::FirePea;

    _isFire = true;
    _fireNumbers = 1;
    _peaAnimationName = "fire";
}

FirePea::~FirePea()
{
}

void FirePea::createBullet()
{
    bulletInit();
    createShadow();
}

void FirePea::bulletInit()
{
    _bulletAnimation = SkeletonAnimation::createWithData(_global->userInformation->getAnimationData().find("FirePea")->second);
    _bulletAnimation->setAnimation(0, _peaAnimationName, true);
    _bulletAnimation->setPosition(_position + Vec2(70, 85));
    _bulletAnimation->setScale(1.5f);
    _bulletAnimation->setName(_bulletName);
    _bulletAnimation->setLocalZOrder(getZOrder(_position.y));
    _node->addChild(_bulletAnimation);
    _bulletAnimation->runAction(Sequence::create(MoveBy::create(3.0f, Vec2(2000, 0)),
        CallFunc::create([=]()
            {
                _bulletAnimation->setVisible(false);
            }), nullptr));
}

void FirePea::createShadow()
{
    /* ����Ӱ�� */
    auto shadow = Sprite::createWithSpriteFrameName("plantshadow.png");
    shadow->setScaleY(0.7f);
    shadow->setName("shadow");
    shadow->setPosition(Vec2(0, -52));
    shadow->setLocalZOrder(_bulletAnimation->getLocalZOrder());
    _bulletAnimation->addChild(shadow, -1);
}

void FirePea::bulletAndZombiesCollision()
{
    for (auto zombie : ZombiesGroup)
    {
        if (!_isUsed && getBulletIsSameLineWithZombie(zombie) &&           /* �㶹û�б�ʹ�� && �㶹�뽩ʬ��ͬһ�� */
            getBulletIsEncounterWithZombie(zombie) &&                      /* �㶹�뽩ʬ��ײ */
            zombie->getZombieIsSurvive() && zombie->getZombieIsEnterMap()) /* ��ʬû������ && ��ʬ�����ͼ�� */
        {
            playSoundEffect(SoundEffectType::firepea); /* ����ָ������ */

            setBulletOpacity();                /* �ӵ���ʧ */
            attackZombies();                   /* ������ʬ */
            createPeaExplode();                /* �����㶹��ը���� */
            setBulletAttack(0);
            setBulletIsUsed(true);

            break;
        }
    }

}

void FirePea::attackZombies()
{
    for (auto zombie : ZombiesGroup)
    {
        if (getBulletIsSameLineWithZombie(zombie) &&
            zombie->getZombieIsSurvive() &&
            zombie->getZombieIsEnterMap())
        {
            auto attack = static_cast<int>(getZombieInExplodeRange(zombie) / 50 * 20);
            _attack = 40;

            if (attack > 0) _attack -= attack;
            if (_attack <= 0) continue;
            bulletAttackHurtZombies(zombie);
            zombie->setZombieHurtBlink();
        }
    }
}

float FirePea::getZombieInExplodeRange(Zombies* zombie)
{
    /* ��ʬ�Ƿ��ڱ�ը��Χ�ж� */
    return zombie->getZombieAnimation()->getPositionX() - _bulletAnimation->getPositionX();
}
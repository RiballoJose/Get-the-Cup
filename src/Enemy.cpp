#include <Enemy.h>

Enemy::Enemy(std::string name, Ogre::SceneNode* node, Ogre::Entity* ent)
{
  _name = name;
  _node = node;
  _ent = ent;
}

Ghost::~Enemy(){
  delete _node;
}

void Enemy::setEntity(Ogre::Entity* ent){_ent = ent;}

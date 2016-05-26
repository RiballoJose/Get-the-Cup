#ifndef Ghost_H
#define Ghost_H

#include <Ogre.h>

class Enemy{
 public:
  Enemy (std::string name, Ogre::SceneNode* node, Ogre::Entity* ent);
  ~Enemy ();

  std::string name() const {return _name;}
  Ogre::SceneNode* getNode() const{return _node;}
  Ogre::Entity* getEntity() const{return _ent;}

 private:
  std::string _name;
  Ogre::SceneNode* _node;
  Ogre::Entity* _ent;
};

#endif

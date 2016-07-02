#include "PlayState.h"
#include "PauseState.h"
#include "NextLevelState.h"

template<> PlayState* Ogre::Singleton<PlayState>::msSingleton = 0;

void
PlayState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _viewport->setClearEveryFrame(true);
  _viewport->setOverlaysEnabled(false);
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().hide();
  initLights();
  createScene();
  initBullet();
  _currentLevel = 1;
  loadLevel();
  _exitGame = _nextLevel = false;
}

void
PlayState::exit ()
{
  _sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();
}

void
PlayState::pause()
{
}

void
PlayState::resume()
{
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().hide();
}

void
PlayState::initLights()
{
  _sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
  _sceneMgr->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5) );
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.9, 0.9, 0.9));
  _sceneMgr->setShadowTextureCount(2);
  _sceneMgr->setShadowTextureSize(512);
  
  _light = _sceneMgr->createLight("Light");
  _light->setPosition(-5, 12, 2);
  _light->setType(Ogre::Light::LT_SPOTLIGHT);
  _light->setDirection(Ogre::Vector3(1,-1,0));
  _light->setSpotlightInnerAngle(Ogre::Degree(25.0f));
  _light->setSpotlightOuterAngle(Ogre::Degree(60.0f));
  _light->setSpotlightFalloff(0.0f);
  _light->setCastShadows(true);

  _camera->setPosition(Ogre::Vector3(17, 16, -4));
  _camera->lookAt(Ogre::Vector3(-3, 2.7, 0));
  _camera->setNearClipDistance(1);
  _camera->setFOVy(Ogre::Degree(38));
}

void
PlayState::initBullet()
{

  Ogre::Vector3 pos = Ogre::Vector3(0,10,5);
  Ogre::Vector3 dir = Ogre::Vector3(0,0,0);

  int fuerza = 5;
  
  Ogre::Entity* entity = _sceneMgr->createEntity("ball", "Player.mesh");
  _player = _sceneMgr->getRootSceneNode()->createChildSceneNode();
  _player->attachObject(entity);

  OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = NULL; 
  OgreBulletCollisions::CollisionShape *bodyShape = NULL;

  trimeshConverter = new 
    OgreBulletCollisions::StaticMeshToShapeConverter(entity);
  bodyShape = trimeshConverter->createConvex();

  _player_rb = new OgreBulletDynamics::RigidBody("rigidBody", _world);

  _player_rb->setShape(_player, bodyShape,
         0.6 /* Restitucion */, 0.6 /* Friccion */,
         5.0 /* Masa */, pos /* Posicion inicial */,
         Ogre::Quaternion::IDENTITY /* Orientacion */);

  _player_rb->setLinearVelocity(dir * fuerza);

  _shapes.push_back(bodyShape);   _bodies.push_back(_player_rb);
}

void
PlayState::createScene()
{
  Ogre::SceneNode* node = NULL;
  /* Fisica */
  _debugDrawer = new OgreBulletCollisions::DebugDrawer();
  _debugDrawer->setDrawWireframe(true);  
  node = _sceneMgr->getRootSceneNode()->createChildSceneNode(
    "debugNode", Ogre::Vector3::ZERO);
  node->attachObject(static_cast<Ogre::SimpleRenderable *>(_debugDrawer));

  Ogre::AxisAlignedBox worldBounds = Ogre::AxisAlignedBox (
    Ogre::Vector3 (-10000, -10000, -10000), 
    Ogre::Vector3 (10000,  10000,  10000));
  Ogre::Vector3 gravity = Ogre::Vector3(0, -10, 0);

  _world = new OgreBulletDynamics::DynamicsWorld(_sceneMgr,
     worldBounds, gravity);
  _world->setDebugDrawer (_debugDrawer);
  _world->setShowDebugShapes (false);  // Muestra los collision shapes

  
  _meta_n = _sceneMgr->getRootSceneNode()->createChildSceneNode("Meta", Ogre::Vector3(0,1.5,-18.5));
  _meta_e =  _sceneMgr->createEntity("Muro.mesh");
  _meta_n->setScale(0.5, 0.5, 0.1);
  _meta_n->attachObject(_meta_e);
}

void PlayState::loadLevel()
{
  std::stringstream blq; blq.str("");
  if(_currentLevel==1){
    Ogre::Entity* ent_l1 =  _sceneMgr->createEntity("Level1.mesh");
    Ogre::SceneNode* nodo_l1 = _sceneMgr->getRootSceneNode()->createChildSceneNode("Level1", Ogre::Vector3(0,0,-17));
    nodo_l1->attachObject(ent_l1);
    OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent_l1);
    OgreBulletCollisions::CollisionShape *level1_sh = trimeshConverter->createConvex();
    OgreBulletDynamics::RigidBody *level1_rb = new OgreBulletDynamics::RigidBody("Level1", _world);
    level1_rb->setStaticShape(nodo_l1, level1_sh, 0.6, 1.0 , /*5.0,*/ Ogre::Vector3(0,0,-17), Ogre::Quaternion::IDENTITY);
    level1_rb->setGravity(0.0,0.0,0.0);
    level1_rb->setKinematicObject(true);
    _shapes.push_back(level1_sh);   _bodies.push_back(level1_rb);

    Ogre::SceneNode* l1_n1 = _sceneMgr->getRootSceneNode()->createChildSceneNode("Cup1_1", Ogre::Vector3(0,1.5,-8.5));
    Ogre::Entity* l1_e1 =  _sceneMgr->createEntity("Muro.mesh");
    l1_n1->attachObject(l1_e1);
    _cups.push_back(l1_n1);
    Ogre::SceneNode* l1_n2 = _sceneMgr->getRootSceneNode()->createChildSceneNode("Cup1_2", Ogre::Vector3(-3,1.5,-12.5));
    Ogre::Entity* l1_e2 =  _sceneMgr->createEntity("Muro.mesh");
    l1_n2->attachObject(l1_e2);
    _cups.push_back(l1_n2);
    Ogre::SceneNode* l1_n3 = _sceneMgr->getRootSceneNode()->createChildSceneNode("Cup1_3", Ogre::Vector3(3,1.5,-12.5));
    Ogre::Entity* l1_e3 =  _sceneMgr->createEntity("Muro.mesh");
    l1_n3->attachObject(l1_e3);
    _cups.push_back(l1_n3);
    
    _meta_n->setPosition(0,1.5,-18.5);
  }
  _player->setPosition(0,10,5);
}
void PlayState::nextLevel()
{
  _currentLevel++;
  removeLevel();
  pushState(NextLevelState::getSingletonPtr());
  loadLevel();
}

void PlayState::removeLevel()
{

}

void PlayState::updatePlayer()
{
  int vel = 1;
  Ogre::Vector3 lv = Ogre::Vector3::ZERO;
  bool move = false;
  if(_arriba){
    lv+=Ogre::Vector3(0,0,-1*vel);
    move = true;
  }
  if(_abajo){
    lv+=Ogre::Vector3(0,0,1*vel);
    move = true;
  }
  if(_izquierda){
    lv+=Ogre::Vector3(-1*vel,0,0);
    move = true;
  }
  if(_derecha){
    lv+=Ogre::Vector3(1*vel,0,0);
    move = true;
  }
  if(_salto && _player_rb->getCenterOfMassPosition().y < 1.5){
    lv+=Ogre::Vector3(0,0.5*vel,0);
    move = true;
  }
  if(move){
    _player_rb->enableActiveState();
    _player_rb->applyImpulse(lv, _player_rb->getCenterOfMassPosition());
  }
}
void PlayState::detectCollisions()
{
  std::stringstream blq; blq.str("Cup");
  blq << _currentLevel << "_";
  Ogre::SceneNode* nodo = NULL;
  try{nodo = _sceneMgr->getSceneNode("Cup1_1");}catch(...){}
  if(nodo){
    Ogre::AxisAlignedBox bboxPac = _player->_getWorldAABB();
    Ogre::AxisAlignedBox bboxDot = nodo->_getWorldAABB();
    if(bboxPac.intersects(bboxDot)){
      nodo->removeAndDestroyAllChildren();
      _sceneMgr->destroySceneNode(nodo);
    }
  }
  nodo = NULL;
  try{nodo = _sceneMgr->getSceneNode("Cup1_2");}catch(...){}
  if(nodo){
    Ogre::AxisAlignedBox bboxPac = _player->_getWorldAABB();
    Ogre::AxisAlignedBox bboxDot = nodo->_getWorldAABB();
    if(bboxPac.intersects(bboxDot)){
      nodo->removeAndDestroyAllChildren();
      _sceneMgr->destroySceneNode(nodo);
    }
  }
  nodo = NULL;
  try{nodo = _sceneMgr->getSceneNode("Cup1_3");}catch(...){}
  if(nodo){
    Ogre::AxisAlignedBox bboxPac = _player->_getWorldAABB();
    Ogre::AxisAlignedBox bboxDot = nodo->_getWorldAABB();
    if(bboxPac.intersects(bboxDot)){
      nodo->removeAndDestroyAllChildren();
      _sceneMgr->destroySceneNode(nodo);
    }
  }
  nodo = NULL;
  try{nodo = _sceneMgr->getSceneNode("Meta");}catch(...){}
  if(nodo){
    Ogre::AxisAlignedBox bboxPac = _player->_getWorldAABB();
    Ogre::AxisAlignedBox bboxDot = nodo->_getWorldAABB();
    if(bboxPac.intersects(bboxDot)){
      _nextLevel = true;
    }
  }
}

bool
PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{
  _deltaT = evt.timeSinceLastFrame;
  _world->stepSimulation(_deltaT);
  updatePlayer();
  detectCollisions();
  _camera->setAspectRatio
   (Ogre::Real(_viewport->getActualWidth())/
    Ogre::Real(_viewport->getActualHeight()));
  _camera->setPosition(_player->getPosition()+Ogre::Vector3(0, 2.5, 10));
  _camera->lookAt(_player->getPosition().x, _player->getPosition().y+0.5, _player->getPosition().z);

  return true;
}

bool
PlayState::frameEnded
(const Ogre::FrameEvent& evt)
{
  _deltaT = evt.timeSinceLastFrame;
  _world->stepSimulation(_deltaT);

  if(_exitGame)
    return false;

  if(_nextLevel)
    nextLevel();
  
  //std::cout << _player->getPosition().y << std::endl;
  if(_jump && _player_rb->getLinearVelocity().y<0.05 && _player->getPosition().y <1.1)
    _jump = false;
  return true;
}

void
PlayState::keyPressed
(const OIS::KeyEvent &e)
{
  switch(e.key){
  case OIS::KC_P:
    pushState(PauseState::getSingletonPtr());
    break;
  case OIS::KC_SPACE:
    _salto=true;
    break;
  case OIS::KC_S:
    _abajo = true;
    break;
  case OIS::KC_D:
    _derecha = true;
    break;
  case OIS::KC_A:
    _izquierda = true;
    break;
  case OIS::KC_W:
    _arriba = true;
    break;
  default:
  break;
  }
}

void
PlayState::keyReleased
(const OIS::KeyEvent &e)
{
  switch(e.key){
  case OIS::KC_W:
    _arriba = false;
    break;
  case OIS::KC_S:
    _abajo = false;
    break;
  case OIS::KC_D:
    _derecha = false;
    break;
  case OIS::KC_A:
    _izquierda = false;
    break;
  case OIS::KC_SPACE:
    _salto = false;
    break;
  default:
    break;
  }
}

void
PlayState::mouseMoved
(const OIS::MouseEvent &evt)
{
  //_camera->yaw(Degree(evt.state.X.rel * -0.15f));
  //_camera->pitch(Degree(evt.state.Y.rel * -0.15f));
}

void
PlayState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
PlayState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

PlayState*
PlayState::getSingletonPtr ()
{
return msSingleton;
}

PlayState&
PlayState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

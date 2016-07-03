#include "PlayState.h"
#include "PauseState.h"
#include "NextLevelState.h"
#include "EndState.h"

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
  _pTrackManager = TrackManager::getSingletonPtr();
  _pSoundFXManager = SoundFXManager::getSingletonPtr();
  _mainTrack = _pTrackManager->load("music.ogg");
  _mainTrack->play();
  _simpleEffect = _pSoundFXManager->load("aplausos.ogg");
  createScene();
  initBullet();
  _currentLevel = 1; _lives = 3;
  loadLevel();
  _exitGame = _nextLevel = _noLives = _resetLevel = false;
  
}

void
PlayState::exit ()
{
  _sceneMgr->clearScene();
  _mainTrack->stop();
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
  resetLevel();
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
  _camera->setNearClipDistance(1);
  _camera->setFOVy(Ogre::Degree(38));
}

void
PlayState::initBullet()
{ 
  _ball_ent = _sceneMgr->createEntity("ball", "Player.mesh");
  _player = _sceneMgr->getRootSceneNode()->createChildSceneNode();
  _player->attachObject(_ball_ent);

  _ball_sh = new OgreBulletCollisions::SphereCollisionShape(Ogre::Real(0.375));
  _player_rb = new OgreBulletDynamics::RigidBody("rigidBody", _world);

  _player_rb->setShape(_player, _ball_sh,
         0.0 /* Restitucion */, 0.6 /* Friccion */,
         10.0 /* Masa */, Ogre::Vector3(0,10,5)/* Posicion inicial */,
         Ogre::Quaternion::IDENTITY /* Orientacion */);

  _shapes.push_back(_ball_sh);   _bodies.push_back(_player_rb);
}

void
PlayState::createScene()
{
  _sceneMgr->setSkyDome(true, "Sky", 5, 8);
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
    Ogre::Entity* ent_l1 =  _sceneMgr->createEntity("Plat_big.mesh");
    Ogre::SceneNode* nodo_l1 = _sceneMgr->getRootSceneNode()->createChildSceneNode("l11", Ogre::Vector3(0,0,-17));
    nodo_l1->attachObject(ent_l1);
    OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent_l1);
    OgreBulletCollisions::CollisionShape *level1_sh = trimeshConverter->createConvex();
    OgreBulletDynamics::RigidBody *level1_rb = new OgreBulletDynamics::RigidBody("l11", _world);
    level1_rb->setStaticShape(nodo_l1, level1_sh, 0.6, 1.0 , /*5.0,*/ Ogre::Vector3(0,0,-17), Ogre::Quaternion::IDENTITY);
    level1_rb->setGravity(0.0,0.0,0.0);
    level1_rb->setKinematicObject(true);
    _shapes.push_back(level1_sh);   _bodies.push_back(level1_rb);

    ent_l1 =  _sceneMgr->createEntity("Hall1.mesh");
    nodo_l1 = _sceneMgr->getRootSceneNode()->createChildSceneNode("l12", Ogre::Vector3(0,0,-7));
    nodo_l1->attachObject(ent_l1);
    trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent_l1);
    level1_sh = trimeshConverter->createConvex();
    level1_rb = new OgreBulletDynamics::RigidBody("l12", _world);
    level1_rb->setStaticShape(nodo_l1, level1_sh, 0.6, 1.0 , /*5.0,*/ Ogre::Vector3(0,0,-7), Ogre::Quaternion::IDENTITY);
    level1_rb->setGravity(0.0,0.0,0.0);
    level1_rb->setKinematicObject(true);
    _shapes.push_back(level1_sh);   _bodies.push_back(level1_rb);

    ent_l1 =  _sceneMgr->createEntity("Plat_big.mesh");
    nodo_l1 = _sceneMgr->getRootSceneNode()->createChildSceneNode("l13", Ogre::Vector3(0,0,3));
    nodo_l1->attachObject(ent_l1);
    trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent_l1);
    level1_sh = trimeshConverter->createConvex();
    level1_rb = new OgreBulletDynamics::RigidBody("l13", _world);
    level1_rb->setStaticShape(nodo_l1, level1_sh, 0.6, 1.0 , /*5.0,*/ Ogre::Vector3(0,0,3), Ogre::Quaternion::IDENTITY);
    level1_rb->setGravity(0.0,0.0,0.0);
    level1_rb->setKinematicObject(true);
    _shapes.push_back(level1_sh);   _bodies.push_back(level1_rb);

   Ogre::SceneNode* l1_n1 = _sceneMgr->getRootSceneNode()->createChildSceneNode("Cup1_1", Ogre::Vector3(0,1.5,-8.5));
    Ogre::Entity* l1_e1 =  _sceneMgr->createEntity("Cup.mesh");
    l1_n1->attachObject(l1_e1);
    _cups.push_back(l1_n1);
    Ogre::SceneNode* l1_n2 = _sceneMgr->getRootSceneNode()->createChildSceneNode("Cup1_2", Ogre::Vector3(-3,1.5,-12.5));
    Ogre::Entity* l1_e2 =  _sceneMgr->createEntity("Cup.mesh");
    l1_n2->attachObject(l1_e2);
    _cups.push_back(l1_n2);
    Ogre::SceneNode* l1_n3 = _sceneMgr->getRootSceneNode()->createChildSceneNode("Cup1_3", Ogre::Vector3(3,1.5,-12.5));
    Ogre::Entity* l1_e3 =  _sceneMgr->createEntity("Cup.mesh");
    l1_n3->attachObject(l1_e3);
    _cups.push_back(l1_n3);
    
    _meta_n->setPosition(0,1.5,-18.5);
  }
}
void PlayState::nextLevel()
{
  _currentLevel++;
  removeLevel();
  pushState(NextLevelState::getSingletonPtr());
  loadLevel();
}

void PlayState::resetLevel()
{
  _lives--;
  if(_lives<1){
    _noLives = true;
  }
  else{
    _player_rb->getBulletRigidBody()->translate(btVector3(0-_player_rb->getCenterOfMassPosition().x,10-_player_rb->getCenterOfMassPosition().y,5-_player_rb->getCenterOfMassPosition().z));
    _player_rb->getBulletRigidBody()->clearForces();
    _player_rb->setLinearVelocity(Ogre::Vector3(0,0,0));
    _player_rb->getBulletRigidBody()->setAngularVelocity(btVector3(0,0,0));
    _player_rb->getBulletRigidBody()->setLinearVelocity(btVector3(0,0,0));
    _player_rb->applyForce(Ogre::Vector3::ZERO, _player_rb->getCenterOfMassPosition());
  }
}
void PlayState::removeLevel()
{

}

void PlayState::died()
{
  pushState(EndState::getSingletonPtr());
}

void PlayState::updatePlayer()
{
  double vel = 10.0*_deltaT;
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
    //_simpleEffect->play();
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
      try{
	_simpleEffect->play();
      }
      catch(...){}
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
      try{
	_simpleEffect->play();
      }
      catch(...){}
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
      try{
	_simpleEffect->play();
      }
      catch(...){}
    }
  }
  nodo = NULL;
/*  try{nodo = _sceneMgr->getSceneNode("Meta");}catch(...){}
  if(nodo){
    Ogre::AxisAlignedBox bboxPac = _player->_getWorldAABB();
    Ogre::AxisAlignedBox bboxDot = nodo->_getWorldAABB();
    if(bboxPac.intersects(bboxDot)){
      _nextLevel = true;
    }
  }*/
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

  if(_player->getPosition().y < -5)
    resetLevel();

  if(_noLives)
    died();
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

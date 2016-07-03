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

  _currentLevel = 1; _lives = 3; _stops = 0; _score = 0; _time = 0;
  _exitGame = _nextLevel = _noLives = _resetLevel = _stopBall = _jumping =  false;

  _level = _sceneMgr -> getRootSceneNode() -> createChildSceneNode();
  
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().hide();
  initLights();
  //_sceneMgr->addRenderQueueListener(GameManager::getSingletonPtr()->getOverlaySystem());
  //_overlayManager = Ogre::OverlayManager::getSingletonPtr();
  createOverlay();
  _pTrackManager = TrackManager::getSingletonPtr();
  _pSoundFXManager = SoundFXManager::getSingletonPtr();
  _mainTrack = _pTrackManager->load("music.ogg");
  _mainTrack->play();
  _simpleEffect = _pSoundFXManager->load("aplausos.ogg");
  createScene();
  initBullet();
  loadLevel();
  
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
  _sheet->show();
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_sheet);
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().hide();
  //resetBall();
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
PlayState::createOverlay()
{
  /*_ovJuego = _overlayManager->getByName("Juego");
  _ovPunt = _overlayManager->getOverlayElement("Puntuacion");
  _ovVida = _overlayManager->getOverlayElement("Vida");
  _ovScore = _overlayManager->getOverlayElement("Puntos");
  _ovVida->setCaption("Vidas");
  _ovPunt->setCaption("Puntos");
  std::cout << "Overlays" << std::endl;
  try{_ovJuego->show();} catch(...){std::cout << "Overlay error" << std::endl;}*/

  _sheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Play");
  CEGUI::Window* configWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("play_score.layout");
  _scoreWin = configWin->getChild("Score");
  _sheet->addChild(configWin);
  _scoreWin->setText(Ogre::StringConverter::toString(_score));
  
  CEGUI::Window* configWin2 = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("play_lives.layout");
  _livesWin = configWin2->getChild("Lives");
  _sheet->addChild(configWin2);
  _livesWin->setText(Ogre::StringConverter::toString(_lives));
  
  CEGUI::Window* configWin3 = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("timer.layout");
  _timeWin = configWin3->getChild("Time");
  _sheet->addChild(configWin3);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_sheet);
  _timeWin->setText(Ogre::StringConverter::toString(_time));
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
         0.0 /* Restitucion */, 0.5 /* Friccion */,
         7.0 /* Masa */, Ogre::Vector3(0,10,5)/* Posicion inicial */,
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
  _meta_n->attachObject(_meta_e);
  _meta_e =  _sceneMgr->createEntity("Goal.mesh");
  _meta_n->roll(Degree(-90));
   _meta_n->pitch(Degree(90));
  _meta_n->setScale(0.5, 0.5, 0.5);
  _meta_n->attachObject(_meta_e);

}

void PlayState::loadLevel()
{
  std::stringstream blq; blq.str("");
  Ogre::Entity* ent = NULL;
  Ogre::SceneNode* nodo = NULL;
  OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = NULL;
  OgreBulletCollisions::CollisionShape *level_sh = NULL;
  OgreBulletDynamics::RigidBody *level_rb = NULL;
  switch(_currentLevel){
  case 1:
    ent =  _sceneMgr->createEntity("Plat_big.mesh");
    nodo = _level->createChildSceneNode("l11", Ogre::Vector3(0,0,-17));
    nodo->attachObject(ent);
    trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent);
    level_sh = trimeshConverter->createConvex();
    level_rb = new OgreBulletDynamics::RigidBody("l11", _world);
    level_rb->setStaticShape(nodo, level_sh, 0.6, 1.0 , /*5.0,*/ Ogre::Vector3(0,0,-17), Ogre::Quaternion::IDENTITY);
    level_rb->setGravity(0.0,0.0,0.0);
    level_rb->setKinematicObject(true);
    _shapes.push_back(level_sh);   _bodies.push_back(level_rb);

    ent =  _sceneMgr->createEntity("Hall1.mesh");
    nodo = _level->createChildSceneNode("l12", Ogre::Vector3(0,0,-7));
    nodo->attachObject(ent);
    trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent);
    level_sh = trimeshConverter->createConvex();
    level_rb = new OgreBulletDynamics::RigidBody("l12", _world);
    level_rb->setStaticShape(nodo, level_sh, 0.6, 1.0 , /*5.0,*/ Ogre::Vector3(0,0,-7), Ogre::Quaternion::IDENTITY);
    level_rb->setGravity(0.0,0.0,0.0);
    level_rb->setKinematicObject(true);
    _shapes.push_back(level_sh);   _bodies.push_back(level_rb);

    ent =  _sceneMgr->createEntity("Plat_big.mesh");
    nodo = _level->createChildSceneNode("l13", Ogre::Vector3(0,0,3));
    nodo->attachObject(ent);
    trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent);
    level_sh = trimeshConverter->createConvex();
    level_rb = new OgreBulletDynamics::RigidBody("l13", _world);
    level_rb->setStaticShape(nodo, level_sh, 0.6, 1.0 , /*5.0,*/ Ogre::Vector3(0,0,3), Ogre::Quaternion::IDENTITY);
    level_rb->setGravity(0.0,0.0,0.0);
    level_rb->setKinematicObject(true);
    _shapes.push_back(level_sh);   _bodies.push_back(level_rb);

    nodo = _level->createChildSceneNode("Cup1_1", Ogre::Vector3(0,1.5,-8.5));
    ent =  _sceneMgr->createEntity("Cup.mesh");
    nodo->attachObject(ent);
    _cups.push_back(nodo);
    nodo = _level->createChildSceneNode("Cup1_2", Ogre::Vector3(-3,1.5,-12.5));
    ent =  _sceneMgr->createEntity("Cup.mesh");
    nodo->attachObject(ent);
    _cups.push_back(nodo);
    nodo = _level->createChildSceneNode("Cup1_3", Ogre::Vector3(3,1.5,-12.5));
    ent =  _sceneMgr->createEntity("Cup.mesh");
    nodo->attachObject(ent);
    _cups.push_back(nodo);
    
    _meta_n->setPosition(0,1.5,-18.5);
    _stops = 1;
    break;
  case 2:
    ent =  _sceneMgr->createEntity("Plat_big.mesh");
    nodo = _level->createChildSceneNode("l21", Ogre::Vector3(0,0,3));
    nodo->attachObject(ent);
    trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent);
    level_sh = trimeshConverter->createConvex();
    level_rb = new OgreBulletDynamics::RigidBody("l21", _world);
    level_rb->setStaticShape(nodo, level_sh, 0.6, 1.0 , /*5.0,*/ Ogre::Vector3(0,0,3), Ogre::Quaternion::IDENTITY);
    level_rb->setGravity(0.0,0.0,0.0);
    level_rb->setKinematicObject(true);
    _shapes.push_back(level_sh);   _bodies.push_back(level_rb);

    ent =  _sceneMgr->createEntity("Ramp2.mesh");
    nodo = _level->createChildSceneNode("l22", Ogre::Vector3(0,0.75,-4.1));
    nodo->attachObject(ent);
    trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent);
    level_sh = trimeshConverter->createConvex();
    level_rb = new OgreBulletDynamics::RigidBody("l22", _world);
    level_rb->setStaticShape(nodo, level_sh, 0.6, 1.0 , /*5.0,*/ Ogre::Vector3(0,0.75,-4.1), Ogre::Quaternion::IDENTITY);
    level_rb->setGravity(0.0,0.0,0.0);
    level_rb->setKinematicObject(true);
    _shapes.push_back(level_sh);   _bodies.push_back(level_rb);

    ent =  _sceneMgr->createEntity("Plat_big.mesh");
    nodo = _level->createChildSceneNode("l23", Ogre::Vector3(0,0,-18.1));
    nodo->attachObject(ent);
    trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent);
    level_sh = trimeshConverter->createConvex();
    level_rb = new OgreBulletDynamics::RigidBody("l23", _world);
    level_rb->setStaticShape(nodo, level_sh, 0.6, 1.0 , /*5.0,*/ Ogre::Vector3(0,0,-18.1), Ogre::Quaternion::IDENTITY);
    level_rb->setGravity(0.0,0.0,0.0);
    level_rb->setKinematicObject(true);
    _shapes.push_back(level_sh);   _bodies.push_back(level_rb);

    ent =  _sceneMgr->createEntity("Ramp2.mesh");
    nodo = _level->createChildSceneNode("l24", Ogre::Vector3(0,0.75,-25.1));
    nodo->attachObject(ent);
    trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent);
    level_sh = trimeshConverter->createConvex();
    level_rb = new OgreBulletDynamics::RigidBody("l24", _world);
    level_rb->setStaticShape(nodo, level_sh, 0.6, 1.0 , /*5.0,*/ Ogre::Vector3(0,0.75,-25.1), Ogre::Quaternion::IDENTITY);
    level_rb->setGravity(0.0,0.0,0.0);
    level_rb->setKinematicObject(true);
    _shapes.push_back(level_sh);   _bodies.push_back(level_rb);

    ent =  _sceneMgr->createEntity("Plat_big.mesh");
    nodo = _level->createChildSceneNode("l25", Ogre::Vector3(0,0,-38.1));
    nodo->attachObject(ent);
    trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent);
    level_sh = trimeshConverter->createConvex();
    level_rb = new OgreBulletDynamics::RigidBody("l25", _world);
    level_rb->setStaticShape(nodo, level_sh, 0.6, 1.0 , /*5.0,*/ Ogre::Vector3(0,0,-38.1), Ogre::Quaternion::IDENTITY);
    level_rb->setGravity(0.0,0.0,0.0);
    level_rb->setKinematicObject(true);
    _shapes.push_back(level_sh);   _bodies.push_back(level_rb);


    nodo = _level->createChildSceneNode("Cup2_1", Ogre::Vector3(0,3,-8.5));
    ent =  _sceneMgr->createEntity("Cup.mesh");
    nodo->attachObject(ent);
    _cups.push_back(nodo);
    nodo = _level->createChildSceneNode("Cup2_2", Ogre::Vector3(-3,1.5,-18.1));
    ent =  _sceneMgr->createEntity("Cup.mesh");
    nodo->attachObject(ent);
    _cups.push_back(nodo);
    nodo = _level->createChildSceneNode("Cup2_3", Ogre::Vector3(0,1.5,-38.1));
    ent =  _sceneMgr->createEntity("Cup.mesh");
    nodo->attachObject(ent);
    _cups.push_back(nodo);
    
    _meta_n->setPosition(0,1.5,-43.5);
    _stops = 1;
    break;
  }
}
void PlayState::nextLevel()
{
  _currentLevel++;
  removeLevel();
  pushState(NextLevelState::getSingletonPtr());
  resetPos();
  loadLevel();
}

void PlayState::resetLevel()
{
  _lives--;_livesWin->setText(Ogre::StringConverter::toString(_lives));
  if(_lives<1){
    _noLives = true;
  }
  else{
    resetPos();
  }
}
void PlayState::resetPos()
{
  int x, y, z;
  switch(_currentLevel){
  case 1:
    x = 0; y = 10; z = 5;
    break;
  case 2:
    x = 0; y = 10; z = 7;
    break;
  default:
    x = y = z = 0;
  }
  _player_rb->getBulletRigidBody()->translate(btVector3(x-_player_rb->getCenterOfMassPosition().x,y-_player_rb->getCenterOfMassPosition().y,z-_player_rb->getCenterOfMassPosition().z));
  _player_rb->getBulletRigidBody()->clearForces();
  _player_rb->setLinearVelocity(Ogre::Vector3(0,0,0));
  _player_rb->getBulletRigidBody()->setAngularVelocity(btVector3(0,0,0));
  _player_rb->getBulletRigidBody()->setLinearVelocity(btVector3(0,0,0));
  _player_rb->applyForce(Ogre::Vector3::ZERO, _player_rb->getCenterOfMassPosition());
}
void PlayState::removeLevel()
{
  std::deque <OgreBulletDynamics::RigidBody *>::iterator 
        itBody = _bodies.begin()+1;
    while (_bodies.end() != itBody) {   
        delete *itBody;  ++itBody;
    }//Fin while

  destroyAllAttachedMovableObjects(_level);
}

void
PlayState::destroyAllAttachedMovableObjects(Ogre::SceneNode* node)
{
   if(!node) return;

   Ogre::SceneNode::ObjectIterator itObject = node->getAttachedObjectIterator();

   while (itObject.hasMoreElements()){
      node->getCreator()->destroyMovableObject(itObject.getNext());
   }

   Ogre::SceneNode::ChildNodeIterator itChild = node->getChildIterator();

   while (itChild.hasMoreElements()){
      Ogre::SceneNode* pChildNode = static_cast<Ogre::SceneNode*>(itChild.getNext());
      destroyAllAttachedMovableObjects(pChildNode);
   }
}

void PlayState::died()
{
  std::cout << _score << std::endl;
  EndState::getSingletonPtr()->addScore(_score-_time);
  pushState(EndState::getSingletonPtr());
}

void PlayState::updatePlayer()
{
  if(_stopBall){
    _player_rb->getBulletRigidBody()->clearForces();
    _player_rb->setLinearVelocity(Ogre::Vector3(0,0,0));
    _player_rb->getBulletRigidBody()->setAngularVelocity(btVector3(0,0,0));
    _player_rb->getBulletRigidBody()->setLinearVelocity(btVector3(0,0,0));
  }
  double vel = 15.0*_deltaT;
  Ogre::Vector3 lv = Ogre::Vector3::ZERO;
  bool move = false;
  if(_arriba){
    lv+=Ogre::Vector3(0,0,-1*vel);
    move = true;
  }
  if(_abajo){
    lv+=Ogre::Vector3(0,0,1.25*vel);
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
  if(_jumping && _player_rb->getLinearVelocity().y<_deltaT && _player_rb->getLinearVelocity().y>-_deltaT)
    _jumping=false;
  if(_salto && !_jumping){
    _jumping = true;
    _player_rb->enableActiveState();
    _player_rb->getBulletRigidBody()->setLinearVelocity
      (btVector3(_player_rb->getLinearVelocity().x,
		 30*vel, _player_rb->getLinearVelocity().z));
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
      _score += 50;_scoreWin->setText(Ogre::StringConverter::toString(_score));
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
      _score += 50;_scoreWin->setText(Ogre::StringConverter::toString(_score));
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
      _score += 50;
      _scoreWin->setText(Ogre::StringConverter::toString(_score));
      try{
	_simpleEffect->play();
      }
      catch(...){}
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
  if(_score)
    _scoreWin->setText(Ogre::StringConverter::toString(_score));
    //_ovScore->setCaption(Ogre::StringConverter::toString(_score));
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
  _time += _deltaT;
  _timeWin->setText(Ogre::StringConverter::toString(_time));
  
  if(_exitGame)
    return false;

  if(_nextLevel)
    nextLevel();_nextLevel=false;

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
    _salto=true;_jumping = true;
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
  case OIS::KC_LSHIFT:
    if(_stops>0){
      _stops--; _stopBall = true;
    }
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
  case OIS::KC_LSHIFT:
    _stopBall = false;
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

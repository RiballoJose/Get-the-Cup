#include "PlayState.h"
#include "PauseState.h"

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
  
  _exitGame = false;
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
  //_light->setSpecularColour(0.9, 0.9, 0.9);
  //_light->setDiffuseColour(0.8, 0.8, 0.8);
  _light->setSpotlightFalloff(0.0f);
  _light->setCastShadows(true);

  //_camera->setPosition(Ogre::Vector3(17, 16, -4));
  //_camera->lookAt(Ogre::Vector3(-3, 2.7, 0));
  _camera->setNearClipDistance(1);
  _camera->setFOVy(Ogre::Degree(38));
}

void
PlayState::initBullet()
{
  OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = NULL; 
  OgreBulletCollisions::CollisionShape *bodyShape = NULL;
  OgreBulletDynamics::RigidBody *rigidBody = NULL;

  int num = 1;

  Ogre::Entity* entity=NULL;
  Ogre::SceneNode* node=NULL;
  Ogre::Vector3 pos = Ogre::Vector3(3,0.5,8.0);

  /* Creacion de la entidad y del SceneNode */
  Ogre::Plane plane1(Ogre::Vector3::UNIT_Y, 0);
  Ogre::MeshManager::getSingleton().createPlane("p1",
  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane1,
  500, 500, 1, 1, true, 1, 20, 20, Ogre::Vector3::UNIT_Z);
  node = _sceneMgr->createSceneNode("Ground");
  Ogre::Entity* groundEnt = _sceneMgr->createEntity("Base", "p1");
  groundEnt->setCastShadows(false);
  groundEnt->setMaterialName("Ground");
  node->attachObject(groundEnt);
  _sceneMgr->getRootSceneNode()->addChild(node);

  /* Creamos forma de colision para el plano */ 
  OgreBulletCollisions::CollisionShape *Shape;
  Shape = new OgreBulletCollisions::StaticPlaneCollisionShape
    (Ogre::Vector3::UNIT_Y, 0);
  OgreBulletDynamics::RigidBody *rigidBodyPlane = new 
    OgreBulletDynamics::RigidBody("rigidBodyPlane", _world);

  /* Creamos la forma estatica (forma, Restitucion, Friccion) */
  rigidBodyPlane->setStaticShape(Shape, 0.1, 0.8); 
  
  /* Anadimos los objetos Shape y RigidBody */
  _shapes.push_back(Shape);      _bodies.push_back(rigidBodyPlane);

  entity = _sceneMgr->createEntity("cube" + Ogre::StringConverter::toString(num), "Muro_tex.mesh");
  _player = _sceneMgr->getRootSceneNode()->
    createChildSceneNode();
  _player->attachObject(entity);
  
  _player->attachObject(_camera);
  _camera->setPosition(Ogre::Vector3(0, 2.5, 5));
  _camera->lookAt(_player->getPosition());//Ogre::Vector3(0.0, 0.0, 0.0));

  trimeshConverter = new 
    OgreBulletCollisions::StaticMeshToShapeConverter(entity);
  bodyShape = trimeshConverter->createConvex();

  _player_rb = new OgreBulletDynamics::RigidBody("rigidBody" + Ogre::StringConverter::toString(num), _world);

  _player_rb->setShape(_player, bodyShape,
         0.6 /* Restitucion */, 0.6 /* Friccion */,
         5.0 /* Masa */, pos /* Posicion inicial */,
         Ogre::Quaternion::IDENTITY /* Orientacion */);

  _shapes.push_back(bodyShape);   _bodies.push_back(_player_rb);
}

void
PlayState::createScene()
{

  Ogre::SceneNode* nodo = NULL;
   /* Fisica */
  _debugDrawer = new OgreBulletCollisions::DebugDrawer();
  _debugDrawer->setDrawWireframe(true);  
  nodo = _sceneMgr->getRootSceneNode()->createChildSceneNode(
    "debugNode", Ogre::Vector3::ZERO);
  nodo->attachObject(static_cast<Ogre::SimpleRenderable *>(_debugDrawer));

  Ogre::AxisAlignedBox worldBounds = Ogre::AxisAlignedBox (
    Ogre::Vector3 (-10000, -10000, -10000), 
    Ogre::Vector3 (10000,  10000,  10000));
  Ogre::Vector3 gravity = Ogre::Vector3(0, -9.8, 0);

  _world = new OgreBulletDynamics::DynamicsWorld(_sceneMgr,
     worldBounds, gravity);
  _world->setDebugDrawer (_debugDrawer);
  _world->setShowDebugShapes (false);  
}

void PlayState::updatePlayer(){
  //Ogre::Vector3 imp = Ogre::Vector3(0,0,0);
 
  /*if(_arriba){
    imp+=Ogre::Vector3(0.0 , 0.0, 5.0);
  }
  if (_abajo){
    imp+=Ogre::Vector3(0.0,0.0,-5.0);
  }
  if(_izquierda){
    imp+=Ogre::Vector3(5.0,0.0,0.0);
  }
  if(_derecha){
    imp+=Ogre::Vector3(-5.0,0.0,.0);
  }
  if(_salto){
    _player_rb->applyForce(Ogre::Vector3(0.0, 3.0 ,0.0), _player->getPosition());
    //imp+=Ogre::Vector3(0.0,3.0,0.0);
    }*/
  if(_arriba){
  _player_rb->enableActiveState();
  _player_rb->setLinearVelocity(Ogre::Vector3(_camera->getDirection().x,
						_camera->getDirection().y*0,
						_camera->getDirection().z));
  }
  else
    _player_rb->setLinearVelocity(Ogre::Vector3::ZERO);
}

bool
PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{
  _deltaT = evt.timeSinceLastFrame;
  _world->stepSimulation(_deltaT);
  updatePlayer();
  _camera->setAspectRatio
    (Ogre::Real(_viewport->getActualWidth())/
     Ogre::Real(_viewport->getActualHeight()));
  Ogre::Vector3 movement(0, 0, 0);
  Ogre::Vector3 direction = _player->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
  direction.normalise();


  _player_rb->setOrientation(_camera->getOrientation().w, 0*_camera->getOrientation().x,
			     _camera->getOrientation().y, 0*_camera->getOrientation().z);
  return true;
}

bool
PlayState::frameEnded
(const Ogre::FrameEvent& evt)
{
  _deltaT = evt.timeSinceLastFrame;
  _world->stepSimulation(_deltaT);
  //_camera->lookAt(_player->getPosition());//Ogre::Vector3(0.0, 0.0, 0.0));

  if(_exitGame)
    return false;
  
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
  _camera->yaw(Degree(evt.state.X.rel * -0.15f));
  _camera->pitch(Degree(evt.state.Y.rel * -0.15f));
  /*_camera->setPosition(Ogre::Vector3(_camera->getPosition().x-(evt.state.X.rel*0.01f),
				   _camera->getPosition().y,
				   _camera->getPosition().z));*/
}

void
PlayState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  if(OIS::MB_Right)
    _arriba = true;
}

void
PlayState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  if(OIS::MB_Right)
    _arriba = false;
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

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

  _camera->setPosition(Ogre::Vector3(17, 16, -4));
  _camera->lookAt(Ogre::Vector3(-3, 2.7, 0));
  _camera->setNearClipDistance(5);
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

  std::cout << "2" << std::endl;

  entity = _sceneMgr->createEntity("cube" + Ogre::StringConverter::toString(num), "Muro_tex.mesh");
  _player = _sceneMgr->getRootSceneNode()->
    createChildSceneNode();
  _player->attachObject(entity);
  
  _camera->setPosition(0.0, 0.0, 0.0);
  _player->attachObject(_camera);
  _camera->setPosition(Ogre::Vector3(17, 16, -4));
  _camera->lookAt(Ogre::Vector3(-3, 2.7, 0));

  trimeshConverter = new 
    OgreBulletCollisions::StaticMeshToShapeConverter(entity);
  bodyShape = trimeshConverter->createConvex();

  _player_rb = new OgreBulletDynamics::RigidBody("rigidBody" + Ogre::StringConverter::toString(num), _world);

  _player_rb->setShape(_player, bodyShape,
         0.6 /* Restitucion */, 0.6 /* Friccion */,
         5.0 /* Masa */, pos /* Posicion inicial */,
         Ogre::Quaternion::IDENTITY /* Orientacion */);

  _shapes.push_back(bodyShape);   _bodies.push_back(_player_rb);

  

  /* Creamos forma de colision para el plano */ 
 /* OgreBulletCollisions::CollisionShape *Shape;
  Shape = new OgreBulletCollisions::StaticPlaneCollisionShape
    (Ogre::Vector3::UNIT_Y, 0);
  OgreBulletDynamics::RigidBody *rigidBodyPlane = new 
    OgreBulletDynamics::RigidBody("rigidBodyPlane", _world);

  /* Creamos la forma estatica (forma, Restitucion, Friccion) */
  //rigidBodyPlane->setStaticShape(Shape, 0.1, 0.8); 
  
  /* Anadimos los objetos Shape y RigidBody */
  //_shapes.push_back(Shape);      _bodies.push_back(rigidBodyPlane);
  /*_broadphase = new btDbvtBroadphase();
  std::cout << "1" << std::endl;
  _collisionConfiguration = new btDefaultCollisionConfiguration();
  _dispatcher = new btCollisionDispatcher(_collisionConfiguration);
  _solver = new btSequentialImpulseConstraintSolver;
  _dynamicsWorld = new btDiscreteDynamicsWorld(_dispatcher,_broadphase,
                 _solver,_collisionConfiguration);

  // Definicion de las propiedades del mundo -------------------
  _dynamicsWorld->setGravity(btVector3(0,-10,0));*/

  // Creacion de las formas de colision ------------------------
  /*btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
  btCollisionShape* fallShape = new btSphereShape(1);
  // Definicion de los cuerpos rigidos en la escena ------------
  btDefaultMotionState* groundMotionState = new btDefaultMotionState
    (btTransform(btQuaternion(0,0,0,1), btVector3(0,-1,0)));
  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI
    (0,groundMotionState,groundShape,btVector3(0,0,0));
  btRigidBody* gRigidBody = new btRigidBody(groundRigidBodyCI);
  _dynamicsWorld->addRigidBody(gRigidBody);
  btDefaultMotionState* fallMotionState = new btDefaultMotionState
    (btTransform(btQuaternion(0,0,0,1), btVector3(0,50,0)));
  btScalar mass = 1;
  btVector3 fallInertia(0,0,0);
  fallShape->calculateLocalInertia(mass,fallInertia);
  btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI
    (mass,fallMotionState,fallShape,fallInertia);
  btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
  _dynamicsWorld->addRigidBody(fallRigidBody);
  // Bucle principal de la simulacion --------------------------
  for (int i=0 ; i<300 ; i++) {
    _dynamicsWorld->stepSimulation(1/60.f,10);
    btTransform trans;
    fallRigidBody->getMotionState()->getWorldTransform(trans);
    std::cout << "Altura: " << trans.getOrigin().getY() << std::endl;
  }
  //Finalizacion (limpieza) -----------------------------------
  _dynamicsWorld->removeRigidBody(fallRigidBody);
  delete fallRigidBody->getMotionState(); delete fallRigidBody;
  _dynamicsWorld->removeRigidBody(gRigidBody);
  delete gRigidBody->getMotionState(); delete gRigidBody;
  delete fallShape; delete groundShape;*/
}

void
PlayState::createScene()
{

  Ogre::SceneNode* nodo = NULL;

  nodo = _sceneMgr->getRootSceneNode()->createChildSceneNode
    ("Muro", Ogre::Vector3(0.0,5.0,8.0));
  Ogre::Entity* ent2 = _sceneMgr->createEntity("Muro_tex.mesh");
  ent2->setCastShadows(true);
  //nodo->attachObject(_camera);
  nodo->attachObject(ent2);

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
  _world->setShowDebugShapes (false);  // Muestra los collision shapes

  // Creacion de la entidad y del SceneNode ------------------------
  /*Ogre::Plane plane1(Vector3(0,1,0), 0);    // Normal y distancia
  MeshManager::getSingleton().createPlane("p1",
  ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane1,
  200, 200, 1, 1, true, 1, 20, 20, Vector3::UNIT_Z);
  Ogre::SceneNode* node = _sceneMgr->createSceneNode("ground");
  Ogre::Entity* groundEnt = _sceneMgr->createEntity("planeEnt", "p1");
  groundEnt->setMaterialName("Ground");
  node->attachObject(groundEnt);
  _sceneMgr->getRootSceneNode()->addChild(node);
  // Creamos forma de colision para el plano ----------------------- 
  OgreBulletCollisions::CollisionShape *Shape;
  Shape = new OgreBulletCollisions::StaticPlaneCollisionShape
    (Ogre::Vector3(0,1,0), 0);   // Vector normal y distancia
  OgreBulletDynamics::RigidBody *rigidBodyPlane = new 
    OgreBulletDynamics::RigidBody("rigidBodyPlane", _world);
  // Creamos la forma estatica (forma, Restitucion, Friccion) ------
  rigidBodyPlane->setStaticShape(Shape, 0.1, 0.8); 
  
  // Anadimos los objetos Shape y RigidBody ------------------------
  //_shapes.push_back(Shape);      _bodies.push_back(rigidBodyPlane);*/
}

void PlayState::updatePlayer(){
  Ogre::Vector3 imp = Ogre::Vector3(0,50,0);
  if(_arriba){
    _player_rb->enableActiveState();
    _player_rb->setLinearVelocity(-5,0,0);
  }else{
    _player_rb->setLinearVelocity(0,0,0);
  }
  if(_abajo){
    _player_rb->enableActiveState();
    _player_rb->setLinearVelocity(5,0,0);
  }else{
    _player_rb->setLinearVelocity(0,0,0);
  }
  if(_izquierda){
    _player_rb->enableActiveState();
    _player_rb->setLinearVelocity(0,0,5);
  }else{
    _player_rb->setLinearVelocity(0,0,0);
  }
  if(_derecha){
    _player_rb->enableActiveState();
    _player_rb->setLinearVelocity(0,0,-5);
  }else{
    _player_rb->setLinearVelocity(0,0,0);
  }
  if(_salto){
    _player_rb->enableActiveState();
    _player_rb->applyForce(imp, _player->getPosition());
  }
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


  return true;
}

bool
PlayState::frameEnded
(const Ogre::FrameEvent& evt)
{
  _deltaT = evt.timeSinceLastFrame;
  _world->stepSimulation(_deltaT);
  if (_exitGame)
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
  case OIS::KC_DOWN:
    _abajo = true;
    break;
  case OIS::KC_RIGHT:
    _derecha = true;
    break;
  case OIS::KC_LEFT:
    _izquierda = true;
    break;
  case OIS::KC_UP:
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
   case OIS::KC_UP:
    _arriba = false;
    break;
   case OIS::KC_DOWN:
    _abajo = false;
    break;
   case OIS::KC_RIGHT:
    _derecha = false;
    break;
   case OIS::KC_LEFT:
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

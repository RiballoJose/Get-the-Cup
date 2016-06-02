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
  
  // Nuevo background colour.
  //_viewport->setBackgroundColour(Ogre::ColourValue(1.0, 1.0, 1.0));
  createScene();
  initLights();
  initBullet();
  _exitGame = false;
}

void
PlayState::exit ()
{
  delete _dynamicsWorld; delete _solver;
  delete _collisionConfiguration;
  delete _dispatcher; delete _broadphase;
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
  // Se restaura el background colour.
  //_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
}

void
PlayState::initLights()
{
  _light = _sceneMgr->createLight("Light");
  _sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
  _sceneMgr->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5) );
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.9, 0.9, 0.9));
  _sceneMgr->setShadowTextureCount(2);
  _sceneMgr->setShadowTextureSize(512);
  
  _light->setType(Ogre::Light::LT_SPOTLIGHT);
  _light->setDirection(Ogre::Vector3(0,-1,0));
  _light->setSpotlightInnerAngle(Ogre::Degree(25.0f));
  _light->setSpotlightOuterAngle(Ogre::Degree(200.0f));
  _light->setPosition(0, 100, 0);
  _light->setSpecularColour(0.8, 0.8, 0.8);
  _light->setDiffuseColour(0.8, 0.8, 0.8);
  _light->setSpotlightFalloff(5.0f);
  _light->setCastShadows(true);
}

void
PlayState::initBullet()
{
  _broadphase = new btDbvtBroadphase();
  _collisionConfiguration = new btDefaultCollisionConfiguration();
  _dispatcher = new btCollisionDispatcher(_collisionConfiguration);
  _solver = new btSequentialImpulseConstraintSolver;
  _dynamicsWorld = new btDiscreteDynamicsWorld(_dispatcher,_broadphase,
					       _solver,_collisionConfiguration);

  // Definicion de las propiedades del mundo -------------------
  _dynamicsWorld->setGravity(btVector3(0,-10,0));

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

  // Finalizacion (limpieza) -----------------------------------
  _dynamicsWorld->removeRigidBody(fallRigidBody);
  delete fallRigidBody->getMotionState(); delete fallRigidBody;
  _dynamicsWorld->removeRigidBody(gRigidBody);
  delete gRigidBody->getMotionState(); delete gRigidBody;
  delete fallShape; delete groundShape;*/
}

void
PlayState::createScene()
{
  _camera->setPosition(Ogre::Vector3(0, 42, 7));
  _camera->lookAt(Ogre::Vector3(0, -50, 0));
  Ogre::SceneNode* nodo = _sceneMgr->getRootSceneNode()->createChildSceneNode
    ("Escenario", Ogre::Vector3(-0.5,0.0,3.0));
  Ogre::Entity* ent = _sceneMgr->createEntity("Base.mesh");
  nodo->attachObject(ent);
}

bool
PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{
  return true;
}

bool
PlayState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
PlayState::keyPressed
(const OIS::KeyEvent &e)
{
  if (e.key == OIS::KC_P or e.key == OIS::KC_ESCAPE) {
    pushState(PauseState::getSingletonPtr());
  }
}

void
PlayState::keyReleased
(const OIS::KeyEvent &e)
{
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

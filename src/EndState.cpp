#include "EndState.h"
#include "MenuState.h"
#include "RecordManager.h"

template<> EndState* Ogre::Singleton<EndState>::msSingleton = 0;
void EndState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->getViewport(0);
  _endGame = false;
}

void EndState::exit ()
{
}

void EndState::pause ()
{
}

void EndState::resume ()
{
}

bool EndState::frameStarted(const Ogre::FrameEvent& evt)
{
  return true;
}

bool EndState::frameEnded(const Ogre::FrameEvent& evt)
{  
  if(_endGame)
    {
      popState();
    }
  
  return true;
}

void EndState::keyPressed(const OIS::KeyEvent &e)
{  
}

void EndState::keyReleased(const OIS::KeyEvent &e)
{
}

EndState* EndState::getSingletonPtr (){
  return msSingleton;
}
EndState& EndState::getSingleton (){ 
  assert(msSingleton);
  return *msSingleton;
}

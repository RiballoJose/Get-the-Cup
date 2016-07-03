#include "EndState.h"
#include "MenuState.h"
#include "RecordManager.h"

template<> EndState* Ogre::Singleton<EndState>::msSingleton = 0;

void
EndState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->getViewport(0);
  _pSoundFXManager = SoundFXManager::getSingletonPtr();
  _simpleEffect = _pSoundFXManager->load("gameover.ogg");
  _simpleEffect->play();
  _exitGame = _save = false;

    //CEGUI
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().show();
  _sheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Menu");

  //Config Window
  CEGUI::Window* configWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("end.layout");
  CEGUI::Window* mark = configWin->getChild("MarkValue");
  mark->setText(Ogre::StringConverter::toString(_score));
  _nick = configWin->getChild("Nick");
  _nick->subscribeEvent(CEGUI::Window::EventMouseButtonDown,
			CEGUI::Event::Subscriber(&EndState::clear, this));
  CEGUI::Window* menuButton = configWin->getChild("MenuButton");
  menuButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&EndState::save, this));
  CEGUI::Window* exitButton = configWin->getChild("ExitButton");
  exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&EndState::quit, this));

  //Attaching buttons
  _sheet->addChild(configWin);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_sheet);
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

bool EndState::clear(const CEGUI::EventArgs &e)
{
  _nick->setText("");
  return true;
}

bool EndState::save(const CEGUI::EventArgs &e)
{
  //_help->hide();
  //CEGUI::string name = nick->getText();
  _save=true;
  return true;
}

bool EndState::quit(const CEGUI::EventArgs &e)
{
  _exitGame = true;
  return true;
}
void EndState::addScore(int score){
  _score = score;
}

bool EndState::frameStarted(const Ogre::FrameEvent& evt)
{
  return true;
}

bool EndState::frameEnded(const Ogre::FrameEvent& evt)
{
  if(_exitGame)
    return false;
  if(_save){
    popState();
    changeState(MenuState::getSingletonPtr());
  }
   
  return true;
}

void EndState::keyPressed(const OIS::KeyEvent &e)
{
  char aux;

  switch(e.key){
  case OIS::KC_RIGHT:
    break;
  case OIS::KC_LEFT:
    break;
  case OIS::KC_DOWN:
    break;
  case OIS::KC_UP:
    break;
  case OIS::KC_SPACE:
    _exitGame = true;
    break;
  case OIS::KC_RETURN:
    _exitGame = true;
  default:
    break;
  }
}

void EndState::keyReleased(const OIS::KeyEvent &e)
{
}

void
EndState::mouseMoved
(const OIS::MouseEvent &evt)
{
}

void
EndState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
EndState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

EndState* EndState::getSingletonPtr (){
  return msSingleton;
}
EndState& EndState::getSingleton (){ 
  assert(msSingleton);
  return *msSingleton;
}

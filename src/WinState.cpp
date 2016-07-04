#include "WinState.h"
#include "MenuState.h"
#include "RecordManager.h"

template<> WinState* Ogre::Singleton<WinState>::msSingleton = 0;

void
WinState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->getViewport(0);
  _pSoundFXManager = SoundFXManager::getSingletonPtr();
  _simpleEffect = _pSoundFXManager->load("end.ogg");
  _simpleEffect->play();
  _exitGame = _save = false;

  _rec = new Record();
    //CEGUI
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().show();
  _sheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Menu");

  //Config Window
  CEGUI::Window* configWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("win.layout");
  CEGUI::Window* mark = configWin->getChild("MarkValue");
  mark->setText(Ogre::StringConverter::toString(_score));
  _nick = configWin->getChild("Nick");
  _nick->subscribeEvent(CEGUI::Window::EventMouseButtonDown,
			CEGUI::Event::Subscriber(&WinState::clear, this));
  CEGUI::Window* menuButton = configWin->getChild("MenuButton");
  menuButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&WinState::save, this));
  CEGUI::Window* exitButton = configWin->getChild("ExitButton");
  exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&WinState::quit, this));

  //Attaching buttons
  _sheet->addChild(configWin);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_sheet);
}

void WinState::exit ()
{
}

void WinState::pause ()
{
}

void WinState::resume ()
{
}

bool WinState::clear(const CEGUI::EventArgs &e)
{
  _nick->setText("");
  return true;
}

bool WinState::save(const CEGUI::EventArgs &e)
{
  //_help->hide();
  //CEGUI::string name = nick->getText();
  RecordManager::getSingletonPtr()->loadRecords();
  if(_score<0)
    _rec->setPunt(0);
  else
    _rec->setPunt(_score);
  _rec->setPlayer(_nick->getText().c_str());
  RecordManager::getSingletonPtr()->addRecord(*_rec);
  RecordManager::getSingletonPtr()->saveRecords();
  _save=true;
  return true;
}

bool WinState::quit(const CEGUI::EventArgs &e)
{
  _exitGame = true;
  return true;
}
void WinState::addScore(int score){
  _score = score;
}

bool WinState::frameStarted(const Ogre::FrameEvent& evt)
{
  return true;
}

bool WinState::frameEnded(const Ogre::FrameEvent& evt)
{
  if(_exitGame)
    return false;
  if(_save){
    popState();
    changeState(MenuState::getSingletonPtr());
  }
   
  return true;
}

void WinState::keyPressed(const OIS::KeyEvent &e)
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

void WinState::keyReleased(const OIS::KeyEvent &e)
{
}

void
WinState::mouseMoved
(const OIS::MouseEvent &evt)
{
}

void
WinState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
WinState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

WinState* WinState::getSingletonPtr (){
  return msSingleton;
}
WinState& WinState::getSingleton (){ 
  assert(msSingleton);
  return *msSingleton;
}

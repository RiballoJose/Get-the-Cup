#include "MenuState.h"
#include "NextLevelState.h"

template<> NextLevelState* Ogre::Singleton<NextLevelState>::msSingleton = 0;

void
NextLevelState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->getViewport(0);
  createGUI();
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().show();
  _exitGame = false;
}

void
NextLevelState::exit ()
{
  _sheet->hide();
}

void
NextLevelState::pause ()
{
}

void
NextLevelState::resume ()
{
}
void NextLevelState::createGUI()
{
  //CEGUI
  _sheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Menu");

  //Config Window
  CEGUI::Window* configWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("next.layout");
  CEGUI::Window* playButton = configWin->getChild("PlayButton");
  playButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&NextLevelState::play, this));
  /*CEGUI::Window* saveButton = configWin->getChild("SaveButton");
  saveButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&NextLevelState::save, this));
  CEGUI::Window* loadButton = configWin->getChild("LoadButton");
  loadButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&NextLevelState::load, this));*/
  CEGUI::Window* helpButton = configWin->getChild("HelpButton");
  helpButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&NextLevelState::help, this));
  CEGUI::Window* menuButton = configWin->getChild("MenuButton");
  menuButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&NextLevelState::menu, this));
  CEGUI::Window* exitButton = configWin->getChild("ExitButton");
  exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&NextLevelState::quit, this));

  //Attaching buttons
  _sheet->addChild(configWin);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_sheet);
}
bool NextLevelState::play(const CEGUI::EventArgs &e)
{
  popState();
  return true;
}
/*bool NextLevelState::save(const CEGUI::EventArgs &e)
{
  return true;
}
bool NextLevelState::load(const CEGUI::EventArgs &e)
{
  return true;
  }*/
bool NextLevelState::help(const CEGUI::EventArgs &e)
{
  CEGUI::Window* configWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("help.layout");
  _help = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Menu");
  CEGUI::Window* helpText = configWin->getChild("HelpText");
  helpText->setText("El objetivo de Get the Cup es llevar a tu\nequipo hasta lo mas alto, y para ello\ndeberas guiar su escudo a traves de niveles y plataformas\nEn ellos encontraras obstaculos y rivales\nque querran impedir tus exitos. Tendras\nque evitarlos!Puedes controlar a tu\npersonaje con las siguientes teclas:");
  CEGUI::Window* exitButton = configWin->getChild("ExitButton");
  exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&NextLevelState::back, this));
  _help->addChild(configWin);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_help);
  return true;
}
bool NextLevelState::menu(const CEGUI::EventArgs &e)
{
  popState();
  changeState(MenuState::getSingletonPtr());
  return true;
}

bool NextLevelState::back(const CEGUI::EventArgs &e)
{
  //_help->hide();
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_sheet);
  return true;
}

bool NextLevelState::quit(const CEGUI::EventArgs &e)
{
  _exitGame = true;
  return true;
}

bool
NextLevelState::frameStarted
(const Ogre::FrameEvent& evt)
{
  return true;
}

bool
NextLevelState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
NextLevelState::keyPressed
(const OIS::KeyEvent &e) {
  // Tecla p --> Estado anterior.
  if (e.key == OIS::KC_P or e.key == OIS::KC_ESCAPE) {
    popState();
  }
}

void
NextLevelState::keyReleased
(const OIS::KeyEvent &e)
{
}

void
NextLevelState::mouseMoved
(const OIS::MouseEvent &evt)
{
}

void
NextLevelState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
NextLevelState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

NextLevelState*
NextLevelState::getSingletonPtr ()
{
return msSingleton;
}

NextLevelState&
NextLevelState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

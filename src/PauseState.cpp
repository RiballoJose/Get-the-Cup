#include "MenuState.h"
#include "PauseState.h"

template<> PauseState* Ogre::Singleton<PauseState>::msSingleton = 0;

void
PauseState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->getViewport(0);
  // Nuevo background colour.
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 1.0, 0.0));
  createGUI();
  
  _exitGame = false;
}

void
PauseState::exit ()
{
  _sheet->hide();
}

void
PauseState::pause ()
{
}

void
PauseState::resume ()
{
}
void PauseState::createGUI()
{
  //CEGUI
  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
  CEGUI::Font::setDefaultResourceGroup("Fonts");
  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");

  CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
  CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-12");
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

  //Sheet
  _sheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Menu");

  //Config Window
  CEGUI::Window* configWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("pause.layout");
  CEGUI::Window* playButton = configWin->getChild("PlayButton");
  playButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&PauseState::play, this));
  CEGUI::Window* saveButton = configWin->getChild("SaveButton");
  saveButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&PauseState::save, this));
  CEGUI::Window* loadButton = configWin->getChild("LoadButton");
  loadButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&PauseState::load, this));
  CEGUI::Window* cfgButton = configWin->getChild("CfgButton");
  cfgButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&PauseState::cfg, this));
  CEGUI::Window* menuButton = configWin->getChild("MenuButton");
  menuButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&PauseState::menu, this));
  CEGUI::Window* exitButton = configWin->getChild("ExitButton");
  exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&PauseState::quit, this));

  //Attaching buttons
  _sheet->addChild(configWin);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_sheet);
}
bool PauseState::play(const CEGUI::EventArgs &e)
{
  popState();
  return true;
}
bool PauseState::save(const CEGUI::EventArgs &e)
{
  return true;
}
bool PauseState::load(const CEGUI::EventArgs &e)
{
  return true;
}
bool PauseState::cfg(const CEGUI::EventArgs &e)
{
  return true;
}
bool PauseState::menu(const CEGUI::EventArgs &e)
{
  popState();
  changeState(MenuState::getSingletonPtr());
  return true;
}
bool PauseState::quit(const CEGUI::EventArgs &e)
{
  _exitGame = true;
  return true;
}

bool
PauseState::frameStarted
(const Ogre::FrameEvent& evt)
{
  return true;
}

bool
PauseState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
PauseState::keyPressed
(const OIS::KeyEvent &e) {
  // Tecla p --> Estado anterior.
  if (e.key == OIS::KC_P or e.key == OIS::KC_ESCAPE) {
    popState();
  }
}

void
PauseState::keyReleased
(const OIS::KeyEvent &e)
{
}

void
PauseState::mouseMoved
(const OIS::MouseEvent &evt)
{
}

void
PauseState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
PauseState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

PauseState*
PauseState::getSingletonPtr ()
{
return msSingleton;
}

PauseState&
PauseState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

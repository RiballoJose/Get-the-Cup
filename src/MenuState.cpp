#include "MenuState.h"
#include "PlayState.h"

template<> MenuState* Ogre::Singleton<MenuState>::msSingleton = 0;

void
MenuState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  _sceneMgr = _root->getSceneManager("SceneManager");
  //_sceneMgr->addRenderQueueListener(GameManager::getSingletonPtr()->getOverlaySystem());
  //_overlayManager = Ogre::OverlayManager::getSingletonPtr();
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _viewport->setBackgroundColour(Ogre::ColourValue(1.0, 0.0, 1.0));
  createGUI();
  
  _exitGame = false;
}

void
MenuState::exit()
{
  //_renderer->destroySystem();
  _sheet->hide();
  _sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();
}

void
MenuState::pause ()
{
}

void
MenuState::resume ()
{
}

void MenuState::createGUI()
{
  //CEGUI
  //_renderer = &CEGUI::OgreRenderer::bootstrapSystem();
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
  CEGUI::Window* configWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("menu.layout");
  CEGUI::Window* playButton = configWin->getChild("PlayButton");
  playButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&MenuState::play, this));
  CEGUI::Window* loadButton = configWin->getChild("LoadButton");
  loadButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&MenuState::load, this));
  CEGUI::Window* recordsButton = configWin->getChild("RecordsButton");
  recordsButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&MenuState::records, this));
  CEGUI::Window* cfgButton = configWin->getChild("CfgButton");
  cfgButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&MenuState::cfg, this));
  CEGUI::Window* exitButton = configWin->getChild("ExitButton");
  exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&MenuState::quit, this));

  //Attaching buttons
  _sheet->addChild(configWin);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_sheet);
}

bool MenuState::play(const CEGUI::EventArgs &e)
{
  changeState(PlayState::getSingletonPtr());
  return true;
}
bool MenuState::load(const CEGUI::EventArgs &e)
{
  return true;
}
bool MenuState::records(const CEGUI::EventArgs &e)
{
  return true;
}
bool MenuState::cfg(const CEGUI::EventArgs &e)
{
  return true;
}
bool MenuState::quit(const CEGUI::EventArgs &e)
{
  _exitGame = true;
  return true;
}

bool
MenuState::frameStarted
(const Ogre::FrameEvent& evt) 
{
  return true;
}

bool
MenuState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
MenuState::keyPressed
(const OIS::KeyEvent &e)
{
}

void
MenuState::keyReleased
(const OIS::KeyEvent &e )
{
}

void
MenuState::mouseMoved
(const OIS::MouseEvent &evt)
{
}

void
MenuState::mousePressed
(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
}

void
MenuState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}
MenuState*
MenuState::getSingletonPtr ()
{
return msSingleton;
}

MenuState&
MenuState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

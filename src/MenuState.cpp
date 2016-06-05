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
  createGUI();
  
  _exitGame = false;
}

void
MenuState::exit()
{
  //_renderer->destroySystem();
  _menu->hide();
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
  _menu = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Menu");

  //Config Window
  CEGUI::Window* configWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("menu.layout");
  CEGUI::Window* playButton = configWin->getChild("PlayButton");
  playButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&MenuState::play, this));
  
  CEGUI::Window* loadButton = configWin->getChild("LoadButton");
  loadButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&MenuState::load, this));

  CEGUI::Window* helpButton = configWin->getChild("HelpButton");
  helpButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&MenuState::help, this));
  
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
  _menu->addChild(configWin);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_menu);
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
bool MenuState::help(const CEGUI::EventArgs &e)
{
  CEGUI::Window* configWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("help.layout");
  _cfg = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Menu");
  CEGUI::Window* helpText = configWin->getChild("HelpText");
  helpText->setText("El objetivo de Get the Cup es llevar a tu\nequipo hasta lo mas alto, y para ello\ndeberas guiar su escudo a traves de niveles y plataformas.\nEn ellas encontraras obstaculos y rivales\nque querran impedir tus exitos. Tendras\nque evitarlos!Puedes controlar a tu\npersonaje con las siguientes instrucciones:");
  CEGUI::Window* exitButton = configWin->getChild("ExitButton");
  exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&MenuState::back, this));
  _cfg->addChild(configWin);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_cfg);
  return true;
}
bool MenuState::records(const CEGUI::EventArgs &e)
{
  return true;
}
bool MenuState::cfg(const CEGUI::EventArgs &e)
{
  CEGUI::Window* configWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("cfg.layout");
  _cfg = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Menu");
  CEGUI::Window* exitButton = configWin->getChild("ExitButton");
  exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&MenuState::back, this));
  CEGUI::Window* applyButton = configWin->getChild("ApplyButton");
  applyButton->subscribeEvent(CEGUI::PushButton::EventClicked,
  CEGUI::Event::Subscriber(&MenuState::apply, this));
  _cfg->addChild(configWin);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_cfg);
  return true;
}
bool MenuState::quit(const CEGUI::EventArgs &e)
{
  _exitGame = true;
  return true;
}

bool MenuState::back(const CEGUI::EventArgs &e)
{
  //_cfg->hide();
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_menu);
  return true;
}

bool MenuState::apply(const CEGUI::EventArgs &e)
{
  std::ifstream file("ogre.cfg");
  std::string str;
  if(file.is_open()){
    for(int i = 0; i < 9; i++)
      getline(file,str);
    
    std::cout << str << std::endl;
  }
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

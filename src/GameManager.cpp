#include <Ogre.h>

#include "GameManager.h"
#include "GameState.h"

template<> GameManager* Ogre::Singleton<GameManager>::msSingleton = 0;

GameManager::GameManager ()
{
  _root = 0;
}

GameManager::~GameManager ()
{
  while (!_states.empty()) {
    _states.top()->exit();
    _states.pop();
  }
  
  if (_root)
    delete _root;
}

void
GameManager::start
(GameState* state)
{
  // Creación del objeto Ogre::Root.
  _root = new Ogre::Root();
  //Iniciamos SDL
  initSDL();
  //Iniciamos Overlays
  _overlaySystem = new Ogre::OverlaySystem();
  loadResources();

  if (!configure())
    return;    
  	
  _inputMgr = new InputManager;
  _inputMgr->initialise(_renderWindow);

  // Registro como key y mouse listener...
  _inputMgr->addKeyListener(this, "GameManager");
  _inputMgr->addMouseListener(this, "GameManager");

  // El GameManager es un FrameListener.
  _root->addFrameListener(this);

  //Iniciamos CEGUI
  _renderer = &CEGUI::OgreRenderer::bootstrapSystem();
  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
  CEGUI::Font::setDefaultResourceGroup("Fonts");
  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");

  CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
  CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-12");
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
  _pTrackManager=new TrackManager();
  _pSoundFXManager=new SoundFXManager();
  
  
  // Transición al estado inicial.
  changeState(state);
  
  // Bucle de rendering.
  _root->startRendering();
}

void
GameManager::changeState
(GameState* state)
{
  // Limpieza del estado actual.
  if (!_states.empty()) {
    // exit() sobre el último estado.
    _states.top()->exit();
    // Elimina el último estado.
    _states.pop();
  }

  // Transición al nuevo estado.
  _states.push(state);
  // enter() sobre el nuevo estado.
  _states.top()->enter();
}

void
GameManager::pushState
(GameState* state)
{
  // Pausa del estado actual.
  if (!_states.empty())
    _states.top()->pause();
  
  // Transición al nuevo estado.
  _states.push(state);
  // enter() sobre el nuevo estado.
  _states.top()->enter();
}

void
GameManager::popState ()
{
  // Limpieza del estado actual.
  if (!_states.empty()) {
    _states.top()->exit();
    _states.pop();
  }
  
  // Vuelta al estado anterior.
  if (!_states.empty())
    _states.top()->resume();
}

void
GameManager::loadResources ()
{
  Ogre::ConfigFile cf;
  cf.load("resources.cfg");
  
  Ogre::ConfigFile::SectionIterator sI = cf.getSectionIterator();
  Ogre::String sectionstr, typestr, datastr;
  while (sI.hasMoreElements()) {
    sectionstr = sI.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = sI.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for (i = settings->begin(); i != settings->end(); ++i) {
      typestr = i->first;    datastr = i->second;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation
            (datastr, typestr, sectionstr);	
    }
  }
}

bool
GameManager::configure ()
{
  if (!_root->restoreConfig()) {
    if (!_root->showConfigDialog()) {
      return false;
    }
  }
  
  _renderWindow = _root->initialise(true, "Get the Cup");
  
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
  
  return true;
}

GameManager*
GameManager::getSingletonPtr ()
{
  return msSingleton;
}

GameManager&
GameManager::getSingleton ()
{  
  assert(msSingleton);
  return *msSingleton;
}

// Las siguientes funciones miembro delegan
// el evento en el estado actual.
bool
GameManager::frameStarted
(const Ogre::FrameEvent& evt)
{
  _inputMgr->capture();
  return _states.top()->frameStarted(evt);
}

bool
GameManager::frameEnded
(const Ogre::FrameEvent& evt)
{
  return _states.top()->frameEnded(evt);
}

bool
GameManager::keyPressed 
(const OIS::KeyEvent &e)
{
  _states.top()->keyPressed(e);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
  CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);
  
  return true;
}

bool
GameManager::keyReleased
(const OIS::KeyEvent &e)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(e.key));
  _states.top()->keyReleased(e);
  return true;
}

bool
GameManager::mouseMoved 
(const OIS::MouseEvent &e)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);  
  _states.top()->mouseMoved(e);
  return true;
}

bool
GameManager::mousePressed 
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
  _states.top()->mousePressed(e, id);
  return true;
}

bool
GameManager::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
  _states.top()->mouseReleased(e, id);
  return true;
}

CEGUI::MouseButton GameManager::convertMouseButton(OIS::MouseButtonID id)
{
  CEGUI::MouseButton ceguiId;
  switch(id)
    {
    case OIS::MB_Left:
      ceguiId = CEGUI::LeftButton;
      break;
    case OIS::MB_Right:
      ceguiId = CEGUI::RightButton;
      break;
    case OIS::MB_Middle:
      ceguiId = CEGUI::MiddleButton;
      break;
    default:
      ceguiId = CEGUI::LeftButton;
    }
  return ceguiId;
}
bool GameManager::initSDL () {
    // Inicializando SDL...
  std::cout<<"Inicianzo SDL"<<std::endl;
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        return false;
    // Llamar a  SDL_Quit al terminar.
    atexit(SDL_Quit);

    // Inicializando SDL mixer...
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,MIX_DEFAULT_CHANNELS, 4096) < 0)
      return false;

    // Llamar a Mix_CloseAudio al terminar.
    atexit(Mix_CloseAudio);

    return true;
}

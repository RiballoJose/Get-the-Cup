#ifndef EndState_H
#define EndState_H

#include <Ogre.h>
#include <OIS/OIS.h>

#include "Record.h"
#include "GameState.h"

class EndState : public Ogre::Singleton<EndState>, public GameState{
 public:
  EndState() {};

  void enter ();
  void exit ();
  void pause ();
  void resume ();

  void keyPressed (const OIS::KeyEvent &e);
  void keyReleased (const OIS::KeyEvent &e);

  void mouseMoved(const OIS::MouseEvent &evt);
  void mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
  void mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

  bool clear(const CEGUI::EventArgs &e);
  bool save(const CEGUI::EventArgs &e);
  bool quit(const CEGUI::EventArgs &e);
  
  bool frameStarted (const Ogre::FrameEvent& evt);
  bool frameEnded (const Ogre::FrameEvent& evt);

  static EndState& getSingleton ();
  static EndState* getSingletonPtr ();

 protected:
  Ogre::Root* _root;
  Ogre::SceneManager* _sceneMgr;
  Ogre::Viewport* _viewport;
  Ogre::Camera* _camera;

  CEGUI::Window* _sheet;
  CEGUI::Window* _nick;
  
  SoundFXPtr _simpleEffect;
  SoundFXManager* _pSoundFXManager;
        
 private:
  bool _exitGame, _save;
};

#endif

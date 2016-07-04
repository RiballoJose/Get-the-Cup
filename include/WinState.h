#ifndef WinState_H
#define WinState_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include <string>

#include "Record.h"
#include "GameState.h"

class WinState : public Ogre::Singleton<WinState>, public GameState{
 public:
  WinState() {};

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
  void addScore(int score);
  
  bool frameStarted (const Ogre::FrameEvent& evt);
  bool frameEnded (const Ogre::FrameEvent& evt);

  static WinState& getSingleton ();
  static WinState* getSingletonPtr ();

 protected:
  Ogre::Root* _root;
  Ogre::SceneManager* _sceneMgr;
  Ogre::Viewport* _viewport;
  Ogre::Camera* _camera;

  CEGUI::Window* _sheet;
  CEGUI::Window* _nick;
  Record *_rec;
  std::string _scoreStr, _nickname;
  SoundFXPtr _simpleEffect;
  SoundFXManager* _pSoundFXManager;
        
 private:
  int _score;
  bool _exitGame, _save;
};

#endif

/* **********************************************************
** Importador NoEscape 1.0
** Curso de Experto en Desarrollo de Videojuegos 
** Escuela Superior de Informatica - Univ. Castilla-La Mancha
** Carlos Gonzalez Morcillo - David Vallejo Fernandez
************************************************************/

#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <OgreVector3.h>

using namespace std;

class Node
{
 public:
  Node ();
  Node (const int& id, const string& type, const Ogre::Vector3& position);
  ~Node ();

  int getID () const { return _id; }
  string getType () const { return _type; }
  Ogre::Vector3 getPosition () const { return _position; }
    
 private:
  int _id;
  string _type;
  Ogre::Vector3 _position;
};

#endif

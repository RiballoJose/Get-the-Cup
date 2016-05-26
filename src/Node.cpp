/* **********************************************************
** Importador NoEscape 1.0
** Curso de Experto en Desarrollo de Videojuegos 
** Escuela Superior de Informatica - Univ. Castilla-La Mancha
** Carlos Gonzalez Morcillo - David Vallejo Fernandez
************************************************************/

#include <Node.h>

Node::Node ()
{
}

Node::Node
(const int& id, const string& type, const Ogre::Vector3& position)
{
  _id = id;
  _type = type;
  _position = position;
}

Node::~Node ()
{
}


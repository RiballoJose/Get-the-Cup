/* **********************************************************
** Importador NoEscape 1.0
** Curso de Experto en Desarrollo de Videojuegos 
** Escuela Superior de Informatica - Univ. Castilla-La Mancha
** Carlos Gonzalez Morcillo - David Vallejo Fernandez
************************************************************/

#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <GraphVertex.h>
#include <GraphEdge.h>

class Graph
{
 public:
  Graph ();
  ~Graph ();
  
  void addVertex (GraphVertex* pVertex);
  void addEdge (GraphVertex* pOrigin, GraphVertex* pDestination,
		bool undirected = true);

 private:
  std::vector<GraphVertex*> _vertexes;
  std::vector<GraphEdge*> _edges;
};

#endif

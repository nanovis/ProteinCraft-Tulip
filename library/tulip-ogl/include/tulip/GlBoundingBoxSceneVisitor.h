/*
 *
 * This file is part of Tulip (https://tulip.labri.fr)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
///@cond DOXYGEN_HIDDEN

#ifndef Tulip_GLBOUNDINGBOXSCENEVISITOR_H
#define Tulip_GLBOUNDINGBOXSCENEVISITOR_H
#ifndef DOXYGEN_NOTFOR_DEVEL

#include <vector>

#include <tulip/BoundingBox.h>
#include <tulip/GlSceneVisitor.h>

namespace tlp {

class GlGraphInputData;

/** \brief Visitor to collect boundingBox of all GlEntities
 *
 * Visitor to collect boundingBox of all GlEntities
 * At end, boundingBox member contains the scene boundingBox (in 3D coordinates)
 * This class can be useful to center the scene in a widget for example
 */
class TLP_GL_SCOPE GlBoundingBoxSceneVisitor : public GlSceneVisitor {

public:
  /**
   * Constructor
   */
  GlBoundingBoxSceneVisitor(GlGraphInputData *inputData);

  /**
   * Method used for GlSimpleEntity
   */
  void visit(GlSimpleEntity *entity) override;
  /**
   * Method used for GlNodes (and GlMetaNodes)
   */
  void visit(GlNode *glNode) override;
  /**
   * Method used for GlEdges
   */
  void visit(GlEdge *glEdge) override;

  /**
   * Return the scene boundingBox
   */
  BoundingBox getBoundingBox();

private:
  std::vector<bool> noBBCheck;
  std::vector<BoundingBox> bbs;
  GlGraphInputData *inputData;
};
} // namespace tlp

#endif // DOXYGEN_NOTFOR_DEVEL

#endif // Tulip_GLLODSCENEVISITOR_H
///@endcond

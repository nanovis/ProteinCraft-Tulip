/**
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
#ifndef _TreeReingoldAndTilfordExtended_H
#define _TreeReingoldAndTilfordExtended_H
#include <string>
#include <cmath>
#include <tulip/tuliphash.h>
#include <tulip/LayoutProperty.h>

struct LR {
  double L;
  double R;
  int size;
};
/** \addtogroup layout */

/** This plugin is an implementation of the hierarchical tree layout
 *  algorithm first published as:
 *
 *  E.M. Reingold and J.S. Tilford, \n
 *  "Tidier Drawings of Trees", \n
 *  "IEEE Transactions on Software Engineering" \n
 *  pages "223--228", 1981 \n
 *  doi: <a href=\"https://doi.org/10.1109/TSE.1981.234519\">doi.org/10.1109/TSE.1981.234519</a>.
 *
 *  \note This algorithm only works on trees; so if the graph is not a tree
 *  it will be applied on a spanning tree.\n
 *  Let n be the number of nodes, the algorithm complexity is in O(n).\n
 *  It extends the original Reingold and Tilford by providing management
 *  of different node size and different edge length. \n \n
 *  The algorithm use the (Size)"viewSize" property for element size and the
 *  (int)"treeEdgeLength" for the edge length.
 *
 */
class TreeReingoldAndTilfordExtended : public tlp::LayoutAlgorithm {
public:
  PLUGININFORMATION("Hierarchical Tree (R-T Extended)", "David Auber and Romain Bourqui",
                    "24/09/2010",
                    "Implements the hierarchical tree layout algorithm first published as:<br/>"
                    "<b>Tidier Drawings of Trees</b>,<br/>E.M. Reingold and J.S. Tilford, IEEE "
                    "Transactions on Software Engineering pages 223--228 (1981),<br/>"
                    "doi: <a "
                    "href=\"https://doi.org/10.1109/TSE.1981.234519\">10.1109/TSE.1981.234519</a>.",
                    "1.1", "Tree")
  TreeReingoldAndTilfordExtended(const tlp::PluginContext *context);
  ~TreeReingoldAndTilfordExtended() override;
  bool run() override;

private:
  void calcLayout(tlp::node, tlp_hash_map<tlp::node, double> *, double, double, int,
                  tlp_hash_map<int, double> &);
  double calcDecal(const std::list<LR> &, const std::list<LR> &);
  std::list<LR> *mergeLRList(std::list<LR> *, std::list<LR> *, double decal);
  std::list<LR> *TreePlace(tlp::node, tlp_hash_map<tlp::node, double> *);
  void TreeLevelSizing(tlp::node, tlp_hash_map<int, double> &, int,
                       tlp_hash_map<tlp::node, int> &levels);

  tlp::Graph *tree;
  tlp::SizeProperty *sizes;
  tlp::IntegerProperty *lengthMetric;
  bool ortho;
  bool useLength;
  std::string orientation;
  float spacing;
  float nodeSpacing;
  bool compactLayout;
};

#endif

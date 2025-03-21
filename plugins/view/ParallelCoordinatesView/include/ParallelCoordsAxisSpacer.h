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

#ifndef PARALLELCOORDSAXISSPACER_H_
#define PARALLELCOORDSAXISSPACER_H_

#include <tulip/GLInteractor.h>

namespace tlp {

class ParallelCoordinatesView;
class ParallelAxis;

class ParallelCoordsAxisSpacer : public GLInteractorComponent {

public:
  ParallelCoordsAxisSpacer();
  bool eventFilter(QObject *, QEvent *) override;
  bool draw(GlMainWidget *glMainWidget) override;
  void viewChanged(View *view) override;

private:
  void determineAxisRegionAtPos(const Coord &pos);
  void drawCurrentEditedRegionMarker();

  ParallelCoordinatesView *parallelView;
  ParallelAxis *selectedAxis;
  std::pair<ParallelAxis *, ParallelAxis *> neighborsAxis;
  int x, y;
  bool dragStarted;
};
} // namespace tlp

#endif /* PARALLELCOORDSAXISSPACER_H_ */

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

#ifndef MOUSEZOOMBOX_H
#define MOUSEZOOMBOX_H

#include <tulip/GLInteractor.h>

class QMouseEvent;
class QKeyEvent;

namespace tlp {

class Graph;

class TLP_QT_SCOPE MouseBoxZoomer : public GLInteractorComponent {
protected:
  Qt::MouseButton mButton;
  Qt::KeyboardModifier kModifier;
  unsigned int x, y;
  int w, h;
  bool started, updateViewport;
  Graph *graph;

public:
  MouseBoxZoomer(Qt::MouseButton button = Qt::LeftButton,
                 Qt::KeyboardModifier modifier = Qt::NoModifier, bool updateViewport = true);
  ~MouseBoxZoomer() override;
  bool draw(GlMainWidget *) override;
  bool eventFilter(QObject *, QEvent *) override;
};
} // namespace tlp
#endif // MOUSEZOOMBOX_H
///@endcond

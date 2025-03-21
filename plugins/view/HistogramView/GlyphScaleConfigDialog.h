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

#ifndef GLYPHSCALECONFIGDIALOG_H_
#define GLYPHSCALECONFIGDIALOG_H_

#include <QDialog>
#include <QStringList>

namespace Ui {
class GlyphScaleConfigDialogData;
}

class QShowEvent;
class QWidget;

namespace tlp {

class GlyphScaleConfigDialog : public QDialog {

  Q_OBJECT

public:
  GlyphScaleConfigDialog(QWidget *parent = nullptr);
  ~GlyphScaleConfigDialog() override;

  std::vector<int> getSelectedGlyphsId() const;

  void showEvent(QShowEvent *event) override;

private slots:

  void nbGlyphsSpinBoxValueChanged(int value);

private:
  QStringList glyphsNameList;
  Ui::GlyphScaleConfigDialogData *_ui;
};
} // namespace tlp
#endif /* GLYPHSCALECONFIGDIALOG_H_ */

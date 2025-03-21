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

#ifndef AXISCONFIGDIALOGS_H_
#define AXISCONFIGDIALOGS_H_

#ifndef DOXYGEN_NOTFOR_DEVEL

#include <QDialog>

#include <tulip/ItemsListWidget.h>

class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QCheckBox;
class QCloseEvent;

namespace tlp {

class QuantitativeParallelAxis;
class NominalParallelAxis;

// A really simple dialog which allows user to set number of graduations
// and data ordering (ascendent or not) for a particular quantitative axis
class QuantitativeAxisConfigDialog : public QDialog {

  Q_OBJECT

public:
  QuantitativeAxisConfigDialog(QuantitativeParallelAxis *axis);
  void closeEvent(QCloseEvent *event) override;

private:
  void initAxisPartitionsTable();

  QuantitativeParallelAxis *axis;
  QSpinBox *nbGrads;
  QDoubleSpinBox *doubleAxisMinValue, *doubleAxisMaxValue;
  QSpinBox *intAxisMinValue, *intAxisMaxValue;
  QComboBox *axisOrder;
  QPushButton *okButton;
  QCheckBox *log10Scale;
};

// A really simple dialog which allows user to set labels order
// on a particular nominative axis
class NominalAxisConfigDialog : public QDialog {

  Q_OBJECT

public:
  NominalAxisConfigDialog(NominalParallelAxis *axis);
  void closeEvent(QCloseEvent *event) override;

public slots:

  void pressButtonUp();
  void pressButtonDown();
  void pressButtonLexOrder();

private:
  NominalParallelAxis *axis;
  QPushButton *okButton;
  QHash<QString, itemInfo> hashDataBase;
  ItemsListWidget *axisLabelsOrder;
};
} // namespace tlp

#endif // DOXYGEN_NOTFOR_DEVEL

#endif /* AXISCONFIGDIALOGS_H_ */

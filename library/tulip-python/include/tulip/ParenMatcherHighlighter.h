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

#ifndef PARENMATCHERHIGHLIGHTER_H_
#define PARENMATCHERHIGHLIGHTER_H_

#include <QSyntaxHighlighter>
#include <QVector>

class QTextDocument;

struct ParenInfo {
  char character;
  int position;

  bool operator<(const ParenInfo &info) const {
    return position < info.position;
  }
};

class ParenInfoTextBlockData : public QTextBlockUserData {

  QVector<ParenInfo> _parenInfo;

public:
  ParenInfoTextBlockData();

  QVector<ParenInfo> parens();
  void insert(const ParenInfo &parenInfo);
  void sortParenInfo();
};

class ParenMatcherHighlighter : public QSyntaxHighlighter {
public:
  explicit ParenMatcherHighlighter(QTextDocument *parent = nullptr) : QSyntaxHighlighter(parent) {}

protected:
  void highlightBlock(const QString &text) override;
};

#endif /* PARENMATCHERHIGHLIGHTER_H_ */

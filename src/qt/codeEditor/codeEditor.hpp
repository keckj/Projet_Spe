
#include "headers.hpp"

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

class CodeEditor : public QPlainTextEdit
 {
     Q_OBJECT

 public:
     CodeEditor(QWidget *parent = 0);
     ~CodeEditor();

     void lineNumberAreaPaintEvent(QPaintEvent *event);
     int lineNumberAreaWidth();

 protected:
     void resizeEvent(QResizeEvent *event);

 private slots:
     void updateLineNumberAreaWidth(int newBlockCount);
     void highlightCurrentLine();
     void updateLineNumberArea(const QRect &, int);

 private:
     QWidget *lineNumberArea;
 };

#endif /* end of include guard: CODEEDITOR_H */

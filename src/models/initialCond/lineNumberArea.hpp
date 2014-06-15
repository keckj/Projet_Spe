
#include "headers.hpp"
#include "codeEditor.hpp"

#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

class LineNumberArea : public QWidget {
 public:
     LineNumberArea(CodeEditor *editor) : QWidget(editor) {
         codeEditor = editor;
     }

     QSize sizeHint() const {
         return QSize(codeEditor->lineNumberAreaWidth(), 0);
     }

 protected:
     void paintEvent(QPaintEvent *_event) {
         codeEditor->lineNumberAreaPaintEvent(_event);
     }

 private:
     CodeEditor *codeEditor;
 };

#endif /* end of include guard: LINENUMBERAREA_H */


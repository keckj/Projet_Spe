
#include "headers.hpp"
#include "codeEditor.hpp"
#include "codeEditor.moc"
#include "lineNumberArea.hpp"

CodeEditor::CodeEditor(QWidget *parent_) : QPlainTextEdit(parent_)
{
	lineNumberArea = new LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}
     
CodeEditor::~CodeEditor() {
}

int CodeEditor::lineNumberAreaWidth() {
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}

	int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */) {
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &_rect, int _dy) {
	if (_dy)
		lineNumberArea->scroll(0, _dy);
	else
		lineNumberArea->update(0, _rect.y(), lineNumberArea->width(), _rect.height());

	if (_rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e) {
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine() {
	QList<QTextEdit::ExtraSelection> _extraSelections;

	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::yellow).lighter(160);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		_extraSelections.append(selection);
	}

	setExtraSelections(_extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *_event) {
	QPainter painter(lineNumberArea);
	painter.fillRect(_event->rect(), Qt::lightGray);
	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();
	while (block.isValid() && top <= _event->rect().bottom()) {
		if (block.isVisible() && bottom >= _event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
					Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}
	

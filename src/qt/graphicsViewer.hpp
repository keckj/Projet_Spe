#ifndef GRAPHICS_VIEW_H
#define GRAPHICS_VIEW_H

#include "headers.hpp"

class GraphicsViewer : public QGraphicsView {

    public:
        GraphicsViewer();

    protected:
        void resizeEvent(QResizeEvent *event_) {
            if (scene())
                scene()->setSceneRect(QRect(QPoint(0,0), event_->size()));
            QGraphicsView::resizeEvent(event_);
        }
};

#endif

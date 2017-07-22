#include "imagetableview.h"

ImageTableView::ImageTableView(QWidget *parent = 0):
    QGraphicsView(parent)
{
    setMouseTracking(true);

    undoLineAction = new QAction(this);
    undoLineAction->setShortcut(QKeySequence::Undo);
}

void ImageTableView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint scenePos = mapToScene(event->pos()).toPoint();
    if(event->buttons() & Qt::RightButton)
    {
        QRect r = this->sceneRect().toRect();
        r.setTopLeft(r.topLeft() + lmpos - scenePos);

        this->setSceneRect(r);
    }

    emit mouseMoved(event);

    lmpos = scenePos;
}

void ImageTableView::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(event);
}

void ImageTableView::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() & Qt::ControlModifier)
    {
        if(event->angleDelta().y() > 0)
            this->scale(1.1, 1.1);
        else
            this->scale(0.9, 0.9);
    }
    else
        QGraphicsView::wheelEvent(event);
}

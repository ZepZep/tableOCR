#ifndef IMAGETABLEVIEW_H
#define IMAGETABLEVIEW_H


#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QAction>

#include <QDebug>

namespace Ui {
class ImageTableView;
}

class ImageTableView : public QGraphicsView
{
    Q_OBJECT
public:
    ImageTableView(QWidget *parent);

    void setMode(QString m);
    QAction *undoLineAction;

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QString mode;
    QGraphicsScene *myScene;

    QPoint lmpos;
signals:
    void mouseMoved(QMouseEvent *event);
    void mousePressed(QMouseEvent *event);


};

#endif // IMAGETABLEVIEW_H

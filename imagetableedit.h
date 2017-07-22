#ifndef IMAGETABLEEDIT_H
#define IMAGETABLEEDIT_H

#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsPixmapItem>

#include "imagetableview.h"

#include <QImage>

#include <QDebug>

namespace Ui {
class ImageTableEdit;
}

class ImageTableEdit : public QWidget
{
    Q_OBJECT

public:
    explicit ImageTableEdit(QWidget *parent = 0);
    ~ImageTableEdit();

    void loadImage(QString name);
    void loadImage(QImage image);
    QVector<QVector<QImage> > getCells();
    void showChopped(QVector<QVector<QImage> > imgs);

private:
    Ui::ImageTableEdit *ui;

    ImageTableView *view;
    QGraphicsScene *scene;

    QString mode;
    QGraphicsLineItem *vLine;

    QImage pdfImage;
    QGraphicsPixmapItem *pdfPixmap;

    QPen linePen;
    QList<QPoint> horizontal, vertical;
    QList<QGraphicsLineItem *> splits;

protected:
    void resizeEvent(QResizeEvent *e);


private slots:
    void cursorMoved(QMouseEvent *e);
    void mousePressed(QMouseEvent *e);
    void on_pBBack_clicked();
    void on_pBResetLines_clicked();
    void on_pBResetView_clicked();
};

#endif // IMAGETABLEEDIT_H

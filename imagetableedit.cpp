#include "imagetableedit.h"
#include "ui_imagetableedit.h"

ImageTableEdit::ImageTableEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageTableEdit)
{
    ui->setupUi(this);

    view = new ImageTableView(this);
    scene = new QGraphicsScene(this);
    view->setScene(scene);

    ui->layView->addWidget(view);

    mode = "";

    connect(view, SIGNAL(mouseMoved(QMouseEvent*)), this, SLOT(cursorMoved(QMouseEvent*)));
    connect(view, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(mousePressed(QMouseEvent*)));
    connect(view->undoLineAction, SIGNAL(triggered(bool)), this, SLOT(on_pBBack_clicked()));

    linePen.setColor(Qt::red);
    linePen.setWidth(5);
    vLine = scene->addLine(0, 0, 0, 0, linePen);
    pdfPixmap = new QGraphicsPixmapItem;
    pdfPixmap->setZValue(-5);
    scene->addItem(pdfPixmap);
}

ImageTableEdit::~ImageTableEdit()
{
    delete ui;
}

void ImageTableEdit::loadImage(QString name)
{
    pdfImage = QImage(name);
    loadImage(pdfImage);
}

void ImageTableEdit::loadImage(QImage image)
{
    qDeleteAll(scene->items());
    splits.clear();
    horizontal.clear();
    vertical.clear();
    vLine = scene->addLine(0, 0, 0, 0, linePen);
    pdfPixmap = new QGraphicsPixmapItem;

    pdfImage = image;
    pdfPixmap->setPixmap(QPixmap::fromImage(pdfImage));
    pdfPixmap->setZValue(-5);
    scene->addItem(pdfPixmap);
    view->fitInView(pdfPixmap, Qt::KeepAspectRatio);
}

QVector<QVector<QImage> > ImageTableEdit::getCells()
{
    QSize s(vertical.size()-1, horizontal.size()-1);

    QVector<QVector<QImage> > output;

    for(int x = 0; x < s.width(); x++)
    {
        output.append(QVector<QImage>());
        for(int y = 0; y < s.height(); y++)
        {
            QRect r;
            r.setTopLeft(QPoint(vertical[x].x(), horizontal[y].y()));
            r.setBottomRight(QPoint(vertical[x+1].x(), horizontal[y+1].y()));
            output[x].append(pdfImage.copy(r));
        }
    }

    showChopped(output);

    return output;
}

void ImageTableEdit::showChopped(QVector<QVector<QImage> > imgs)
{
    qDeleteAll(scene->items());
    splits.clear();
    horizontal.clear();
    vertical.clear();

    vLine = scene->addLine(0, 0, 0, 0, linePen);

    int spacing = 70;

    QPointF p(0, 0);

    QGraphicsPixmapItem *pi;
    for(int y = 0; y < imgs[0].size(); y++)
    {
        p.setX(0);
        for(int x = 0; x < imgs.size(); x++)
        {
            pi = scene->addPixmap(QPixmap::fromImage(imgs[x][y]));
            pi->setPos(p);

            p.setX(p.x() + spacing + imgs[x][0].width());
        }
        p.setY(p.y() + spacing + imgs[0][y].height());
    }
}

void ImageTableEdit::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    view->fitInView(pdfPixmap, Qt::KeepAspectRatio);
}

void ImageTableEdit::cursorMoved(QMouseEvent *e)
{
    QPoint scenePos = view->mapToScene(e->pos()).toPoint();

    if(scenePos.x() > scenePos.y())
    {
        //vertical
        if(scenePos.x() < 0) scenePos.setX(0);
        if(scenePos.x() > pdfImage.width()) scenePos.setX(pdfImage.width());
        QPoint tl = QPoint(scenePos.x(), -20);
        QPoint rb = QPoint(scenePos.x(), pdfImage.height()+20);
        vLine->setLine(QLine(tl, rb));
    }
    else
    {
        //Horizontal
        if(scenePos.y() < 0) scenePos.setY(0);
        if(scenePos.y() > pdfImage.height()) scenePos.setY(pdfImage.height());
        QPoint tl = QPoint(-20, scenePos.y());
        QPoint rb = QPoint(pdfImage.width()+20, scenePos.y());
        vLine->setLine(QLine(tl, rb));
    }
}

void ImageTableEdit::mousePressed(QMouseEvent *e)
{
    if(e->button() & Qt::LeftButton)
    {
        splits.append(scene->addLine(vLine->line(), linePen));
        QPoint scenePos = view->mapToScene(e->pos()).toPoint();

        if(scenePos.x() > scenePos.y())
        {
            //vertical
            vertical.append(vLine->line().p1().toPoint());
            //qDebug() <<"Vertical " << vertical.last();
        }
        else
        {
            //Horizontal
            horizontal.append(vLine->line().p1().toPoint());
            //qDebug() <<"Horizontal " << horizontal.last();
        }
    }
}

void ImageTableEdit::on_pBBack_clicked()
{
    if(splits.size()>0)
    {
        if(splits.last()->line().y1() == - 20)
        {
            //vertical
            vertical.removeLast();
        }
        else
        {
            //Horizontal
            horizontal.removeLast();
        }

        scene->removeItem(splits.last());
        splits.removeLast();
    }
}

void ImageTableEdit::on_pBResetLines_clicked()
{

}

void ImageTableEdit::on_pBResetView_clicked()
{

}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QProcess>
#include <QImage>

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

#include <QDialog>

#include "imagetableedit.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pBChop_clicked();

    void sigImportFile();
    void sigClearProj();
    void sigExportFile();

    void sigPrewPage();
    void sigNextPage();

    void changePageRel(int where);
    void changePage(int where);

    void processFinished(int status);

    void startOCR();
    void segmentOCRd();
    void stopOCR();

private:
    Ui::MainWindow *ui;
    QImage *image;

    ImageTableEdit *tableEdit;

    QMenu *fileMenu;
    QAction *importFile, *clearProj, *exportFileA;
    QAction *nextPage, *prewPage;
    QAction *chopAction;

    QProcess *processor;
    QString processMode;

    QPoint curOCRPos;
    int curOCRi;
    int ocrMax;
    QSize ocrSize;
    QVector<QVector<QImage> > ocrImgMat;

    QMessageBox msgBox;
    QDialog *waitingDialog;

    QVector<QImage> inputImages;

    QFile exportFile;

    void setupActions();
    void prepImgForOCR(QPoint p);
    void processCell(QPoint p, QString text);
    void loadProjectDir();
};

#endif // MAINWINDOW_H

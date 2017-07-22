#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tableEdit = new ImageTableEdit(this);
    ui->layCenter->addWidget(tableEdit);

    //tableEdit->loadImage("/home/dummy/Dokumenty/cvt/cj-1.png");
    ui->lOutOf->setText("0");

    processor = new QProcess(this);
    processMode = "";
    connect(processor, SIGNAL(finished(int)), this, SLOT(processFinished(int)));

    setupActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pBChop_clicked()
{
    startOCR();
}

void MainWindow::sigImportFile()
{
    QDir dir("proj");
    if(dir.exists()) dir.removeRecursively();
    QDir::current().mkdir("proj");

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"));
    if(fileName != "")
    {
        if(fileName.right(3) == "pdf" or fileName.right(3) == "PDF" )
        {
            processMode = "CVT";
            QString command = "pdftoppm -png -r 300 \"" + fileName + "\" proj/page";
            qDebug() <<"Loading pdf: " <<command;
            processor->start(command);
//            msgBox.setText("Converting...");
//            msgBox.setInformativeText("Please wait.");
//            msgBox.setWindowTitle("Converting");
//            msgBox.setModal(true);
//            msgBox.setStandardButtons(QMessageBox::NoButton);
//            msgBox.show();
//            msgBox.resize(200, 150);
            this->setCursor(Qt::WaitCursor);
            importFile->setDisabled(true);
        }
        else
        {
            QFile::copy(fileName, "proj/page-1"+fileName.right(4));
            loadProjectDir();
        }
    }

    QDir exDir("export");
    if(exDir.exists()) exDir.removeRecursively();
    QDir::current().mkdir("export");
}

void MainWindow::sigClearProj()
{

}

void MainWindow::sigExportFile()
{
    if(exportFile.isOpen()) exportFile.close();
    exportFile.setFileName("export/page-"+QString::number(ui->sBPage->value())+".csv");
    exportFile.open(QIODevice::WriteOnly);

    QTextStream exportStream(&exportFile);
    for(int y = 0; y < ui->tableWidget->rowCount(); y++)
    {
        for(int x = 0; x < ui->tableWidget->columnCount(); x++)
        {
            exportStream <<"\"" << ui->tableWidget->item(y, x)->text() <<"\"";
            if(x<ui->tableWidget->columnCount()-1) exportStream <<",";
            else exportStream <<"\n";
        }
    }
}

void MainWindow::sigPrewPage()
{
    changePageRel(-1);
}

void MainWindow::sigNextPage()
{
    changePageRel(1);
}

void MainWindow::processFinished(int status)
{
    if(processMode == "OCR")
        segmentOCRd();
    else if(processMode == "CVT")
    {
        if(status != 0)
            qDebug() <<processor->errorString();
        importFile->setDisabled(false);
        this->setCursor(Qt::ArrowCursor);
        loadProjectDir();
    }
}

void MainWindow::startOCR()
{
    processMode = "OCR";

    ocrImgMat = tableEdit->getCells();
    ocrSize.setWidth(ocrImgMat.size());
    ocrSize.setHeight(ocrImgMat[0].size());

    curOCRi = 0;
    ocrMax = ocrSize.width()*ocrSize.height();

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(ocrSize.width());
    ui->tableWidget->setRowCount(ocrSize.height());

    curOCRPos = QPoint(curOCRi%ocrSize.width(), curOCRi/ocrSize.width());

    qDebug() <<"Processing " <<curOCRPos;

    QString command = "tesseract proj/ocr.png stdout -l ces";

    prepImgForOCR(curOCRPos);
    processor->start(command);
}

void MainWindow::segmentOCRd()
{
    processCell(curOCRPos, QString::fromUtf8(processor->readAll()));

    curOCRi++;
    if(curOCRi < ocrMax)
    {
        curOCRPos = QPoint(curOCRi%ocrSize.width(), curOCRi/ocrSize.width());
        qDebug() <<"Processing " <<curOCRPos;

        QString command = "tesseract proj/ocr.png stdout -l ces -psm 4";
        prepImgForOCR(curOCRPos);
        processor->start(command);
    }
    else
        stopOCR();
}

void MainWindow::stopOCR()
{
    processMode = "";
}

void MainWindow::setupActions()
{
    fileMenu = ui->menuBar->addMenu("File");

    importFile = new QAction("Import...", this);
    fileMenu->addAction(importFile);
    ui->mainToolBar->addAction(importFile);
    connect(importFile, SIGNAL(triggered(bool)), this, SLOT(sigImportFile()));

    exportFileA = new QAction("Export...", this);
    fileMenu->addAction(exportFileA);
    ui->mainToolBar->addAction(exportFileA);
    connect(exportFileA, SIGNAL(triggered(bool)), this, SLOT(sigExportFile()));

    ui->mainToolBar->addSeparator();

    clearProj = new QAction("Clear Project", this);
    fileMenu->addAction(clearProj);
    ui->mainToolBar->addAction(clearProj);
    connect(clearProj, SIGNAL(triggered(bool)), this, SLOT(sigClearProj()));

    ui->mainToolBar->addSeparator();

    chopAction = new QAction("Chop", this);
    fileMenu->addAction(chopAction);
    ui->mainToolBar->addAction(chopAction);
    connect(chopAction, SIGNAL(triggered(bool)), this, SLOT(on_pBChop_clicked()));

    prewPage = new QAction("Prewious Page", this);
    fileMenu->addAction(prewPage);
    ui->mainToolBar->addAction(prewPage);
    connect(prewPage, SIGNAL(triggered(bool)), this, SLOT(sigPrewPage()));

    nextPage = new QAction("Next Page", this);
    fileMenu->addAction(nextPage);
    ui->mainToolBar->addAction(nextPage);
    connect(nextPage, SIGNAL(triggered(bool)), this, SLOT(sigNextPage()));

    connect(ui->pBLeft, SIGNAL(clicked(bool)), prewPage, SLOT(trigger()));
    connect(ui->pBRight, SIGNAL(clicked(bool)), nextPage, SLOT(trigger()));
    connect(ui->sBPage, SIGNAL(valueChanged(int)), this, SLOT(changePage(int)));
}

void MainWindow::prepImgForOCR(QPoint p)
{
    ocrImgMat[p.x()][p.y()].save("proj/ocr.png");
}

void MainWindow::processCell(QPoint p, QString text)
{
    QString ws = "\n \t.";
    while(text.size() > 0 and ws.contains(text.right(1))) text.chop(1);
    while(text.size() > 0 and ws.contains(text.left(1))) text = text.right(text.size()-1);

    QTableWidgetItem *newItem = new QTableWidgetItem(text);
    ui->tableWidget->setItem(p.y(), p.x(), newItem);

    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
}

void MainWindow::loadProjectDir()
{
    QDir pDir("proj");

    inputImages.clear();
    foreach (QString fileName, pDir.entryList())
    {
        if(fileName != "." and fileName != "..")
        {
            inputImages.append(QImage("proj/"+fileName));
            qDebug() <<"Loading " <<fileName;
        }
    }

    if(inputImages.size()>0)
    {
        ui->lOutOf->setText(QString::number(inputImages.size()));
        ui->sBPage->setMaximum(inputImages.size());
        ui->sBPage->setMinimum(1);

        ui->sBPage->setValue(1);
        tableEdit->loadImage(inputImages.at(ui->sBPage->value()-1));
    }

}

void MainWindow::changePageRel(int where)
{
   changePage(ui->sBPage->value() + where);
}

void MainWindow::changePage(int where)
{
    if(where > 0 and where <= inputImages.size())
    {
        ui->sBPage->setValue(where);
        tableEdit->loadImage(inputImages.at(ui->sBPage->value()-1));
    }
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loadDocuments();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadDocuments()
{
    QDir dir(QDir::homePath() + "/phase-portrait-practicum/documents");

    QStringList files = dir.entryList({"*.doc", "*.docx"},
                                     QDir::Files, QDir::Name);

    std::unique_ptr<QStringListModel> model = std::make_unique<QStringListModel>();
    model->setStringList(files);
    ui->lw_documents->setModel(model.release());

    for (const auto &fileName : files) {
        std::cout << fileName.toStdString() << std::endl;
        std::cout << dir.absoluteFilePath(fileName).toStdString() << std::endl;
    }

    QObject::connect(ui->lw_documents, &QListView::doubleClicked, [dir](const QModelIndex &index) {
        QString fileName = index.data().toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(dir.absoluteFilePath(fileName)));
    });

    //QString filePath = dir.absoluteFilePath(fileName);
}

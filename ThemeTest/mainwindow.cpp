#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QDebug>
#include <QFile>

#include <pbscolorconfig.h>
#include <pbscolorscheme.h>
#include <QMessageBox>
#include <pbsskin.h>

#define USEKVANTUM

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifdef USEKVANTUM
    QDir path(qApp->applicationDirPath() + "/Themes/kvAntum");
    QStringList files = path.entryList(QDir::Dirs);

    qDebug() << files;

    ui->colorsCombo->addItems(files);
#else
    QDir path(qApp->applicationDirPath() + "/Colors");
    QStringList files = path.entryList(QDir::Files);

    ui->colorsCombo->addItems(files);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_colorsCombo_currentIndexChanged(const QString &arg1)
{
#ifdef USEKVANTUM
        Kvantum::PBSSkin *sk = static_cast<Kvantum::PBSSkin *>(qApp->style());
        if(sk)
        {
            if(arg1.at(0) != '.')
            {
                try
                {
                    sk->setTheme(arg1, false);
                    update();
                }
                catch (std::exception &e)
                {
                    qDebug() << "Hata :" << e.what();
                }
            }
        }
#else
    if(arg1.at(0) == '.')
        return;

    QString s(qApp->applicationDirPath() + QString("/Colors/%1").arg(arg1));
    qDebug() << s;

    if(QFile::exists(s))
    {
        PBSColorConfig c(s);
        // PBSColorScheme sc(QPalette::Active, c);

        qApp->setPalette(PBSColorScheme::createApplicationPalette(static_cast<PBSColorConfig&>(c)));

        update();
    }
#endif
}

void MainWindow::on_pushButton_8_clicked()
{
    QMessageBox::question(this, "Test", "Deneme 123",
                                    QMessageBox::Yes|QMessageBox::No);
}

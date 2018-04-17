#include "mainwindow.h"
#include <QApplication>

#include <PBSColorConfig.h>
#include <pbscolorscheme.h>
#include <QDebug>

#include <Kvantum.h>
#include <pbsskin.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

/*
    PBSColorConfig c("e:\\QtTest\\ThemeTest2\\build-ThemeTest-Desktop_Qt_5_10_0_MinGW_32bit-Debug\\debug\\colors\\KvAdapta.colors");

    qDebug() << "String : " << c.readString("Colors:Button", "ForegroundActive", "0.00");
    qDebug() << "Integer: " << c.readInt("ColorEffects:Inactive", "ColorEffect", 0);
    qDebug() << "Bool   : " << c.readBool("ColorEffects:Inactive", "ChangeSelectionColor", false);
    qDebug() << "Real   : " << c.readQReal("ColorEffects:Inactive", "ContrastAmount", 11.00);
    qDebug() << "Color  : " << c.readColor("Colors:Button", "ForegroundNegative", QColor(0, 0, 0));

    qDebug() << "Renk   : " << QColor(191,3,3);
*/

    Kvantum::PBSSkin *style = new Kvantum::PBSSkin(false);
    a.setStyle(style);

    // a.setStyle("Fusion");

//    PBSColorConfig c("e:\\QtTest\\ThemeTest2\\build-ThemeTest-Desktop_Qt_5_10_0_MinGW_32bit-Debug\\debug\\colors\\KvAdaptaDark.colors");
//    a.setPalette(PBSColorScheme::createApplicationPalette(static_cast<PBSColorConfig&>(c)));

    MainWindow w;
    w.show();

    return a.exec();
}

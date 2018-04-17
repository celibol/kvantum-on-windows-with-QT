#ifndef PBSSKIN_H
#define PBSSKIN_H

#include <QObject>
#include <QDir>
#include <QSvgRenderer>

#include <kvAntum.h>

namespace Kvantum {

class PBSSkin : public Style
{
    Q_OBJECT
public:
    PBSSkin(bool useDark);

    void setTheme(const QString &baseThemeName, bool useDark) Q_DECL_OVERRIDE;
signals:

public slots:
private:
    void initTheme();
    void installColorScheme();
};

}
#endif // PBSSKIN_H

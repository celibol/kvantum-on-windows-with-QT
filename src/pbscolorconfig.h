#ifndef PBSCOLORCONFIG_H
#define PBSCOLORCONFIG_H

#include <QSettings>
#include <QColor>
#include <QBrush>

class PBSColorConfig
{
public:
    PBSColorConfig(const QString);
    ~PBSColorConfig();

    QString readString(const QString &group, const QString &key, QString aDefault);
    int readInt(const QString &group, const QString &key, int aDefault);
    qreal readQReal(const QString &group, const QString &key, qreal aDefault);
    bool readBool(const QString &group, const QString &key, bool aDefault);
    QColor readColor(const QString &group, const QString &key, QColor aDefault);

    QColor readBrush(const QString &group, const QString &key, QColor);

    QSettings &settings() { return colorSettings; }

    static QByteArray serializeList(const QList<QByteArray> &list);
    static QStringList deserializeList(const QString &data);

    qreal contrastF();
private:
    QSettings colorSettings;
};

#endif // PBSCOLORCONFIG_H

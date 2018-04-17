#ifndef PBSCOLORSCHEME_H
#define PBSCOLORSCHEME_H

#include <QSettings>
#include <QPalette>

#include <pbscolorconfig.h>

class QColor;
class QBrush;
class PBSColorSchemePrivate;

class PBSColorScheme
{
public:
    enum ColorSet {
        View,
        Window,
        Button,
        Selection,
        Tooltip,
        Complementary
    };

    enum BackgroundRole {
        NormalBackground = 0,
        AlternateBackground = 1,
        ActiveBackground = 2,
        LinkBackground = 3,
        VisitedBackground = 4,
        NegativeBackground = 5,
        NeutralBackground = 6,
        PositiveBackground = 7
    };

    enum ForegroundRole {
        NormalText = 0,
        InactiveText = 1,
        ActiveText = 2,
        LinkText = 3,
        VisitedText = 4,
        NegativeText = 5,
        NeutralText = 6,
        PositiveText = 7
    };

    enum DecorationRole {
        FocusColor,
        HoverColor
    };

    enum ShadeRole {
        LightShade,
        MidlightShade,
        MidShade,
        DarkShade,
        ShadowShade
    };

    PBSColorScheme(const PBSColorScheme &);
    PBSColorScheme &operator=(const PBSColorScheme &);
    explicit PBSColorScheme(QPalette::ColorGroup, PBSColorConfig&, ColorSet = View);

    virtual ~PBSColorScheme();

    QBrush background(BackgroundRole = NormalBackground) const;
    QBrush foreground(ForegroundRole = NormalText) const;
    QBrush decoration(DecorationRole) const;
    QColor shade(PBSColorConfig&, ShadeRole) const;

    static int contrast(PBSColorConfig);
    static qreal contrastF(PBSColorConfig&);

    static QColor shade(PBSColorConfig&, const QColor &, ShadeRole);
    static QColor shade(PBSColorConfig&, const QColor &, ShadeRole, qreal contrast, qreal chromaAdjust = 0.0);

    static void adjustBackground(PBSColorConfig,
                                 QPalette &,
                                 BackgroundRole newRole = NormalBackground,
                                 QPalette::ColorRole color = QPalette::Base,
                                 ColorSet set = View);

    static void adjustForeground(PBSColorConfig,
                                 QPalette &,
                                 ForegroundRole newRole = NormalText,
                                 QPalette::ColorRole color = QPalette::Text,
                                 ColorSet set = View);

    static QPalette createApplicationPalette(PBSColorConfig&);
private:
    PBSColorSchemePrivate *d;
};

#endif // PBSCOLORSCHEME_H

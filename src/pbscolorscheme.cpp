#include "pbscolorscheme.h"
#include <kcolorutils.h>
#include <QDebug>

#define DEFAULT(c) QColor( c[0], c[1], c[2] )
#define SET_DEFAULT(a) DEFAULT( defaults.a )
#define DECO_DEFAULT(a) DEFAULT( defaultDecorationColors.a )

class StateEffects
{
public:
    explicit StateEffects(QPalette::ColorGroup state, PBSColorConfig &);
    ~StateEffects() {}

    QBrush brush(const QBrush &background) const;
    QBrush brush(const QBrush &foreground, const QBrush &background) const;

private:
    enum Effects {
        // Effects
        Intensity = 0,
        Color = 1,
        Contrast = 2,
        // Intensity
        IntensityNoEffect = 0,
        IntensityShade = 1,
        IntensityDarken = 2,
        IntensityLighten = 3,
        // Color
        ColorNoEffect = 0,
        ColorDesaturate = 1,
        ColorFade = 2,
        ColorTint = 3,
        // Contrast
        ContrastNoEffect = 0,
        ContrastFade = 1,
        ContrastTint = 2
    };

    int _effects[3];
    double _amount[3];
    QColor _color;
//     StateEffects *_chain; not needed yet
};

StateEffects::StateEffects(QPalette::ColorGroup state, PBSColorConfig &config)
    : _color(0, 0, 0, 0) //, _chain(0) not needed yet
{
    QString group;
    if (state == QPalette::Disabled) {
        group = QStringLiteral("ColorEffects:Disabled");
    } else if (state == QPalette::Inactive) {
        group = QStringLiteral("ColorEffects:Inactive");
    }

    _effects[0] = 0;
    _effects[1] = 0;
    _effects[2] = 0;

    // NOTE: keep this in sync with kdebase/workspace/kcontrol/colors/colorscm.cpp
    if (! group.isEmpty()) {
        bool enabledByDefault = (state == QPalette::Disabled);
        if (config.readBool(group, "Enable", enabledByDefault)) {
            _effects[Intensity] = config.readInt(group, "IntensityEffect",
                                                (int)(state == QPalette::Disabled ?  IntensityDarken : IntensityNoEffect));
            _effects[Color]     = config.readInt(group, "ColorEffect",
                                                (int)(state == QPalette::Disabled ?  ColorNoEffect : ColorDesaturate));
            _effects[Contrast]  = config.readInt(group, "ContrastEffect",
                                                (int)(state == QPalette::Disabled ?  ContrastFade : ContrastTint));
            _amount[Intensity]  = config.readQReal(group, "IntensityAmount", state == QPalette::Disabled ? 0.10 :  0.0);
            _amount[Color]      = config.readQReal(group, "ColorAmount", state == QPalette::Disabled ?  0.0 : -0.9);
            _amount[Contrast]   = config.readQReal(group, "ContrastAmount", state == QPalette::Disabled ? 0.65 :  0.25);
            if (_effects[Color] > ColorNoEffect) {
                _color = config.readColor(group, "Color", state == QPalette::Disabled ?  QColor(56, 56, 56) : QColor(112, 111, 110));
            }
        }
    }
}

QBrush StateEffects::brush(const QBrush &background) const
{
    QColor color = background.color(); // TODO - actually work on brushes
    switch (_effects[Intensity]) {
    case IntensityShade:
        color = KColorUtils::shade(color, _amount[Intensity]);
        break;
    case IntensityDarken:
        color = KColorUtils::darken(color, _amount[Intensity]);
        break;
    case IntensityLighten:
        color = KColorUtils::lighten(color, _amount[Intensity]);
        break;
    }
    switch (_effects[Color]) {
    case ColorDesaturate:
        color = KColorUtils::darken(color, 0.0, 1.0 - _amount[Color]);
        break;
    case ColorFade:
        color = KColorUtils::mix(color, _color, _amount[Color]);
        break;
    case ColorTint:
        color = KColorUtils::tint(color, _color, _amount[Color]);
        break;
    }
    return QBrush(color);
}

QBrush StateEffects::brush(const QBrush &foreground, const QBrush &background) const
{
    QColor color = foreground.color(); // TODO - actually work on brushes
    QColor bg = background.color();
    // Apply the foreground effects
    switch (_effects[Contrast]) {
    case ContrastFade:
        color = KColorUtils::mix(color, bg, _amount[Contrast]);
        break;
    case ContrastTint:
        color = KColorUtils::tint(color, bg, _amount[Contrast]);
        break;
    }
    // Now apply global effects
    return brush(color);
}


struct SetDefaultColors {
    int NormalBackground[3];
    int AlternateBackground[3];
    int NormalText[3];
    int InactiveText[3];
    int ActiveText[3];
    int LinkText[3];
    int VisitedText[3];
    int NegativeText[3];
    int NeutralText[3];
    int PositiveText[3];
};

struct DecoDefaultColors {
    int Hover[3];
    int Focus[3];
};

// these numbers come from the Breeze color scheme ([breeze]/colors/Breeze.colors)
static const SetDefaultColors defaultViewColors = {
    { 252, 252, 252 }, // Background
    { 252, 252, 252 }, // Alternate
    {  35,  38, 39  }, // Normal
    { 127, 140, 141 }, // Inactive
    {  61, 174, 233 }, // Active
    {  41, 128, 185 }, // Link
    { 127, 140, 141 }, // Visited
    { 218,  68,  83 }, // Negative
    { 246, 116,   0 }, // Neutral
    {  39, 174,  96 }  // Positive
};

static const SetDefaultColors defaultWindowColors = {
    { 252, 252, 252 }, // Background
    { 189, 195, 199 }, // Alternate
    {  35,  38, 39  }, // Normal
    { 127, 140, 141 }, // Inactive
    {  61, 174, 233 }, // Active
    {  41, 128, 185 }, // Link
    { 127, 140, 141 }, // Visited
    { 218,  68,  83 }, // Negative
    { 246, 116,   0 }, // Neutral
    {  39, 174,  96 }  // Positive
};

static const SetDefaultColors defaultButtonColors = {
    { 252, 252, 252 }, // Background
    { 189, 195, 199 }, // Alternate
    {  35,  38, 39  }, // Normal
    { 127, 140, 141 }, // Inactive
    {  61, 174, 233 }, // Active
    {  41, 128, 185 }, // Link
    { 127, 140, 141 }, // Visited
    { 218,  68,  83 }, // Negative
    { 246, 116,   0 }, // Neutral
    {  39, 174,  96 }  // Positive
};

static const SetDefaultColors defaultSelectionColors = {
    {  61, 174, 233 }, // Background
    {  29, 153, 243 }, // Alternate
    { 252, 252, 252 }, // Normal
    { 252, 252, 252 }, // Inactive
    { 252, 252, 252 }, // Active
    { 253, 188,  75 }, // Link
    { 189, 195, 199 }, // Visited
    { 218,  68,  83 }, // Negative
    { 246, 116,   0 }, // Neutral
    {  39, 174,  96 }  // Positive
};

static const SetDefaultColors defaultTooltipColors = {
    {  35,  38, 39  }, // Background
    {  77,  77,  77 }, // Alternate
    { 252, 252, 252 }, // Normal
    { 189, 195, 199 }, // Inactive
    {  61, 174, 233 }, // Active
    {  41, 128, 185 }, // Link
    { 127, 140, 141 }, // Visited
    { 218,  68,  83 }, // Negative
    { 246, 116,   0 }, // Neutral
    {  39, 174,  96 }  // Positive
};

static const SetDefaultColors defaultComplementaryColors = {
    {  49,  54,  59 }, // Background
    {  77,  77,  77 }, // Alternate
    { 239, 240, 241 }, // Normal
    { 189, 195, 199 }, // Inactive
    {  61, 174, 233 }, // Active
    {  41, 128, 185 }, // Link
    { 127, 140, 141 }, // Visited
    { 218,  68,  83 }, // Negative
    { 246, 116,   0 }, // Neutral
    {  39, 174,  96 }  // Positive
};

static const DecoDefaultColors defaultDecorationColors = {
    { 147, 206, 233 }, // Hover
    {  61, 174, 233 }, // Focus
};

class PBSColorSchemePrivate : public QSharedData
{
public:
    explicit PBSColorSchemePrivate(PBSColorConfig &, QPalette::ColorGroup, const QString &, SetDefaultColors);
    explicit PBSColorSchemePrivate(PBSColorConfig &, QPalette::ColorGroup, const QString &, SetDefaultColors, const QBrush &);

    ~PBSColorSchemePrivate() {}

    QBrush background(PBSColorScheme::BackgroundRole) const;
    QBrush foreground(PBSColorScheme::ForegroundRole) const;
    QBrush decoration(PBSColorScheme::DecorationRole) const;
    qreal contrast() const;
private:
    struct {
        QBrush fg[8], bg[8], deco[2];
    } _brushes;

    qreal _contrast;

    void init(PBSColorConfig &, QPalette::ColorGroup, QString, SetDefaultColors);
};

PBSColorSchemePrivate::PBSColorSchemePrivate(PBSColorConfig &config,
        QPalette::ColorGroup state,
        const QString &group,
        SetDefaultColors defaults)
{
    _contrast = config.contrastF();
    qDebug() << "Con: " << _contrast;

    // loaded-from-config colors (no adjustment)
    _brushes.bg[0] = config.readBrush(group, QString("BackgroundNormal"), SET_DEFAULT(NormalBackground));    
    _brushes.bg[1] = config.readBrush(group, QString("BackgroundAlternate"), SET_DEFAULT(AlternateBackground));

    qDebug() << "Brush " << group << " " << _brushes.bg[0] << " Brush 2 " << _brushes.bg[1].color();
    // the rest
    init(config, state, group, defaults);
}

PBSColorSchemePrivate::PBSColorSchemePrivate(PBSColorConfig &config,
        QPalette::ColorGroup state,
        const QString &group,
        SetDefaultColors defaults,
        const QBrush &tint)
{
    _contrast = config.contrastF();

    _brushes.bg[0] = config.readBrush(group, QString("BackgroundNormal"), SET_DEFAULT(NormalBackground));
    _brushes.bg[1] = config.readBrush(group, QString("BackgroundAlternate"), SET_DEFAULT(AlternateBackground));

    _brushes.bg[0] = KColorUtils::tint(_brushes.bg[0].color(), tint.color(), 0.4);
    _brushes.bg[1] = KColorUtils::tint(_brushes.bg[1].color(), tint.color(), 0.4);

    init(config, state, group, defaults);
}

void PBSColorSchemePrivate::init(PBSColorConfig &cfg,
                               QPalette::ColorGroup state,
                               QString group,
                               SetDefaultColors defaults)
{
    // loaded-from-config colors
    _brushes.fg[0] = cfg.readBrush(group, "ForegroundNormal", SET_DEFAULT(NormalText));
    _brushes.fg[1] = cfg.readBrush(group, "ForegroundInactive", SET_DEFAULT(InactiveText));
    _brushes.fg[2] = cfg.readBrush(group, "ForegroundActive", SET_DEFAULT(ActiveText));
    _brushes.fg[3] = cfg.readBrush(group, "ForegroundLink", SET_DEFAULT(LinkText));
    _brushes.fg[4] = cfg.readBrush(group, "ForegroundVisited", SET_DEFAULT(VisitedText));
    _brushes.fg[5] = cfg.readBrush(group, "ForegroundNegative", SET_DEFAULT(NegativeText));
    _brushes.fg[6] = cfg.readBrush(group, "ForegroundNeutral", SET_DEFAULT(NeutralText));
    _brushes.fg[7] = cfg.readBrush(group, "ForegroundPositive", SET_DEFAULT(PositiveText));

    _brushes.deco[0] = cfg.readBrush(group, "DecorationHover", DECO_DEFAULT(Hover));
    _brushes.deco[1] = cfg.readBrush(group, "DecorationFocus", DECO_DEFAULT(Focus));

    // apply state adjustments
    if (state != QPalette::Active) {
        StateEffects effects(state, cfg);
        for (int i = 0; i < 8; i++) {
            _brushes.fg[i] = effects.brush(_brushes.fg[i], _brushes.bg[0]);
        }
        _brushes.deco[0] = effects.brush(_brushes.deco[0], _brushes.bg[0]);
        _brushes.deco[1] = effects.brush(_brushes.deco[1], _brushes.bg[0]);
        _brushes.bg[0] = effects.brush(_brushes.bg[0]);
        _brushes.bg[1] = effects.brush(_brushes.bg[1]);
    }

    // calculated backgrounds
    _brushes.bg[2] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[2].color());
    _brushes.bg[3] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[3].color());
    _brushes.bg[4] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[4].color());
    _brushes.bg[5] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[5].color());
    _brushes.bg[6] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[6].color());
    _brushes.bg[7] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[7].color());
}

QBrush PBSColorSchemePrivate::background(PBSColorScheme::BackgroundRole role) const
{
    switch (role)
    {
        case PBSColorScheme::AlternateBackground: return _brushes.bg[1];
        case PBSColorScheme::ActiveBackground   : return _brushes.bg[2];
        case PBSColorScheme::LinkBackground     : return _brushes.bg[3];
        case PBSColorScheme::VisitedBackground  : return _brushes.bg[4];
        case PBSColorScheme::NegativeBackground : return _brushes.bg[5];
        case PBSColorScheme::NeutralBackground  : return _brushes.bg[6];
        case PBSColorScheme::PositiveBackground : return _brushes.bg[7];
        default: return _brushes.bg[0];
    }
}

QBrush PBSColorSchemePrivate::foreground(PBSColorScheme::ForegroundRole role) const
{
    switch (role)
    {
        case PBSColorScheme::InactiveText: return _brushes.fg[1];
        case PBSColorScheme::ActiveText  : return _brushes.fg[2];
        case PBSColorScheme::LinkText    : return _brushes.fg[3];
        case PBSColorScheme::VisitedText : return _brushes.fg[4];
        case PBSColorScheme::NegativeText: return _brushes.fg[5];
        case PBSColorScheme::NeutralText : return _brushes.fg[6];
        case PBSColorScheme::PositiveText: return _brushes.fg[7];
        default: return _brushes.fg[0];
    }
}

QBrush PBSColorSchemePrivate::decoration(PBSColorScheme::DecorationRole role) const
{
    switch (role)
    {
        case PBSColorScheme::FocusColor: return _brushes.deco[1];
        default: return _brushes.deco[0];
    }
}

qreal PBSColorSchemePrivate::contrast() const
{
    return _contrast;
}

PBSColorScheme::PBSColorScheme(const PBSColorScheme &other) : d(other.d)
{
}

PBSColorScheme &PBSColorScheme::operator=(const PBSColorScheme &other)
{
    d = other.d;
    return *this;
}

PBSColorScheme::~PBSColorScheme()
{
}

PBSColorScheme::PBSColorScheme(QPalette::ColorGroup state, PBSColorConfig &config, ColorSet set)
{
    switch (set) {
        case Window:
            d = new PBSColorSchemePrivate(config, state, "Colors:Window", defaultWindowColors);
            break;
        case Button:
            d = new PBSColorSchemePrivate(config, state, "Colors:Button", defaultButtonColors);
            break;
        case Tooltip:
            d = new PBSColorSchemePrivate(config, state, "Colors:Tooltip", defaultTooltipColors);
            break;
        case Complementary:
            d = new PBSColorSchemePrivate(config, state, "Colors:Complementary", defaultComplementaryColors);
            break;
        case Selection:
        {
            bool inactiveSelectionEffect = config.readBool("ColorEffects:Inactive", "ChangeSelectionColor",
                  config.readBool("ColorEffects:Inactive", "Enable", true));
            if (state == QPalette::Active || (state == QPalette::Inactive && !inactiveSelectionEffect))
            {
                d = new PBSColorSchemePrivate(config, state, "Colors:Selection", defaultSelectionColors);
            }
            else if (state == QPalette::Inactive)
            {
                PBSColorScheme c(QPalette::Active, config, Selection);
                QBrush br = c.background();
                d = new PBSColorSchemePrivate(config, state, "Colors:Window", defaultWindowColors, br);
            }
            else
            {
                d = new PBSColorSchemePrivate(config, state, "Colors:Window", defaultWindowColors);
            }
        }
        break;
        default:
            d = new PBSColorSchemePrivate(config, state, "Colors:View", defaultViewColors);
            break;
    }
}

int PBSColorScheme::contrast(PBSColorConfig c)
{
    return c.readInt("KDE", "contrast", 7);
}

qreal PBSColorScheme::contrastF(PBSColorConfig &c)
{
    return 0.1 * c.readInt("KDE", "contrast", 7);
}

QBrush PBSColorScheme::background(BackgroundRole role) const
{
    return d->background(role);
}

QBrush PBSColorScheme::foreground(ForegroundRole role) const
{
    return d->foreground(role);
}

QBrush PBSColorScheme::decoration(DecorationRole role) const
{
    return d->decoration(role);
}

QColor PBSColorScheme::shade(PBSColorConfig &c, ShadeRole role) const
{
    return shade(c, background().color(), role, d->contrast());
}

QColor PBSColorScheme::shade(PBSColorConfig &c, const QColor &color, ShadeRole role)
{
    return shade(c, color, role, PBSColorScheme::contrastF(c));
}

QColor PBSColorScheme::shade(PBSColorConfig &c, const QColor &color, ShadeRole role, qreal contrast, qreal chromaAdjust)
{
    contrast = (1.0 > contrast ? (-1.0 < contrast ? contrast : -1.0) : 1.0);
    qreal y = KColorUtils::luma(color), yi = 1.0 - y;

    if (y < 0.006) {
        switch (role) {
        case PBSColorScheme::LightShade:
            return KColorUtils::shade(color, 0.05 + 0.95 * contrast, chromaAdjust);
        case PBSColorScheme::MidShade:
            return KColorUtils::shade(color, 0.01 + 0.20 * contrast, chromaAdjust);
        case PBSColorScheme::DarkShade:
            return KColorUtils::shade(color, 0.02 + 0.40 * contrast, chromaAdjust);
        default:
            return KColorUtils::shade(color, 0.03 + 0.60 * contrast, chromaAdjust);
        }
    }

    // handle very light colors (base, midlight, light == mid, dark, shadow)
    if (y > 0.93) {
        switch (role) {
        case PBSColorScheme::MidlightShade:
            return KColorUtils::shade(color, -0.02 - 0.20 * contrast, chromaAdjust);
        case PBSColorScheme::DarkShade:
            return KColorUtils::shade(color, -0.06 - 0.60 * contrast, chromaAdjust);
        case PBSColorScheme::ShadowShade:
            return KColorUtils::shade(color, -0.10 - 0.90 * contrast, chromaAdjust);
        default:
            return KColorUtils::shade(color, -0.04 - 0.40 * contrast, chromaAdjust);
        }
    }

    // handle everything else
    qreal lightAmount = (0.05 + y * 0.55) * (0.25 + contrast * 0.75);
    qreal darkAmount = (- y) * (0.55 + contrast * 0.35);
    switch (role) {
    case PBSColorScheme::LightShade:
        return KColorUtils::shade(color, lightAmount, chromaAdjust);
    case PBSColorScheme::MidlightShade:
        return KColorUtils::shade(color, (0.15 + 0.35 * yi) * lightAmount, chromaAdjust);
    case PBSColorScheme::MidShade:
        return KColorUtils::shade(color, (0.35 + 0.15 * y) * darkAmount, chromaAdjust);
    case PBSColorScheme::DarkShade:
        return KColorUtils::shade(color, darkAmount, chromaAdjust);
    default:
        return KColorUtils::darken(KColorUtils::shade(color, darkAmount, chromaAdjust), 0.5 + 0.3 * y);
    }
}

void PBSColorScheme::adjustBackground(PBSColorConfig config,
                             QPalette &palette,
                             BackgroundRole newRole,
                             QPalette::ColorRole color,
                             ColorSet set)
{
    palette.setBrush(QPalette::Active,   color, PBSColorScheme(QPalette::Active,   config, set).background(newRole));
    palette.setBrush(QPalette::Inactive, color, PBSColorScheme(QPalette::Inactive, config, set).background(newRole));
    palette.setBrush(QPalette::Disabled, color, PBSColorScheme(QPalette::Disabled, config, set).background(newRole));
}

void PBSColorScheme::adjustForeground(PBSColorConfig config,
                             QPalette &palette,
                             ForegroundRole newRole,
                             QPalette::ColorRole color,
                             ColorSet set)
{
    palette.setBrush(QPalette::Active,   color, PBSColorScheme(QPalette::Active,   config, set).foreground(newRole));
    palette.setBrush(QPalette::Inactive, color, PBSColorScheme(QPalette::Inactive, config, set).foreground(newRole));
    palette.setBrush(QPalette::Disabled, color, PBSColorScheme(QPalette::Disabled, config, set).foreground(newRole));
}


QPalette PBSColorScheme::createApplicationPalette(PBSColorConfig &config)
{
    QPalette palette;

    static const QPalette::ColorGroup states[3] = { QPalette::Active, QPalette::Inactive, QPalette::Disabled };

    PBSColorScheme schemeTooltip(QPalette::Active, config, PBSColorScheme::Tooltip);

    for (int i = 0; i < 3; i++)
    {
        QPalette::ColorGroup state = states[i];
        PBSColorScheme schemeView(state, config, PBSColorScheme::View);
        PBSColorScheme schemeWindow(state, config, PBSColorScheme::Window);
        PBSColorScheme schemeButton(state, config, PBSColorScheme::Button);
        PBSColorScheme schemeSelection(state, config, PBSColorScheme::Selection);

        palette.setBrush(state, QPalette::WindowText, schemeWindow.foreground());
        palette.setBrush(state, QPalette::Window, schemeWindow.background());
        palette.setBrush(state, QPalette::Base, schemeView.background());
        palette.setBrush(state, QPalette::Text, schemeView.foreground());
        palette.setBrush(state, QPalette::BrightText, schemeView.foreground());
        palette.setBrush(state, QPalette::Button, schemeButton.background());
        palette.setBrush(state, QPalette::ButtonText, schemeButton.foreground());
        palette.setBrush(state, QPalette::Highlight, schemeSelection.background());
        palette.setBrush(state, QPalette::HighlightedText, schemeSelection.foreground());
        palette.setBrush(state, QPalette::ToolTipBase, schemeTooltip.background());
        palette.setBrush(state, QPalette::ToolTipText, schemeTooltip.foreground());

        palette.setColor(state, QPalette::Light, schemeWindow.shade(config, PBSColorScheme::LightShade));
        palette.setColor(state, QPalette::Midlight, schemeWindow.shade(config, PBSColorScheme::MidlightShade));
        palette.setColor(state, QPalette::Mid, schemeWindow.shade(config, PBSColorScheme::MidShade));
        palette.setColor(state, QPalette::Dark, schemeWindow.shade(config, PBSColorScheme::DarkShade));
        palette.setColor(state, QPalette::Shadow, schemeWindow.shade(config, PBSColorScheme::ShadowShade));

        palette.setBrush(state, QPalette::AlternateBase, schemeView.background(PBSColorScheme::AlternateBackground));
        palette.setBrush(state, QPalette::Link, schemeView.foreground(PBSColorScheme::LinkText));
        palette.setBrush(state, QPalette::LinkVisited, schemeView.foreground(PBSColorScheme::VisitedText));
    }

    return palette;
}

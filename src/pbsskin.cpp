#include "pbsskin.h"
#include <QApplication>
#include <QTimer>
#include <pbscolorscheme.h>
#include <Kvantum.h>

namespace Kvantum {

static inline QColor getFromRGBA(const QString str)
{
  QColor col(str);
  if (str.isEmpty() || !(str.size() == 9 && str.startsWith("#")))
    return col;
  bool ok;
  int alpha = str.right(2).toInt(&ok, 16);
  if (ok)
  {
    QString tmp(str);
    tmp.remove(7, 2);
    col = QColor(tmp);
    col.setAlpha(alpha);
  }
  return col;
}

static inline bool enoughContrast (QColor col1, QColor col2)
{
  if (!col1.isValid() || !col2.isValid()) return false;
  if (qAbs(qGray(col1.rgb()) - qGray(col2.rgb())) < 78)
    return false;
  return true;
}

static inline bool isThemeDir(const QString &path, const QString &themeName)
{
  if (themeName.isEmpty()) return false;
  if (QFile::exists (path + QString("/%1/%1.kvconfig").arg(themeName))
           || QFile::exists (path + QString("/%1/%1.svg").arg(themeName)))
  {
    return true;
  }
  return false;
}

PBSSkin::PBSSkin(bool useDark) : Style(useDark)
{
    xdg_config_home = qApp->applicationDirPath();
}

void PBSSkin::setTheme(const QString &baseThemeName, bool useDark)
{
    QString colorName;
    if (themeSettings_)
    {
      delete themeSettings_;
      themeSettings_ = nullptr;
    }
    if (themeRndr_)
    {
      delete themeRndr_;
      themeRndr_ = nullptr;
    }

    if (!baseThemeName.isNull() && !baseThemeName.isEmpty()
        /* "Default" is reserved by Kvantum Manager for copied default theme */
        && baseThemeName != "Default"
        /* "Kvantum" is reserved for the alternative installation paths */
        && baseThemeName != "Kvantum"
        /* no space in theme name */
        && !(baseThemeName.simplified()).contains (" ")
        /* "#" is reserved by Kvantum Manager as an ending for copied root themes */
        && (!baseThemeName.contains("#")
            || (baseThemeName.length() > 1
                && baseThemeName.indexOf("#") == baseThemeName.size() - 1)))
    {
      QStringList themeMames;
      if (useDark)
      {
        QString name = baseThemeName;
        if (name.endsWith("#"))
          name.chop(1);
        /* give priority to modified themes */
        themeMames.append(name + "Dark#");
        themeMames.append(name + "Dark");
      }
      themeMames.append(baseThemeName);
      for (const QString &themeName : static_cast<const QStringList&>(themeMames))
      {
        QString userConfig, userSvg, temp, lightName;

        if (themeName.length() > 4 && themeName.endsWith("Dark"))
        { // dark theme inside light theme folder
          lightName = themeName.left(themeName.length() - 4);
        }

        temp = QString("%1/Themes/kvAntum/%2/%2.kvconfig")
               .arg(xdg_config_home).arg(themeName);
        if (QFile::exists(temp))
          userConfig = temp;
        temp = QString("%1/Themes/kvAntum/%2/%2.svg")
               .arg(xdg_config_home).arg(themeName);
        if (QFile::exists(temp))
          userSvg = temp;

        temp = QString("%1/Themes/kvAntum/%2/%2.colors")
               .arg(xdg_config_home).arg(themeName);
        if (QFile::exists(temp))
            colorName = temp;

        if (userConfig.isEmpty() && userSvg.isEmpty()
            /* dark themes should be inside valid light directories */
            && isThemeDir(QString("%1/Styles").arg(xdg_config_home), lightName))
        {
          temp = QString("%1/Styles/%2/%3.kvconfig")
                 .arg(xdg_config_home).arg(lightName).arg(themeName);
          if (QFile::exists(temp))
            userConfig = temp;
          temp = QString("%1/Styles/%2/%3.svg")
                 .arg(xdg_config_home).arg(lightName).arg(themeName);
          if (QFile::exists(temp))
            userSvg = temp;
        }

        if (themeName.endsWith("#"))
        {
          if (themeName.length() > 5 && themeName.endsWith("Dark#"))
          { // root dark theme inside root light theme folder
            lightName = themeName.left(themeName.length() - 5);
          }
        }
        /* search in the alternative theme installation paths
           only if there's no such theme in the config folder */
        else if (userConfig.isEmpty() && userSvg.isEmpty()) // copied themes don't come here
        {
          QString homeDir = QDir::homePath();
          temp = QString("%1/.themes/%2/Kvantum/%2.kvconfig")
                 .arg(homeDir).arg(themeName);
          if (QFile::exists(temp))
            userConfig = temp;
          temp = QString("%1/.themes/%2/Kvantum/%2.svg")
                 .arg(homeDir).arg(themeName);
          if (QFile::exists(temp))
            userSvg = temp;

          if (userConfig.isEmpty() && userSvg.isEmpty()
              && isThemeDir(QString("%1/.themes").arg(homeDir), lightName))
          {
            temp = QString("%1/.themes/%2/Kvantum/%3.kvconfig")
                   .arg(homeDir).arg(lightName).arg(themeName);
            if (QFile::exists(temp))
              userConfig = temp;
            temp = QString("%1/.themes/%2/Kvantum/%3.svg")
                   .arg(homeDir).arg(lightName).arg(themeName);
            if (QFile::exists(temp))
              userSvg = temp;
          }

          if (userConfig.isEmpty() && userSvg.isEmpty())
          {
            temp = QString("%1/.local/share/themes/%2/Kvantum/%2.kvconfig")
                   .arg(homeDir).arg(themeName);
            if (QFile::exists(temp))
              userConfig = temp;
            temp = QString("%1/.local/share/themes/%2/Kvantum/%2.svg")
                   .arg(homeDir).arg(themeName);
            if (QFile::exists(temp))
              userSvg = temp;

            if (userConfig.isEmpty() && userSvg.isEmpty()
                && isThemeDir(QString("%1/.local/share/themes").arg(homeDir), lightName))
            {
              temp = QString("%1/.local/share/themes/%2/Kvantum/%3.kvconfig")
                     .arg(homeDir).arg(lightName).arg(themeName);
              if (QFile::exists(temp))
                userConfig = temp;
              temp = QString("%1/.local/share/themes/%2/Kvantum/%3.svg")
                     .arg(homeDir).arg(lightName).arg(themeName);
              if (QFile::exists(temp))
                userSvg = temp;
            }
          }

          /* this can't be about a copied theme anymore */
          if (!userConfig.isEmpty())
            themeSettings_ = new ThemeConfig(userConfig);
          if (!userSvg.isEmpty())
          {
            themeRndr_ = new QSvgRenderer();
            themeRndr_->load(userSvg);
          }
          if (themeSettings_ || themeRndr_)
          {
            setupThemeDeps();
            return;
          }
        }

        /*******************
         ** kvconfig file **
         *******************/
        if (!userConfig.isEmpty())
        { // user theme
          themeSettings_ = new ThemeConfig(userConfig);
        }
        else if (userSvg.isEmpty() // otherwise it's a user theme without config file
                 && !themeName.endsWith("#")) // root theme names can't have the ending "#"
        { // root theme
          temp = QString(DATADIR)
                 + QString("/Kvantum/%1/%1.kvconfig").arg(themeName);
          if (QFile::exists(temp))
            themeSettings_ = new ThemeConfig(temp);
          else if (!isThemeDir(QString(DATADIR) + "/Kvantum", themeName) // svg shouldn't be found
                   && isThemeDir(QString(DATADIR) + "/Kvantum", lightName))
          {
            temp = QString(DATADIR)
                   + QString("/Kvantum/%1/%2.kvconfig").arg(lightName).arg(themeName);
            if (QFile::exists(temp))
              themeSettings_ = new ThemeConfig(temp);
          }

          if (!QFile::exists(temp))
          {
            temp = QString(DATADIR)
                   + QString("/Kvantum/%1/%1.svg").arg(themeName);
            if (!QFile::exists(temp)) // otherwise the checked root theme was just an SVG image
            {
              temp = QString(DATADIR)
                     + QString("/themes/%1/Kvantum/%1.kvconfig").arg(themeName);
              if (QFile::exists(temp))
                themeSettings_ = new ThemeConfig(temp);
            }

            if (!QFile::exists(temp)
                && !isThemeDir(QString(DATADIR) + "/themes", themeName)
                && isThemeDir(QString(DATADIR) + "/themes", lightName))
            {
              temp = QString(DATADIR)
                     + QString("/Kvantum/%1/%2.svg").arg(lightName).arg(themeName);
              if (!QFile::exists(temp))
              {
                temp = QString(DATADIR)
                       + QString("/themes/%1/Kvantum/%2.kvconfig").arg(lightName).arg(themeName);
                if (QFile::exists(temp))
                  themeSettings_ = new ThemeConfig(temp);
              }
            }
          }
        }

        /***************
         ** SVG image **
         ***************/
        if (!userSvg.isEmpty())
        { // user theme
          themeRndr_ = new QSvgRenderer();
          themeRndr_->load(userSvg);
        }
        else
        {
          if (!themeName.endsWith("#"))
          {
            if (userConfig.isEmpty()) // otherwise it's a user theme without SVG image
            { // root theme
              temp = QString(DATADIR)
                     + QString("/Kvantum/%1/%1.svg").arg(themeName);
              if (QFile::exists(temp))
              {
                themeRndr_ = new QSvgRenderer();
                themeRndr_->load(temp);
              }
              else if (!isThemeDir(QString(DATADIR) + "/Kvantum", themeName) // config shouldn't be found
                       && isThemeDir(QString(DATADIR) + "/Kvantum", lightName))
              {
                temp = QString(DATADIR)
                       + QString("/Kvantum/%1/%2.svg").arg(lightName).arg(themeName);
                if (QFile::exists(temp))
                {
                  themeRndr_ = new QSvgRenderer();
                  themeRndr_->load(temp);
                }
              }

              if (!QFile::exists(temp))
              {
                temp = QString(DATADIR)
                       + QString("/Kvantum/%1/%1.kvconfig").arg(themeName);
                if (!QFile::exists(temp)) // otherwise the checked root theme was just a config file
                {
                  temp = QString(DATADIR)
                         + QString("/themes/%1/Kvantum/%1.svg").arg(themeName);
                  if (QFile::exists(temp))
                  {
                    themeRndr_ = new QSvgRenderer();
                    themeRndr_->load(temp);
                  }
                }

                if (!QFile::exists(temp)
                    && !isThemeDir(QString(DATADIR) + "/themes", themeName)
                    && isThemeDir(QString(DATADIR) + "/themes", lightName))
                {
                  temp = QString(DATADIR)
                         + QString("/Kvantum/%1/%2.kvconfig").arg(lightName).arg(themeName);
                  if (!QFile::exists(temp))
                  {
                    temp = QString(DATADIR)
                           + QString("/themes/%1/Kvantum/%2.svg").arg(lightName).arg(themeName);
                    if (QFile::exists(temp))
                    {
                      themeRndr_ = new QSvgRenderer();
                      themeRndr_->load(temp);
                    }
                  }
                }
              }
            }
          }
          else if (!userConfig.isEmpty()) // otherwise, the folder has been emptied manually
          { // find the SVG image of the root theme, of which this is a copy
            QString _themeName = themeName.left(themeName.length() - 1);
            if (!_themeName.isEmpty() && !_themeName.contains("#"))
            {
              temp = QString(DATADIR)
                     + QString("/Kvantum/%1/%1.svg").arg(_themeName);
              if (QFile::exists(temp))
              {
                themeRndr_ = new QSvgRenderer();
                themeRndr_->load(temp);
              }
              else if (isThemeDir(QString(DATADIR) + "/Kvantum", lightName))
              {
                temp = QString(DATADIR)
                       + QString("/Kvantum/%1/%2.svg").arg(lightName).arg(_themeName);
                if (QFile::exists(temp))
                {
                  themeRndr_ = new QSvgRenderer();
                  themeRndr_->load(temp);
                }
              }

              if (!QFile::exists(temp))
              {
                temp = QString(DATADIR)
                       + QString("/Kvantum/%1/%1.kvconfig").arg(_themeName);
                if (!QFile::exists(temp)) // otherwise the checked root theme was just a config file
                {
                  temp = QString(DATADIR)
                         + QString("/themes/%1/Kvantum/%1.svg").arg(_themeName);
                  if (QFile::exists(temp))
                  {
                    themeRndr_ = new QSvgRenderer();
                    themeRndr_->load(temp);
                  }
                }

                if (!QFile::exists(temp)
                    && !isThemeDir(QString(DATADIR) + "/themes", _themeName)
                    && isThemeDir(QString(DATADIR) + "/themes", lightName))
                {
                  temp = QString(DATADIR)
                         + QString("/Kvantum/%1/%2.kvconfig").arg(lightName).arg(_themeName);
                  if (!QFile::exists(temp))
                  {
                    temp = QString(DATADIR)
                           + QString("/themes/%1/Kvantum/%2.svg").arg(lightName).arg(_themeName);
                    if (QFile::exists(temp))
                    {
                      themeRndr_ = new QSvgRenderer();
                      themeRndr_->load(temp);
                    }
                  }
                }
              }
            }
          }
        }
        if (themeRndr_)
          break;
      }
    }

    setupThemeDeps();

    tspec_ = settings_->getThemeSpec();
    hspec_ = settings_->getHacksSpec();
    cspec_ = settings_->getColorSpec();

    if (!colorName.isEmpty())
    {
        PBSColorConfig c(colorName);
        qApp->setPalette(PBSColorScheme::createApplicationPalette(static_cast<PBSColorConfig&>(c)));
    }

    installColorScheme();
    initTheme();
}

void PBSSkin::initTheme()
{
    connect(progressTimer_, &QTimer::timeout, this, &Style::advanceProgressbar);

    if (tspec_.animate_states)
    {
      opacityTimer_ = new QTimer(this);
      opacityTimerOut_ = new QTimer(this);
      connect(opacityTimer_, &QTimer::timeout, this, &Style::setAnimationOpacity);
      connect(opacityTimerOut_, &QTimer::timeout, this, &Style::setAnimationOpacityOut);
    }

    itsShortcutHandler_ = nullptr;
    if (tspec_.alt_mnemonic)
      itsShortcutHandler_ = new ShortcutHandler(this);

    // decide, once for all, about active indicator, connecting active tabs to others and using floating tabs
    hasActiveIndicator_ = joinedActiveTab_ = joinedActiveFloatingTab_ = hasFloatingTabs_ = false;
    if (themeRndr_ && themeRndr_->isValid())
    {
      if (themeRndr_->elementExists(getIndicatorSpec("Tab").element+"-close-toggled"))
        hasActiveIndicator_ = true;
      if (themeRndr_->elementExists("floating-"+getInteriorSpec("Tab").element+"-normal"))
        hasFloatingTabs_ = true;
      if (tspec_.joined_inactive_tabs)
      {
        QString sepName = getFrameSpec("Tab").element + "-separator";
        if (themeRndr_->elementExists(sepName+"-normal")
            || themeRndr_->elementExists(sepName+"-toggled"))
        {
          joinedActiveTab_ = true;
        }
        if (hasFloatingTabs_)
        {
          sepName = "floating-"+sepName;
          if (themeRndr_->elementExists(sepName+"-normal")
              || themeRndr_->elementExists(sepName+"-toggled"))
          {
            joinedActiveFloatingTab_ = true;
          }
        }
      }
    }

    // decide about view-item colors once for all
    hasInactiveSelItemCol_ = toggledItemHasContrast_ = false;
    const label_spec lspec = getLabelSpec("ItemView");
    QColor toggleInactiveCol = getFromRGBA(lspec.toggleInactiveColor);
    if (toggleInactiveCol.isValid())
    {
      QColor toggleActiveCol = getFromRGBA(lspec.toggleColor);
      if (toggleActiveCol.isValid() && toggleActiveCol != toggleInactiveCol)
      {
        hasInactiveSelItemCol_ = true;
        if (enoughContrast(toggleActiveCol, getFromRGBA(lspec.pressColor)))
          toggledItemHasContrast_ = true;
      }
    }

    itsWindowManager_ = nullptr;
    blurHelper_ = nullptr;

    if (tspec_.x11drag && tspec_.isX11)
    {
      itsWindowManager_ = new WindowManager(this, tspec_.x11drag/*, tspec_.isX11*/);
      itsWindowManager_->initialize();
    }

    if (tspec_.blurring)
    {
      getShadow("Menu", getMenuMargin(true), getMenuMargin(false));
      const frame_spec fspec = getFrameSpec("ToolTip");
      int thickness = qMax(qMax(fspec.top,fspec.bottom), qMax(fspec.left,fspec.right));
      thickness += tspec_.tooltip_shadow_depth;
      QList<int> tooltipS = getShadow("ToolTip", thickness);
      blurHelper_ = new BlurHelper(this,menuShadow_,tooltipS);
    }
}

void PBSSkin::installColorScheme()
{
}

}


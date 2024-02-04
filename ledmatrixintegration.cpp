// Copyright (c) 2024 Erwan MATHIEU
// This software is released under the terms of the GPLv2

#include "ledmatrixintegration.h"

#include "ledmatrixbackingstore.h"
#include "ledmatrixscreen.h"

#include <QRegularExpression>
#include <QtGui/private/qguiapplication_p.h>
#include <qpa/qplatformnativeinterface.h>
#include <qpa/qplatformwindow.h>

#if QT_CONFIG(fontconfig)
# include <QtGui/private/qgenericunixfontdatabase_p.h>
#endif

#if QT_CONFIG(freetype)
# include <QtGui/private/qfontengine_ft_p.h>
# include <QtGui/private/qfreetypefontdatabase_p.h>
#endif

#include <QtGui/private/qgenericunixeventdispatcher_p.h>

using namespace Qt::StringLiterals;

LedMatrixIntegration::LedMatrixIntegration(const QStringList& parameters)
{
    parseOptions(parameters);

    primary_screen_ =
        new LedMatrixScreen(options_.driver_options.rows, options_.driver_options.cols);

    QWindowSystemInterface::handleScreenAdded(primary_screen_);
}

LedMatrixIntegration::~LedMatrixIntegration()
{
    QWindowSystemInterface::handleScreenRemoved(primary_screen_);
    delete font_database_;
}

bool LedMatrixIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch(cap)
    {
        case ThreadedPixmaps: return true;
        case MultipleWindows: return true;
        case RhiBasedRendering: return false;
        default: return QPlatformIntegration::hasCapability(cap);
    }
}

void LedMatrixIntegration::parseOptions(const QStringList& paramList)
{
    const QString paramEnableFonts("enable-fonts");
    const QRegularExpression regexpGpioMapping("^gpio-mapping=([a-z-]+)$");
    const QRegularExpression regexpRows("^rows=([0-9]+)$");
    const QRegularExpression regexpCols("^cols=([0-9]+)$");
    const QRegularExpression regexpChain("^chain=([0-9]+)$");
    const QRegularExpression regexpParallel("^parallel=([123])$");
    const QRegularExpression regexpMultiplexing("^multiplexing=([0-9]{1,2})$");
    const QRegularExpression regexpPixelMapper(
        "((^pixel-mapper=|[+])(U-mapper|V-mapper|Mirror=[HV]|Rotate=[0-9]+))+$");
    const QRegularExpression regexpPwmBits("^pwm-bits=([0-9]{1,2})$");
    const QRegularExpression regexpBrightness("^brightness=([0-9]+)$");
    const QRegularExpression regexpScanMode("^scan-mode=([01])$");
    const QRegularExpression regexpRowAddrType("^row-addr-type=([0-4])$");
    const QString paramShowRefresh("show-refresh");
    const QRegularExpression regexpLimitRefresh("^limit-refresh=([0-9]+)$");
    const QString paramInverse("inverse");
    const QRegularExpression regexpRgbSequence("^rgb-sequence=([RGB]{3})$");
    const QRegularExpression regexpPwmLsbNanoseconds("^pwm-lsb-nanoseconds=([0-9]+)$");
    const QRegularExpression regexpPwmDitherBits("^pwm-dither-bits=([012])$");
    const QString paramDisableHardwarePulse("no-hardware-pulse");
    const QRegularExpression regexpPanelType("^panel_type=(FM6126A|FM6127)$");
    const QRegularExpression regexpSlowdownGpio("^slowdown-gpio=([0-4])$");
    const QString paramDaemon("daemon");
    const QRegularExpression regexpDropPrivileges("^no-drop-privs=(-1|0|1)$");
    const QRegularExpression regexpDropPrivilegesUser("^drop-priv-user=([a-zA-Z0-9-_]+)$");
    const QRegularExpression regexpDropPrivilegesGroup("^drop-priv-group=([a-zA-Z0-9-_]+)$");

    QRegularExpressionMatch regexpMatch;
    for(const QString& param: paramList)
    {
        if(param == paramEnableFonts)
        {
            options_.enable_fonts = true;
        }
        else if((regexpMatch = regexpGpioMapping.match(param)).hasMatch())
        {
            hardware_mapping_ = regexpMatch.captured(1).toStdString();
            options_.driver_options.hardware_mapping = hardware_mapping_.c_str();
        }
        else if((regexpMatch = regexpRows.match(param)).hasMatch())
        {
            options_.driver_options.rows = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpCols.match(param)).hasMatch())
        {
            options_.driver_options.cols = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpChain.match(param)).hasMatch())
        {
            options_.driver_options.chain_length = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpParallel.match(param)).hasMatch())
        {
            options_.driver_options.parallel = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpMultiplexing.match(param)).hasMatch())
        {
            options_.driver_options.multiplexing = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpPixelMapper.match(param)).hasMatch())
        {
            // We don't use the original set of separators because they are recognized by Qt
            // as delimitors for plugin settings
            QString pixel_mapper =
                regexpMatch.captured(0).mid(13).replace('+', ';').replace('=', ':');
            pixel_mapper_ = pixel_mapper.toStdString();
            options_.driver_options.pixel_mapper_config = pixel_mapper_.c_str();
        }
        else if((regexpMatch = regexpPwmBits.match(param)).hasMatch())
        {
            options_.driver_options.pwm_bits = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpBrightness.match(param)).hasMatch())
        {
            options_.driver_options.brightness = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpScanMode.match(param)).hasMatch())
        {
            options_.driver_options.scan_mode = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpRowAddrType.match(param)).hasMatch())
        {
            options_.driver_options.row_address_type = regexpMatch.captured(1).toInt();
        }
        else if(param == paramShowRefresh)
        {
            options_.driver_options.show_refresh_rate = true;
        }
        else if((regexpMatch = regexpLimitRefresh.match(param)).hasMatch())
        {
            options_.driver_options.limit_refresh_rate_hz = regexpMatch.captured(1).toInt();
        }
        else if(param == paramInverse)
        {
            options_.driver_options.inverse_colors = true;
        }
        else if((regexpMatch = regexpRgbSequence.match(param)).hasMatch())
        {
            rgb_sequence_ = regexpMatch.captured(1).toStdString();
            options_.driver_options.led_rgb_sequence = rgb_sequence_.c_str();
        }
        else if((regexpMatch = regexpPwmLsbNanoseconds.match(param)).hasMatch())
        {
            options_.driver_options.pwm_lsb_nanoseconds = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpPwmDitherBits.match(param)).hasMatch())
        {
            options_.driver_options.pwm_dither_bits = regexpMatch.captured(1).toInt();
        }
        else if(param == paramDisableHardwarePulse)
        {
            options_.driver_options.disable_hardware_pulsing = true;
        }
        else if((regexpMatch = regexpPanelType.match(param)).hasMatch())
        {
            panel_type_ = regexpMatch.captured(1).toStdString();
            options_.driver_options.panel_type = panel_type_.c_str();
        }
        else if((regexpMatch = regexpSlowdownGpio.match(param)).hasMatch())
        {
            options_.runtime_options.gpio_slowdown = regexpMatch.captured(1).toInt();
        }
        else if(param == paramDaemon)
        {
            options_.runtime_options.daemon = true;
        }
        else if((regexpMatch = regexpDropPrivileges.match(param)).hasMatch())
        {
            options_.runtime_options.drop_privileges = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpDropPrivilegesUser.match(param)).hasMatch())
        {
            drop_privileges_user_ = regexpMatch.captured(1).toStdString();
            options_.runtime_options.drop_priv_user = drop_privileges_user_.c_str();
        }
        else if((regexpMatch = regexpDropPrivilegesGroup.match(param)).hasMatch())
        {
            drop_privileges_group_ = regexpMatch.captured(1).toStdString();
            options_.runtime_options.drop_priv_group = drop_privileges_group_.c_str();
        }
        else
        {
            qWarning() << "Invalid platform parameter" << param;
        }
    }
}

// Dummy font database that does not scan the fonts directory to be
// used for command line tools like qmlplugindump that do not create windows
// unless DebugBackingStore is activated.
class DummyFontDatabase : public QPlatformFontDatabase
{
    public:
        virtual void populateFontDatabase() override {}
};

QPlatformFontDatabase* LedMatrixIntegration::fontDatabase() const
{
    if(!font_database_ && (options_.enable_fonts))
    {
        if(!font_database_)
        {
#if QT_CONFIG(fontconfig)
            font_database_ = new QGenericUnixFontDatabase;
#else
            font_database_ = QPlatformIntegration::fontDatabase();
#endif
        }
    }

    if(!font_database_)
    {
        font_database_ = new DummyFontDatabase;
    }

    return font_database_;
}

QPlatformWindow* LedMatrixIntegration::createPlatformWindow(QWindow* window) const
{
    Q_UNUSED(window);

    QPlatformWindow* w = new QPlatformWindow(window);
    w->requestActivateWindow();
    return w;
}

QPlatformBackingStore* LedMatrixIntegration::createPlatformBackingStore(QWindow* window) const
{
    return new LedMatrixBackingStore(window, options_.driver_options, options_.runtime_options);
}

QAbstractEventDispatcher* LedMatrixIntegration::createEventDispatcher() const
{
    return createUnixEventDispatcher();
}

QPlatformNativeInterface* LedMatrixIntegration::nativeInterface() const
{
    if(!native_interface_)
    {
        native_interface_.reset(new QPlatformNativeInterface);
    }

    return native_interface_.get();
}

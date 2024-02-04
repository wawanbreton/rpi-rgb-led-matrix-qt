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

LedMatrixIntegration::LedMatrixIntegration(const QStringList& parameters) :
    options_(parseOptions(parameters)),
    primary_screen_(new LedMatrixScreen(options_.driver_options.rows, options_.driver_options.cols))
{
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

LedMatrixIntegration::Options LedMatrixIntegration::parseOptions(const QStringList& paramList)
{
    Options options;
    options.driver_options.hardware_mapping = "adafruit-hat";
    options.driver_options.rows = 32;
    options.driver_options.cols = 32;
    options.driver_options.chain_length = 1;
    options.driver_options.parallel = 1;
    options.driver_options.multiplexing = 0;
    options.driver_options.pixel_mapper_config = "";
    options.driver_options.brightness = 100;

    QRegularExpression regexpGpioMapping("gpio-mapping=([a-z-]+)");
    QRegularExpression regexpRows("rows=([0-9]+)");
    QRegularExpression regexpCols("cols=([0-9]+)");
    QRegularExpression regexpChain("chain=([0-9]+)");
    QRegularExpression regexpParallel("parallel=([123])");
    QRegularExpression regexpMultiplexing("multiplexing=([0-9]{1,2})");
    QRegularExpression regexpPixelMapper("((^|[+])(U-mapper|V-mapper|Mirror=[HV]|Rotate=[0-9]+))+");
    QRegularExpression regexpBrightness("brightness=([0-9]+)");

    QRegularExpressionMatch regexpMatch;
    for(const QString& param: paramList)
    {
        if(param == "enable_fonts"_L1)
        {
            options.flags |= Option::EnableFonts;
        }
        else if(param == "freetype"_L1)
        {
            options.flags |= Option::FreeTypeFontDatabase;
        }
        else if(param == "fontconfig"_L1)
        {
            options.flags |= Option::FontconfigDatabase;
        }
        else if((regexpMatch = regexpGpioMapping.match(param)).hasMatch())
        {
            hardware_mapping_ = regexpMatch.captured(1).toStdString();
            options.driver_options.hardware_mapping = hardware_mapping_.c_str();
        }
        else if((regexpMatch = regexpRows.match(param)).hasMatch())
        {
            options.driver_options.rows = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpCols.match(param)).hasMatch())
        {
            options.driver_options.cols = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpChain.match(param)).hasMatch())
        {
            options.driver_options.chain_length = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpParallel.match(param)).hasMatch())
        {
            options.driver_options.parallel = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpMultiplexing.match(param)).hasMatch())
        {
            options.driver_options.multiplexing = regexpMatch.captured(1).toInt();
        }
        else if((regexpMatch = regexpPixelMapper.match(param)).hasMatch())
        {
            pixel_mapper_ = regexpMatch.captured(1).toStdString();
            options.driver_options.pixel_mapper_config = pixel_mapper_.c_str();
        }
        else if((regexpMatch = regexpBrightness.match(param)).hasMatch())
        {
            options.driver_options.brightness = regexpMatch.captured(1).toInt();
        }
        else
        {
            qWarning() << "Unrecognized platform parameter" << param;
        }
    }

    return options;
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
    if(!font_database_ && (options_.flags.testFlag(Option::EnableFonts)))
    {
        if(!font_database_)
        {
#if QT_CONFIG(fontconfig)
            font_database_ = new QGenericUnixFontDatabase;
#else
            m_fontDatabase = QPlatformIntegration::fontDatabase();
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
    return new LedMatrixBackingStore(window, options_.driver_options);
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

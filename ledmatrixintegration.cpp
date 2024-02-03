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
    primary_screen_(new LedMatrixScreen(options_.screen_size))
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

    QRegularExpression regexpLedCols("led-cols=([0-9]+)");
    QRegularExpression regexpLedRows("led-rows=([0-9]+)");
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
        else if((regexpMatch = regexpLedCols.match(param)).hasMatch())
        {
            options.screen_size.setWidth(regexpMatch.captured(1).toInt());
        }
        else if((regexpMatch = regexpLedRows.match(param)).hasMatch())
        {
            options.screen_size.setHeight(regexpMatch.captured(1).toInt());
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
    return new LedMatrixBackingStore(window);
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

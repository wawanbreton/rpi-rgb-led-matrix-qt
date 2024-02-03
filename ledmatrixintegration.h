// Copyright (c) 2024 Erwan MATHIEU
// This software is released under the terms of the GPLv2

#pragma once

#include <QScopedPointer>
#include <qpa/qplatformintegration.h>
#include <qpa/qplatformscreen.h>

class LedMatrixScreen;

class LedMatrixIntegration : public QPlatformIntegration
{
    public:
        explicit LedMatrixIntegration(const QStringList& parameters);
        ~LedMatrixIntegration();

        bool hasCapability(QPlatformIntegration::Capability cap) const override;
        QPlatformFontDatabase* fontDatabase() const override;

        QPlatformWindow* createPlatformWindow(QWindow* window) const override;
        QPlatformBackingStore* createPlatformBackingStore(QWindow* window) const override;
        QAbstractEventDispatcher* createEventDispatcher() const override;

        QPlatformNativeInterface* nativeInterface() const override;

    private:
        enum class Option
        {
            None = 0x0,
            EnableFonts = 0x1,
            FreeTypeFontDatabase = 0x2,
            FontconfigDatabase = 0x4
        };

        Q_DECLARE_FLAGS(OptionFlags, Option);

        struct Options
        {
                OptionFlags flags{Option::None};
                QSize screen_size{32, 32};
        };

    private:
        static Options parseOptions(const QStringList& paramList);

    private:
        mutable QPlatformFontDatabase* font_database_{nullptr};
        mutable QScopedPointer<QPlatformNativeInterface> native_interface_;
        const Options options_;
        LedMatrixScreen* primary_screen_{nullptr};
};

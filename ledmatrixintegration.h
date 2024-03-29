// Copyright (c) 2024 Erwan MATHIEU
// This software is released under the terms of the GPLv2

#pragma once

#include <QScopedPointer>
#include <led-matrix.h>
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
        struct Options
        {
                bool enable_fonts{false};
                rgb_matrix::RGBMatrix::Options driver_options;
                rgb_matrix::RuntimeOptions runtime_options;
        };

    private:
        void parseOptions(const QStringList& paramList);

    private:
        mutable QPlatformFontDatabase* font_database_{nullptr};
        mutable QScopedPointer<QPlatformNativeInterface> native_interface_;
        Options options_;
        LedMatrixScreen* primary_screen_{nullptr};

        // Store some config variable strings to make sure the c_str pointers remains valid
        std::string hardware_mapping_;
        std::string pixel_mapper_;
        std::string rgb_sequence_;
        std::string panel_type_;
        std::string drop_privileges_user_;
        std::string drop_privileges_group_;
};

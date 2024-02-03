// Copyright (c) 2024 Erwan MATHIEU
// This software is released under the terms of the GPLv2

#pragma once

#include <qpa/qplatformintegrationplugin.h>

class RpiRgbLedMatrixQtPlugin : public QPlatformIntegrationPlugin
{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPA.QPlatformIntegrationFactoryInterface.5.3" FILE
                              "plugin-metadata.json")

    public:
        QPlatformIntegration* create(const QString&, const QStringList&) override;
};

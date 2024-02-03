// Copyright (c) 2024 Erwan MATHIEU
// This software is released under the terms of the GPLv2

#include "rpiggbledmatrixqtplugin.h"

#include "ledmatrixintegration.h"

using namespace Qt::StringLiterals;

QPlatformIntegration* RpiRgbLedMatrixQtPlugin::create(const QString& system,
                                                      const QStringList& paramList)
{
    if(!system.compare("rpi-rgb-led-matrix"_L1, Qt::CaseInsensitive))
    {
        return new LedMatrixIntegration(paramList);
    }

    return nullptr;
}

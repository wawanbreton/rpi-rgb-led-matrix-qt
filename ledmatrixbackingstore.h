// Copyright (c) 2024 Erwan MATHIEU
// This software is released under the terms of the GPLv2

#pragma once

#include "driveroptions.h"

#include <QImage>
#include <qpa/qplatformbackingstore.h>

namespace rgb_matrix
{
    class RGBMatrix;
}

class LedMatrixBackingStore : public QPlatformBackingStore
{
    public:
        LedMatrixBackingStore(QWindow* window, const DriverOptions& driver_options);
        ~LedMatrixBackingStore();

        QPaintDevice* paintDevice() override;
        void flush(QWindow* window, const QRegion& region, const QPoint& offset) override;
        void resize(const QSize& size, const QRegion& staticContents) override;

    private:
        const DriverOptions options_;
        QImage image_;
        rgb_matrix::RGBMatrix* matrix_{nullptr};
};

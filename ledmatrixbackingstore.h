// Copyright (c) 2024 Erwan MATHIEU
// This software is released under the terms of the GPLv2

#pragma once

#include <QImage>
#include <led-matrix.h>
#include <qpa/qplatformbackingstore.h>

class LedMatrixBackingStore : public QPlatformBackingStore
{
    public:
        LedMatrixBackingStore(QWindow* window,
                              const rgb_matrix::RGBMatrix::Options& driver_options);
        ~LedMatrixBackingStore();

        QPaintDevice* paintDevice() override;
        void flush(QWindow* window, const QRegion& region, const QPoint& offset) override;
        void resize(const QSize& size, const QRegion& staticContents) override;

    private:
        const rgb_matrix::RGBMatrix::Options options_;
        QImage image_;
        rgb_matrix::RGBMatrix* matrix_{nullptr};
};

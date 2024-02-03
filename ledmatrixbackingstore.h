// Copyright (c) 2024 Erwan MATHIEU
// This software is released under the terms of the GPLv2

#pragma once

#include <QImage>
#include <qpa/qplatformbackingstore.h>

class LedMatrixBackingStore : public QPlatformBackingStore
{
    public:
        LedMatrixBackingStore(QWindow* window);
        ~LedMatrixBackingStore();

        QPaintDevice* paintDevice() override;
        void flush(QWindow* window, const QRegion& region, const QPoint& offset) override;
        void resize(const QSize& size, const QRegion& staticContents) override;

    private:
        QImage image_;
};

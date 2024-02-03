// Copyright (c) 2024 Erwan MATHIEU
// This software is released under the terms of the GPLv2

#pragma once

#include <qpa/qplatformscreen.h>

class LedMatrixScreen : public QPlatformScreen
{
    public:
        explicit LedMatrixScreen(const QSize& size) :
            size_(size)
        {
        }

        QRect geometry() const override { return QRect(QPoint(0, 0), size_); }
        int depth() const override { return 32; }
        QImage::Format format() const override { return QImage::Format_ARGB32_Premultiplied; }

    private:
        QSize size_;
};

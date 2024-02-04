// Copyright (c) 2024 Erwan MATHIEU
// This software is released under the terms of the GPLv2

#pragma once

#include <qpa/qplatformscreen.h>

class LedMatrixScreen : public QPlatformScreen
{
    public:
        explicit LedMatrixScreen(int rows, int cols) :
            rows_(rows),
            cols_(cols)
        {
        }

        QRect geometry() const override { return QRect(0, 0, cols_, rows_); }
        int depth() const override { return 32; }
        QImage::Format format() const override { return QImage::Format_ARGB32_Premultiplied; }

    private:
        const int rows_;
        const int cols_;
};

// Copyright (c) 2024 Erwan MATHIEU
// This software is released under the terms of the GPLv2

#include "ledmatrixbackingstore.h"

#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <led-matrix.h>
#include <qpa/qplatformscreen.h>

using namespace Qt::StringLiterals;

LedMatrixBackingStore::LedMatrixBackingStore(QWindow* window,
                                             const rgb_matrix::RGBMatrix::Options& driver_options) :
    QPlatformBackingStore(window),
    options_(driver_options)
{
    rgb_matrix::RuntimeOptions runtime_options;
    // runtime.drop_privileges = -1; // Need this otherwise the touchscreen doesn't work
    // runtime.gpio_slowdown = 4;
    matrix_ = rgb_matrix::CreateMatrixFromOptions(driver_options, runtime_options);
    if(!matrix_)
    {
        qWarning("could not create matrix");
    }
}

LedMatrixBackingStore::~LedMatrixBackingStore() {}

QPaintDevice* LedMatrixBackingStore::paintDevice()
{
    return &image_;
}

void LedMatrixBackingStore::flush(QWindow* window, const QRegion& region, const QPoint& offset)
{
    Q_UNUSED(window);
    Q_UNUSED(offset);

    if(matrix_)
    {
        for(auto iterator = region.begin(); iterator != region.end(); ++iterator)
        {
            const QRect& rect = *iterator;
            for(int x = rect.left(); x <= rect.right() && x < options_.cols; ++x)
            {
                for(int y = rect.top(); y <= rect.bottom() && y < options_.rows; ++y)
                {
                    QRgb pixel = image_.pixel(x, y);
                    matrix_->SetPixel(x, y, qRed(pixel), qGreen(pixel), qBlue(pixel));
                }
            }
        }
    }
}

void LedMatrixBackingStore::resize(const QSize& size, const QRegion&)
{
    QImage::Format format = QGuiApplication::primaryScreen()->handle()->format();
    if(image_.size() != size)
    {
        image_ = QImage(size, format);
    }
}

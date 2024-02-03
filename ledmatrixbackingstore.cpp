// Copyright (c) 2024 Erwan MATHIEU
// This software is released under the terms of the GPLv2

#include "ledmatrixbackingstore.h"

#include "ledmatrixintegration.h"

#include <QDebug>
#include <QGuiApplication>
#include <QScreen>

using namespace Qt::StringLiterals;

LedMatrixBackingStore::LedMatrixBackingStore(QWindow* window) :
    QPlatformBackingStore(window)
{
}

LedMatrixBackingStore::~LedMatrixBackingStore() {}

QPaintDevice* LedMatrixBackingStore::paintDevice()
{
    return &image_;
}

void LedMatrixBackingStore::flush(QWindow* window, const QRegion& region, const QPoint& offset)
{
    Q_UNUSED(window);
    Q_UNUSED(region);
    Q_UNUSED(offset);

    static int c = 0;
    QString filename = QString("/tmp/output%1.png").arg(c++, 4, 10, QChar(u'0'));
    qInfo() << "LedMatrixBackingStore::flush() saving contents to" << filename;
    image_.save(filename);
}

void LedMatrixBackingStore::resize(const QSize& size, const QRegion&)
{
    QImage::Format format = QGuiApplication::primaryScreen()->handle()->format();
    if(image_.size() != size)
    {
        image_ = QImage(size, format);
    }
}

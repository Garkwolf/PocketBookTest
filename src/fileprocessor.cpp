#include "fileprocessor.h"

#include <QMetaObject>
#include <QCoreApplication>
#include <QImage>
#include <QtConcurrent>

#include <fstream>

namespace {
    constexpr char cPackExtension[] = ".packed.barch";
    constexpr char cUnpackExtension[] = ".unpacked.bmp";
    constexpr char cStatusCompressing[] = "Compressing";
    constexpr char cStatusDecompressing[] = "Decompressing";
    constexpr char cStatusCompressSuccess[] = "Compressed successfully";
    constexpr char cStatusDecompressSuccess[] = "Decompressed successfully";
    constexpr char cUnknownFileType[] = "Unknown file type";
}

FileProcessor::FileProcessor(QObject *parent) : QObject(parent)
{
}

bool FileProcessor::loadBMP(const QString &filePath, RawImageData &imageData) {
    QImage image;
    if (!image.load(filePath)) {
        return false;
    }

    imageData.width = image.width();
    imageData.height = image.height();
    imageData.data = std::make_unique<unsigned char[]>(imageData.width * imageData.height);

    for (int y = 0; y < imageData.height; ++y) {
        for (int x = 0; x < imageData.width; ++x) {
            imageData.data[y * imageData.width + x] = image.pixelColor(x, y).red();
        }
    }

    return true;
}

bool FileProcessor::saveBMP(const QString &filePath, const RawImageData &imageData) {
    QImage image(imageData.width, imageData.height, QImage::Format_Indexed8);

    QVector<QRgb> colorTable;
    for (int i = 0; i < 256; ++i) {
        colorTable.push_back(qRgb(i, i, i));
    }
    image.setColorTable(colorTable);

    for (int y = 0; y < imageData.height; ++y) {
        for (int x = 0; x < imageData.width; ++x) {
            image.setPixel(x, y, imageData.data[y * imageData.width + x]);
        }
    }

    return image.save(filePath, "BMP");
}

void FileProcessor::processFile(const QString &filePath)
{
    QtConcurrent::run([this, filePath]() {
        if (filePath.endsWith(".bmp")) {
            emitFileProcessingStatus(filePath, cStatusCompressing);

            try {
                RawImageData image;
                loadBMP(filePath, image);
                GrayImageCompressor compressor;
                compressor.compress(image, filePath.toStdString() + cPackExtension);
                emit fileProcessed(filePath, true, cStatusCompressSuccess);
            } catch (const std::exception &e) {
                emit fileProcessed(filePath, false, e.what());
            }
        } else if (filePath.endsWith(".barch")) {
            emitFileProcessingStatus(filePath, cStatusDecompressing);
            
            try {
                RawImageData image;
                GrayImageCompressor compressor;
                compressor.decompress(image, filePath.toStdString());
                saveBMP(filePath + cUnpackExtension, image);
                emit fileProcessed(filePath, true, cStatusDecompressSuccess);
            } catch (const std::exception &e) {
                emit fileProcessed(filePath, false, e.what());
            }
        } else {
            emit fileProcessed(filePath, false, cUnknownFileType);
        }
    });
}

void FileProcessor::emitFileProcessingStatus(const QString &filePath, const QString &status)
{
    QMetaObject::invokeMethod(QCoreApplication::instance(), [this, filePath, status]() {
        emit fileProcessing(filePath, status);
    });
}

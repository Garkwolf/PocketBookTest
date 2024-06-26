#pragma once

#include <QObject>

#include <GrayImageCompressor.h>

class FileProcessor : public QObject
{
    Q_OBJECT
public:
    explicit FileProcessor(QObject *parent = nullptr);

    Q_INVOKABLE void processFile(const QString &filePath);

signals:
    void fileProcessed(const QString &filePath, bool success, const QString &message);
    void fileProcessing(const QString &filePath, const QString &status);

private:
    bool loadBMP(const QString &filePath, RawImageData &imageData);
    bool saveBMP(const QString &filePath, const RawImageData &imageData);
    void emitFileProcessingStatus(const QString &filePath, const QString &status);
};

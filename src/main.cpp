#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>

#include "filemodel.h"
#include "fileprocessor.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QDir directory = (argc > 1) ? QDir(argv[1]) : QDir::currentPath();

    FileModel fileModel;
    fileModel.setDirectory(directory.absolutePath());

    FileProcessor fileProcessor;

    engine.rootContext()->setContextProperty("fileModel", &fileModel);
    engine.rootContext()->setContextProperty("fileProcessor", &fileProcessor);

    QObject::connect(&fileProcessor, &FileProcessor::fileProcessing,
                         &fileModel, &FileModel::updateFileStatus);

    QObject::connect(&fileProcessor, &FileProcessor::fileProcessed,
                     [&fileModel](const QString &filePath, bool success, const QString &message) {
                         if (!success) {
                             qWarning() << "Error processing file:" << message;
                         }
                         fileModel.updateFileStatus(filePath, "");
                         fileModel.updateDirectory();
                     });

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

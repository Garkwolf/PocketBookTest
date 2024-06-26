#include "filemodel.h"

#include <QFileInfo>

FileModel::FileModel(QObject *parent) : QAbstractListModel(parent) {
    m_supportedFormats << "bmp" << "png" << "barch";
}

QString FileModel::directory() const {
    return m_directory;
}

void FileModel::setDirectory(const QString &directory) {
    if (m_directory != directory) {
        m_directory = directory;
        emit directoryChanged();
        updateDirectory();
    }
}

void FileModel::updateDirectory() {
    QDir dir(m_directory);
    QStringList filters;
    for (const QString &format : m_supportedFormats) {
        filters << "*." + format;
    }
    QList<QFileInfo> newFileInfoList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);
    QHash<QString, QString> newFileStatus;

    for (const QFileInfo &fileInfo : newFileInfoList) {
        QString filePath = fileInfo.absoluteFilePath();
        if (m_fileStatus.contains(filePath)) {
            newFileStatus.insert(filePath, m_fileStatus.value(filePath));
        } else {
            newFileStatus.insert(filePath, "");
        }
    }

    beginResetModel();
    m_fileInfoList = newFileInfoList;
    m_fileStatus = newFileStatus;
    endResetModel();
}

bool FileModel::isSupportedFile(const QString &fileSuffix) const {
    return m_supportedFormats.contains(fileSuffix.toLower());
}

int FileModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_fileInfoList.count();
}

QVariant FileModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    const QFileInfo &fileInfo = m_fileInfoList.at(index.row());

    switch (role) {
    case NameRole:
        return fileInfo.fileName();
    case SizeRole:
        return fileInfo.size();
    case PathRole:
        return fileInfo.absoluteFilePath();
    case TypeRole:
        return fileInfo.suffix();
    case StatusRole:
        return m_fileStatus.value(fileInfo.absoluteFilePath());
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> FileModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[SizeRole] = "size";
    roles[PathRole] = "path";
    roles[TypeRole] = "type";
    roles[StatusRole] = "status";
    return roles;
}

void FileModel::updateFileStatus(const QString &fileName, const QString &status) {
    if (m_fileStatus.contains(fileName)) {
        m_fileStatus[fileName] = status;
        emit dataChanged(index(0), index(rowCount() - 1), { StatusRole });
    }
}

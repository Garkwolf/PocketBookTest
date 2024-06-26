#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QDir>
#include <QHash>
#include <QSet>

class FileModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString directory READ directory WRITE setDirectory NOTIFY directoryChanged)

public:
    explicit FileModel(QObject *parent = nullptr);
    
    Q_INVOKABLE void setDirectory(const QString &directory);
    Q_INVOKABLE void updateDirectory();
    Q_INVOKABLE void updateFileStatus(const QString &fileName, const QString &status);

    QString directory() const;

    enum FileRoles {
        NameRole = Qt::UserRole + 1,
        SizeRole,
        PathRole,
        TypeRole,
        StatusRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void directoryChanged();

private:
    QString m_directory;
    QList<QFileInfo> m_fileInfoList;
    QHash<QString, QString> m_fileStatus;
    QSet<QString> m_supportedFormats;

    bool isSupportedFile(const QString &fileName) const;
};

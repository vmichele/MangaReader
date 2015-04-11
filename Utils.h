#ifndef UTILS_H
#define UTILS_H

#include <QStringList>
#include <QDir>
#include <QObject>

class Utils {

public:
    Utils(void);

    static QStringList dirList(const QDir& dir, bool reverseOrder = false);
    static QStringList filesList(QDir& dir, const QStringList &nameFilters = QStringList());
    static void updateChapterRead(const QString &mangaName, const QString &chapterName, bool read);
    static bool isChapterRead(const QString& mangaName, const QString& chapterName);
    static QList<bool> areChaptersRead(const QString& mangaName);
    static bool isMangaRead(QList<bool> chaptersRead);
    static void initdb(const QString& mangaName);
    static void addChapter(const QString& mangaName, const QString& chapterName);
    static bool removeDirectory(const QString& dirName);

private:
    static QDir _scansDirectory;
    static void initdb(void);

};

#endif // UTILS_H

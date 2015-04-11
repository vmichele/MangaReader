#include "Utils.h"

#include <QProgressDialog>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QProgressBar>
#include <QtAlgorithms>

QDir Utils::_scansDirectory("/home/valentin/Images/Scans/");

Utils::Utils() {
}

// Return every directory's name within directory 'dir'
QStringList Utils::dirList(const QDir& dir, bool reverseOrder) {
    if (!dir.exists()) {
        qDebug() << dir.path() << "doesn't exist";
        return QStringList();
    }

    QStringList res = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::NoSort);
    if (reverseOrder)
        qSort(res.begin(), res.end(), qGreater<QString>());
    return res;
}

// Return every file's name within directory 'dir'
QStringList Utils::filesList(QDir& dir, const QStringList& nameFilters) {
    dir.setNameFilters(nameFilters);
    return dir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::NoSort);
}

// Initialize every db.txt
void Utils::initdb(void) {
    QStringList mangasList = dirList(_scansDirectory);

    foreach (const QString& mangaName, mangasList) {
        initdb(mangaName);
    }
}

// Update chapter read flag
void Utils::updateChapterRead(const QString& mangaName, const QString& chapterName, bool read) {
    QDir mangaDir(_scansDirectory.path()+"/"+mangaName);

    QFile file(mangaDir.path()+"/db.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error while opening db in read only within Utils::initdb";
    }

    QTextStream inFile(&file);
    inFile.setCodec("UTF-8");

    QStringList allText = inFile.readAll().split("\n");
    for (int k = 0; k < allText.size(); k++) {
        QString line = allText.at(k);
        if (line.startsWith(chapterName)) {
            if (!line.contains(":")) {
                qDebug() << "Error:" << mangaDir.path()+"/db.txt" << "corrupted.";
                break;
            }
            allText.replace(k, line.split(":").at(0)+":"+QString::number(read));
            break;
        }
    }

    file.remove();

    QFile newFile(mangaDir.path()+"/db.txt");
    if (!newFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error while opening db in write only within Utils::initdb";
    }

    QTextStream newInFile(&newFile);
    newInFile.setCodec("UTF-8");

    newInFile << allText.join("\n");

//    qDebug() << "Error: can't find" << chapterName << "in" << mangaDir.path()+"/db.txt within Utils::updateChapterRead";
}

// Return true if chapter has been read
bool Utils::isChapterRead(const QString &mangaName, const QString &chapterName) {
    QDir mangaDir(_scansDirectory.path()+"/"+mangaName);

    QFile file(mangaDir.path()+"/db.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error while opening db in read only within Utils::initdb";
    }

    QTextStream inFile(&file);
    inFile.setCodec("UTF-8");

    QString line = "";
    while (!(line = inFile.readLine()).startsWith(chapterName) && !inFile.atEnd()) {}

    file.close();

    if (inFile.atEnd() && !line.startsWith(chapterName)) {
        qDebug() << "Error: can't find" << chapterName << "in" << mangaDir.path()+"/db.txt within Utils::isChapterRead";
        return false;
    }

    QStringList dbArgs = line.split(":");
    if (dbArgs.size() != 2) {
        qDebug() << "Error: db is broken in" << mangaDir.path()+"/db.txt within Utils::isChapterRead";
        return false;
    }

    return dbArgs.at(1).toInt();
}


QList<bool> Utils::areChaptersRead(const QString& mangaName) {
    QList<bool> res;

    QDir mangaDir(_scansDirectory.path()+"/"+mangaName);
    QStringList chaptersDir = dirList(mangaDir, true);
    foreach (const QString& chapterName, chaptersDir) {
        res << isChapterRead(mangaName, chapterName);
    }

    return res;
}

bool Utils::isMangaRead(QList<bool> chaptersRead) {
  foreach (bool isChapterRead, chaptersRead) {
    if (!isChapterRead)
      return false;
  }
  return true;
}

// Initialize db.txt within mangaName directory
void Utils::initdb(const QString& mangaName) {
    QDir mangaDir(_scansDirectory.path()+"/"+mangaName);
    QStringList chaptersList = dirList(mangaDir, true);

    QFile file(mangaDir.path()+"/db.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error while opening db in write only within Utils::initdb";
    }

    QTextStream inFile(&file);
    inFile.setCodec("UTF-8");

    foreach (const QString& chapterName, chaptersList) {
        inFile << chapterName+":0\n";
    }

    file.close();
}

// Add new chapter inside db
void Utils::addChapter(const QString& mangaName, const QString& chapterName) {
    QDir mangaDir(_scansDirectory.path()+"/"+mangaName);

    QFile file(mangaDir.path()+"/db.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error while opening db in read only within Utils::addChapter";
    }

    QTextStream inFile(&file);
    inFile.setCodec("UTF-8");

    QStringList chapterProperty = inFile.readAll().split("\n");
    chapterProperty << chapterName+":0";
    qSort(chapterProperty.begin(), chapterProperty.end(), qGreater<QString>());

    file.remove();

    QFile newFile(mangaDir.path()+"/db.txt");
    if (!newFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error while opening db in write only within Utils::addChapter";
    }

    QTextStream newInFile(&newFile);
    newInFile.setCodec("UTF-8");

    newInFile << chapterProperty.join("\n");

    newFile.close();
}

bool Utils::removeDirectory(const QString& dirName) {
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        foreach (const QFileInfo& info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDirectory(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

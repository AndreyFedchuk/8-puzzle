// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#ifndef LOGGING_H
#define LOGGING_H

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDesktopServices>
#include <QDateTime>

#include <QMessageBox>
#include <QList>
#include <QVector>
#include <QString>


class Logging
{
public:
    Logging();
    bool logingInFile(const QList<QVector<int> > *pSolution);
    bool showLoggingFile();

private:
    const QList<QVector<int>> *m_pSolution;
    QString m_fileName;
    QFile m_file;
};

#endif // LOGGING_H

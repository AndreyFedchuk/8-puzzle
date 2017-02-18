// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#include "logging.h"

Logging::Logging()
{
    m_fileName = "LOG.txt";
    m_file.setFileName(m_fileName);
}

bool Logging::logingInFile(const QList<QVector<int> > *pSolution)
{
    Q_ASSERT(!pSolution->isEmpty());

    bool bOk(true);
    m_pSolution = pSolution;

    if(m_file.open(QIODevice::Append))
    {
        QTextStream stream(&m_file);

        stream << "\r\n\r\n ======================================= \r\n";
        stream << "     " << QDateTime::currentDateTime().toString();
        stream << "\r\n ======================================= \r\n";

        int index(0);
        for(auto &it : *m_pSolution)
        {
            if(index == 0)
            {
                stream << " Start position: \r\n";
                index++;
            }
            else
                stream << QString("     Step %1:\r\n").arg(index++);
            QString str;
            int indexNum(0);
            for(auto &num : it)
            {
                if(num == 0)
                    str += "     ";
                else
                    str += QString(" [%1] ").arg(QString::number(num));
                indexNum++;
                if(indexNum % 3 == 0)
                {
                    stream << QString("            %1\r\n").arg(str);
                    str = "";
                }
            }
            stream << "\r\n";
        }

        stream << " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \r\n";
        m_file.close();

        Q_ASSERT(stream.status() == QTextStream::Ok);
    }
    else
        bOk = false;

    return bOk;
}

bool Logging::showLoggingFile()
{
    bool bOk(true);
    QDir dir;
    QString fullPath = dir.absoluteFilePath(m_fileName);

    if(!QDesktopServices::openUrl(QUrl::fromUserInput(fullPath)))
        bOk = false;

    return bOk;
}

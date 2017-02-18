// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#include "mainwindow.h"
#include <QApplication>
#include <stdexcept>
#include <iostream>

int main(int argc, char *argv[])
{
    try
    {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        a.exec();
    }
    catch(std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

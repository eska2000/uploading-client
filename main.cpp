#include <QCoreApplication>
#include "testclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TestClient test;

    return a.exec();
}

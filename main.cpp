#include <QCoreApplication>
#include "lib/grooveshark/qsearch.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSearch* search = new QSearch();

    return a.exec();
}

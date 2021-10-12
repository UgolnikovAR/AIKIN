#include <QtWidgets>
#include "TServer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TServer server(2323);
    server.move(1920-400, 0);
    server.resize(400, 1050);
    server.show();

    return a.exec();
}

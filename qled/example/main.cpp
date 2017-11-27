#include <QApplication>
#include "dialog.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE_EXTERN(qled);
    QApplication a(argc, argv);
    Dialog w;
    w.show();
    return a.exec();
}

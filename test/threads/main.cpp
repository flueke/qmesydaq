#include <QDebug>
#include <QThread>

int main(int, char **)
{
	qDebug() << "Ideal number of threads : " << QThread::idealThreadCount();
	return 0;
}

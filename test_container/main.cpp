#include "testcontainer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	testcontainer w;
	w.show();

	return a.exec();
}

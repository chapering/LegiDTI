#include <QtGui/QApplication>
#include "transfer_function.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	TransferFunctionWidget tfw;
	tfw.setWindowTitle( "Transfer function ported from wxPython version" );
	tfw.place( 0, 0, 400, 250 );
	tfw.show();
	
	return app.exec();
}


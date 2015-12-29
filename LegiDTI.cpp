#include "QVTKApplication.h"
#include "vmRenderWindow.h"

#include "kaapi++"

int main(int argc, char* argv[])
{
	QVTKApplication app(argc, argv);

	ka::Community com = ka::System::join_community(argc, argv);
	com.commit();

	CLegiMainWindow win;
	win.setWindowTitle( "LegiDTI 1.0.0" );
	win.show();
	//win.selfRotate(5);
	app.exec();

	com.leave();
	ka::System::terminate();
	return 0;
}


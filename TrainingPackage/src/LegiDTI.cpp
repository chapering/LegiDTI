#include "QVTKApplication.h"
#include "vmRenderWindow.h"

int main(int argc, char* argv[])
{
	QVTKApplication app(argc, argv);

	CLegiMainWindow legi(argc, argv);
	legi.setVerinfo("LegiDTI Visual Encoding Study");
	if (0 == legi.run()) {
		return app.exec();
	}
	return 0;
}


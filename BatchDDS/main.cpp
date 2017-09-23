
/**	\file	main.cpp
*	Batch DDS Tool.

// Licensed under the "New" BSD License. 
//		License page - https://github.com/Neill3d/BatchDDS/blob/master/LICENSE

	GitHub repo - https://github.com/Neill3d/BatchDDS

	Author Sergey Solokhin (Neill3d)
	 e-mail to: s@neill3d.com
	  www.neill3d.com

*/

#include "batchdds.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	SearchFBX w;
	w.SetStarupPath(a.applicationDirPath() );
	w.show();
	return a.exec();
}

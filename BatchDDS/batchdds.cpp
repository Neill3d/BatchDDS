
/**	\file	batchdds.cpp
*	Batch DDS Tool.

// Licensed under the "New" BSD License. 
//		License page - https://github.com/Neill3d/BatchDDS/blob/master/LICENSE

	GitHub repo - https://github.com/Neill3d/BatchDDS

	Author Sergey Solokhin (Neill3d)
	 e-mail to: s@neill3d.com
	  www.neill3d.com

*/

#include "batchdds.h"
#include <QString>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QTableWidgetItem>
#include <qprocess.h>
#include <qdiriterator.h>
#include <qsettings.h>
#include <qfont.h>
#include "Common.h"

SearchFBX::SearchFBX(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.buttonClose, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.buttonAbout, SIGNAL(clicked()), this, SLOT(about()));
	connect(ui.buttonFolder, SIGNAL(clicked()), this, SLOT(chooseFolder()));

	connect(ui.buttonSearch, SIGNAL(clicked()), this, SLOT(doSearch()));
	connect(ui.buttonPreview, SIGNAL(clicked()), this, SLOT(doOpenFile()));
	//connect(ui.buttonApply, SIGNAL(clicked()), this, SLOT(doApply()));
	connect(ui.buttonConvert, SIGNAL(clicked()), this, SLOT(doConvertSelected()));
	connect(ui.buttonConvertAll, SIGNAL(clicked()), this, SLOT(doConvertAll()));
	//connect(ui.buttonMoBu, SIGNAL(clicked()), this, SLOT(chooseMoBuPath()));
	//connect(ui.editMoBu, SIGNAL(editingFinished()), this, SLOT(changePath()));

	connect(ui.buttonBrowseOutput, SIGNAL(clicked()), this, SLOT(chooseFolderOutput()));

	setUp();

	QSettings settings( tr("%1\\config.ini").arg( QApplication::applicationDirPath() ), QSettings::IniFormat );
	mMoBuPath = settings.value( "Common/MoBu_Path", "c:\\Program Files\\Autodesk\\MotionBuilder 2014\\bin\\x64\\motionbuilder.exe").toString();
	ui.editPath->setText( settings.value( "Common/Search_Path", "C:\\").toString() );
	ui.editOutput->setText( settings.value( "Common/Output_Path", "C:\\").toString() );
	mRowHeight = settings.value( "Row/Height", 15 ).toInt();
	mFontHeight = settings.value( "Font/Height", 9 ).toInt();

	ui.table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//ui.table->setVerticalScrollBar( ui.verticalScrollBar );
	//ui.table->addScrollBarWidget( ui.verticalScrollBar, Qt::AlignLeft );

	connect(ui.table->verticalScrollBar(), SIGNAL(rangeChanged(int, int) ), ui.verticalScrollBar, SLOT(setRange(int, int) ) );
	connect(ui.table->verticalScrollBar(), SIGNAL(rangeChanged(int, int) ), this, SLOT(doSetRange(int, int) ) );
	ui.verticalScrollBar->setVisible(false);
	connect(ui.verticalScrollBar, SIGNAL(valueChanged(int) ), ui.table->verticalScrollBar(), SLOT(setValue(int) ) );
	//connect(ui.verticalScrollBar, SIGNAL(sliderPressed() ), ui.table->verticalScrollBar(), SLOT(sliderPressed() ) );
	//connect(ui.verticalScrollBar, SIGNAL(sliderMoved() ), ui.table->verticalScrollBar(), SLOT(sliderMoved() ) );
	//connect(ui.verticalScrollBar, SIGNAL(sliderReleased() ), ui.table->verticalScrollBar(), SLOT(sliderReleased() ) );
	//connect(ui.verticalScrollBar, SIGNAL(actionTriggered() ), ui.table->verticalScrollBar(), SLOT(actionTriggered() ) );


	mFont.setPointSize(mFontHeight);

	// Create the FBX SDK manager which is the object allocator for almost 
   // all the classes in the SDK and create the scene.
   //InitializeSdkObjects(mSdkManager, mScene);
}

void SearchFBX::doSetRange(int min, int max)
{
	ui.verticalScrollBar->setVisible(true);
}

void SearchFBX::changePath()
{
	//QSettings settings( tr("%1\\config.ini").arg( QApplication::applicationDirPath() ), QSettings::IniFormat );
	//settings.setValue( "Common/MoBu_Path", ui.editMoBu->text() );
}

SearchFBX::~SearchFBX()
{
	// Delete the FBX SDK manager. All the objects that have been allocated 
    // using the FBX SDK manager and that haven't been explicitly destroyed 
    // are automatically destroyed at the same time.
	//DestroySdkObjects(mSdkManager, true);

	//
	
	QSettings settings( tr("%1\\config.ini").arg( QApplication::applicationDirPath() ), QSettings::IniFormat );
	settings.setValue( "Common/Search_Path", ui.editPath->text() );
	settings.setValue( "Common/Output_Path", ui.editOutput->text() );
	//settings.setValue( "Row/Height", mRowHeight );
	//settings.setValue( "Font/Height", mFontHeight );
	
}

void SearchFBX::setUp()
{
    ui.table->setRowCount( 0 );
    ui.table->setColumnCount( 2 );
    ui.table->setColumnWidth(0, 300);
	//ui.table->setColumnWidth(3, 550);

	QStringList list;
    list << "File name" << "Path";
    ui.table->setHorizontalHeaderLabels(list);
}

void SearchFBX::about()
{
	QMessageBox::information(this, tr("Batch DDS Converter v 0.5b about"),
                             tr("Created by Sergey Solokhin (Neill) 2015, e-mail to: s@neill3d.com"));
}

void SearchFBX::chooseFolder()
{
	QString dir = ui.editPath->text();

    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    dir = QFileDialog::getExistingDirectory(this,
                                    tr("Specify fbx directory"),
                                    dir,
                                    options);
    if (!dir.isEmpty())
    {
        ui.editPath->setText(dir);

		doSearch();
    }
}

void SearchFBX::chooseFolderOutput()
{
	QString dir = ui.editOutput->text();

    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    dir = QFileDialog::getExistingDirectory(this,
                                    tr("Specify output directory"),
                                    dir,
                                    options);
    if (!dir.isEmpty())
    {
        ui.editOutput->setText(dir);
		ui.checkOutput->setChecked(false);
    }
}

void SearchFBX::doSearch()
{
	updateData();
	displayData();
}

void SearchFBX::doApply()
{
	updateData();
	displayData();
}

QString SearchFBX::getFormatStr()
{
	int index = ui.comboFormats->currentIndex();

	switch(index)
	{
	case 0:
		return QString( "-bc1" );
	case 1:
		return QString( "-bc1n" );	
	case 2:
		return QString( "-bc1a" );
	case 3:
		return QString( "-bc2" );
	case 4:
		return QString( "-bc3" );
	case 5:
		return QString( "-bc3n" );
	case 6:
		return QString( "-bc4" );
	case 7:
		return QString( "-bc5" );
	}

	return QString( "-bc1a" );
}

bool SearchFBX::ConvertItem(const QString &nvcompressPath, const int rowIndex, const int index, bool mipmaps, QProgressDialog *pProgress)
{
	QTableWidgetItem *item;

	item = ui.table->item(rowIndex,0);
	QString filename = item->text();

	item = ui.table->item(rowIndex,1);
	QString filepath = item->text();

	QString file = filepath + QDir::separator() + filename;
	QStringList arguments;
	arguments.append( getFormatStr() );
	if (!mipmaps)
		arguments.append( tr("nomips") );
	arguments.append(file);
	
	bool useSameFolder = ui.checkOutput->isChecked();
	if (useSameFolder == false)
	{
		file = ui.editOutput->text() + QDir::separator() + filename.section(".",0,0) + ".dds";
		arguments.append(file);
	}

	if (pProgress)
	{
		pProgress->setLabelText( file );
		pProgress->setValue(index);
		QApplication::processEvents();
			
		if (pProgress->wasCanceled() )
			return false;
	}

	//
	QProcess process;
	process.start( nvcompressPath, arguments );
	process.write( "exit\n\r" );
	process.waitForFinished(INT_MAX);
	process.close();

	return true;
}

void SearchFBX::doConvertAll()
{
	if (ui.table->rowCount() == 0)
		return;

	QProgressDialog		progress( tr("Converting Images to DDS"), tr("Cancel"), 0, ui.table->rowCount() );
	progress.show();
	progress.setWindowModality(Qt::WindowModal);

	QString mobu = mStartupPath + QDir::separator() + "nvc" + QDir::separator() + "nvcompress.exe";
	bool needMipmaps = ui.checkMipmaps->isChecked();

	for (int i=0; i<ui.table->rowCount(); ++i)
	{
		if (false == ConvertItem( mobu, i, i, needMipmaps, &progress ) )
			break;
	}
}

void SearchFBX::doConvertSelected()
{
	if (ui.table->rowCount() == 0)
		return;
	
	QList<QTableWidgetItem*> items = ui.table->selectedItems();

	QProgressDialog		progress( tr("Converting Images to DDS"), tr("Cancel"), 0, items.size() );
	progress.show();
	progress.setWindowModality(Qt::WindowModal);

	QString mobu = mStartupPath + QDir::separator() + "nvc" + QDir::separator() + "nvcompress.exe";
	//mobu = "c:\\Work\\MOPLUGS\\Projects\\BatchDDS\\nvidia-texture-tools-master\\project\\vc12\\Release.Win32\\bin\\nvcompress.exe";

	bool needMipmaps = ui.checkMipmaps->isChecked();

	for (int i=0; i<items.size(); ++i)
	{
		QTableWidgetItem *item = items[i];

		const int rowIndex = item->row();

		if (false == ConvertItem( mobu, rowIndex, i, needMipmaps, &progress ) )
			break;
	}
}

void SearchFBX::chooseMoBuPath()
{
	/*
	QString path = ui.editMoBu->text();

    QFileDialog::Options options = QFileDialog::DontResolveSymlinks;
	
	path = QFileDialog::getOpenFileName(this,
										tr("Specify the MotionBuilder path"),
										path,
										"*.exe");

    if (!path.isEmpty())
    {
        ui.editMoBu->setText(path);
		changePath();
    }
	*/
}

void SearchFBX::doPreview()
{
	/*
	if (mMoBuPath.size() == 0)
	{
		QMessageBox::warning(this, tr("Search in FBX files"), tr("Please enter a MotionBuilder path!"));
	}
	else
	if (ui.table->rowCount() > 0)
	{
		QList<QTableWidgetItem*> items = ui.table->selectedItems();

		if (items.size() > 0)
		{
			QTableWidgetItem *item = items[0];

			const int rowIndex = item->row();

			item = ui.table->item(rowIndex,0);
			QString filename = item->text();

			item = ui.table->item(rowIndex,3);
			QString filepath = item->text();

			QString file = filepath + QDir::separator() + filename;
			QStringList arguments;
			arguments.append( tr("-open") );
			arguments.append(file);

			QProcess *process = new QProcess();
			QString mobu = mMoBuPath;
			process->start( mobu, arguments );
		}

	}
	*/
}

void SearchFBX::doExportList()
{
	QString path = "C:\\";

    QFileDialog::Options options = QFileDialog::DontResolveSymlinks;
	
	QFileDialog dialog(this, "Choose file to save");

	dialog.setNameFilter( "Txt Files (*.txt)" );
	dialog.setDefaultSuffix("txt");
	
	if ( dialog.exec() )
    {
		QFile file(dialog.selectedFiles().first());

		if (file.open( QFile::WriteOnly | QFile::Text ) )
		{
			QString line;

			for (int i=0; i<ui.table->rowCount(); ++i)
			{
				line = tr("%1,%2,%3,%4\n").arg(ui.table->item(i,0)->text(), ui.table->item(i,1)->text(), ui.table->item(i,2)->text(), ui.table->item(i, 3)->text() );
				file.write( line.toLocal8Bit() );
			}

			file.close();
		}
    }
}

QString SearchFBX::getFileName(const QString &path)
{
	int idx = path.lastIndexOf('.');
	/*
    while (idx > 0)
    {
        if (path[idx-1].isLetter()) break;
        idx--;
    }
	*/

    QString result = (idx>0) ? path.left(idx) : path;

	return result;
}

void SearchFBX::updateData()
{
	
	QString path = ui.editPath->text();
	
	if (path.isEmpty() ) {
		QMessageBox::warning(this, tr("Batch DDS Converter"), tr("Please enter a path for searching!"));
		return;
	}

	QDir	dir(path);

	if (!dir.exists() )
		return;

	mFiles.clear();

	QStringList	nameFilters;
	nameFilters << "*.tga" << "*.jpg" << "*.png" << "*.tif" << "*.psd"; // << "*.dds";

	const QDirIterator::IteratorFlag flags = ui.checkSubFolders->isChecked() ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
	QDirIterator dirIterator(path, nameFilters, QDir::Dirs | QDir::Files, flags );

	while(dirIterator.hasNext()) {
 
		QString file = dirIterator.next();

		ImageFile newItem;

		QFileInfo fileInfo(file);
		QString filename(fileInfo.fileName());
		path = fileInfo.absolutePath();

		//newItem.setName( getFileName(filename) );
		newItem.setName( filename );

		QString fullFileName = file;
		QFileInfo   info(fullFileName);
		newItem.setDate(info.lastModified());
		newItem.setPath( path );

		mFiles.append(newItem);
	}
}

void SearchFBX::displayData()
{
	ui.table->clear();
	ui.table->setRowCount(mFiles.size());

	
	const bool caseSensivity = ui.checkCaseSensitive->isChecked();
	
	int index = 0;
    for (int i=0; i<mFiles.size(); ++i)
    {
        auto &imagefile = mFiles.at(i);

        QTableWidgetItem *newItem = new QTableWidgetItem( imagefile.getName() );
        //if (i % 2 == 0) newItem->setBackgroundColor(QColor(darkerColor, darkerColor, darkerColor));
        ui.table->setItem(index, 0, newItem);

		//output date
		/*
		QDateTime date = fbxfile.getDate();

        QTableWidgetItem *newDateItem = new QTableWidgetItem(date.toString());
        //if (i % 2 == 0) newDateItem->setBackgroundColor(QColor(darkerColor, darkerColor, darkerColor));
        ui.table->setItem(index, 2, newDateItem);
		*/
		newItem = new QTableWidgetItem(imagefile.getPath() );
		ui.table->setItem(index, 1, newItem);

		index++;
	}
	
	ui.table->setRowCount(index);
	ui.table->setFont(mFont);
	for (int i=0; i<index; ++i)
	{
		ui.table->setRowHeight(i, mRowHeight);
	}

	QStringList list;
    list << "File name" << "Path" ;
    ui.table->setHorizontalHeaderLabels(list);

	//
	ui.labelResult->setText( tr( "Search Result: %1" ).arg(index) );
}

void SearchFBX::SetStarupPath(const QString &path)
{
	mStartupPath = path;
}
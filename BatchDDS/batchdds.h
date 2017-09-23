#ifndef SEARCHFBX_H
#define SEARCHFBX_H

/**	\file	batchdds.h
*	Batch DDS Tool.

// Licensed under the "New" BSD License. 
//		License page - https://github.com/Neill3d/BatchDDS/blob/master/LICENSE

	GitHub repo - https://github.com/Neill3d/BatchDDS

	Author Sergey Solokhin (Neill3d)
	 e-mail to: s@neill3d.com
	  www.neill3d.com

*/

#include <QtWidgets/QMainWindow>
#include <QList>
#include <QDateTime>
#include <qprogressdialog.h>
#include <qfont.h>
#include "ui_batchdds.h"

#include <fbxsdk.h>

//////////////////////////////////////////////////////////////////////////////////////
//
struct ImageFile
{
public:
    //! a constructor
    ImageFile()
    {
    }

	const QString &getPath() const { return path; }
    void setPath(const QString &newPath) { path = newPath; }

    const QString &getName() const { return name; }
    void setName(const QString &newName) { name = newName; }

	void setDate(const QDateTime &datetime) {
        date = datetime;
    }
	const QDateTime &getDate() const {
		return date;
	}

private:
	QString					path;	// file path
    QString                 name;   // sequence name
	QDateTime				date;
};

/////////////////////////////////////////////////////////////////////////////////////
//
class SearchFBX : public QMainWindow
{
	Q_OBJECT

public:
	SearchFBX(QWidget *parent = 0);
	~SearchFBX();

	void SetStarupPath(const QString &path);

public slots:
	
	void	about();

	void	chooseFolder();
	void	chooseFolderOutput();
	void	chooseMoBuPath();

	void	changePath();

	void	doSearch();
	void	doPreview();

	void	doConvertSelected();
	void	doConvertAll();
	
	void	doApply();	// apply display phrase filter
	void	doExportList();

	void	doSetRange(int min, int max);

private:
	Ui::BatchDDSClass ui;

	QList<ImageFile>	mFiles;
	QFont				mFont;
	QString				mMoBuPath;

	QString				mStartupPath;

	int				mRowHeight;
	int				mFontHeight;

	void	setUp();

	void	updateData();
	void	displayData();

	FbxManager * mSdkManager;
    FbxScene * mScene;
	FbxImporter * mImporter;

	QString	getFileName(const QString &path);

	QString getFormatStr();
	bool ConvertItem(const QString &nvcompressPath, const int row, const int index, bool mipmaps, QProgressDialog *pProgress);
};

#endif // SEARCHFBX_H

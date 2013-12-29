#include "uivinputsettings.h"
#include <QFileDialog>
#include "UIFaceFinderWidget.h"
//
UIVInputSettings::UIVInputSettings( VideoInput *vi,
                                    QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)	  
{
	setupUi(this);
	videoInput = vi;
	sourceComboBox->addItem( vs2str( VideoInput:: VSWebCamera));
	sourceComboBox->addItem( vs2str( VideoInput:: VSPictureSetRandom));
	sourceComboBox->addItem( vs2str( VideoInput:: VSListRandom));
	viewFaceSet->setModel( &listFaces);
	viewNonfaceSet->setModel( &listNonfaces);
	defaultFaceSetDir    = "/home/lukhaz/Dokumenty/Praca__Magisterska/Data/ALL/Face";
	defaultNonfaceSetDir = "/home/lukhaz/Dokumenty/Praca__Magisterska/Data/ALL/Nonface";
	tableWidget->setColumnCount( 2);
}

void UIVInputSettings:: updateSettings()
{
	labelWebCamera->setText( (QString)videoInput->cameraProperties().c_str());
}

void UIVInputSettings::on_buttonBox_accepted()
{
	std::vector<std::string> positive, negative, filesList;
	switch (sourceComboBox->currentIndex())
	{
		case 0:
			videoSource = VideoInput:: VSWebCamera;
			break;
		case 1:
			if (radioSetRandom->isChecked())
				videoSource = VideoInput:: VSPictureSetRandom;
			else
				videoSource = VideoInput:: VSPictureSetSequence;				
			for (int i = 0; i < listFaces.stringList().size(); i++)
				positive.push_back( std::string( (const char*) 
				                                 listFaces.stringList()[i]
				                                 .toLocal8Bit()));
			for (int i = 0; i < listNonfaces.stringList().size(); i++)
				negative.push_back( std::string( (const char*) 
				                                 listNonfaces.stringList()[i]
				                                 .toLocal8Bit()));
			videoInput->setPictureSets( positive, negative);
			break;
		case 2:
			if (radioListRandom->isChecked())
				videoSource = VideoInput:: VSListRandom;
			else
				videoSource = VideoInput:: VSListSequence;
			for (int i = 0; i < tableWidget->rowCount(); i++)
				filesList.push_back( std::string( (const char*)
				                                  tableWidget->item( i, 1)
				                                  ->text().toLocal8Bit()));
			videoInput->setFilesList( filesList);
			break;
		default:
			videoSource = VideoInput:: VSUndefined;
	}
	videoInput->setVideoSource( videoSource);
	faceFinderWidget->videoWidget->setMaximumWidth( videoInput->width());
	faceFinderWidget->videoWidget->setMaximumHeight( videoInput->height());
}

void UIVInputSettings::on_buttonBox_rejected()
{
	//no extra actions
}

void UIVInputSettings::on_buttonFaceSet_clicked()
{
	QFileDialog dialogFaceSet;
	dialogFaceSet.setFileMode( QFileDialog::ExistingFiles);
	dialogFaceSet.setNameFilter( tr("Images (*.pgm)"));
	dialogFaceSet.setDirectory( defaultFaceSetDir);
	if (dialogFaceSet.exec())
		listFaces.setStringList( dialogFaceSet.selectedFiles());
}

void UIVInputSettings::on_buttonNonfaceSet_clicked()
{
	QFileDialog dialogNonfaceSet;
	dialogNonfaceSet.setFileMode( QFileDialog::ExistingFiles);
	dialogNonfaceSet.setNameFilter( tr("Images (*.pgm)"));
	dialogNonfaceSet.setDirectory( defaultNonfaceSetDir);
	if (dialogNonfaceSet.exec())
		listNonfaces.setStringList( dialogNonfaceSet.selectedFiles());
}

QString UIVInputSettings:: vs2str( VideoInput::VideoSource vs)
{
	switch (vs)
	{
		case VideoInput:: VSUndefined:
			return "Undefined";
		case VideoInput:: VSWebCamera:
			return "Web Camera";
		case VideoInput:: VSPictureSetRandom:
		case VideoInput:: VSPictureSetSequence:
			return "Picture Set";
		case VideoInput:: VSListRandom:
		case VideoInput:: VSListSequence:
			return "Pictures List";
		default:
			return "Unknown VideoSource value";
	}
}
void UIVInputSettings::on_buttonAddPictures_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setFileMode( QFileDialog::ExistingFiles);
	fileDialog.setNameFilter( tr("images (*.pgm)"));
	if (fileDialog.exec())
	{
		int offset = tableWidget->rowCount();
		QTableWidgetItem *item;
		tableWidget->setRowCount( offset + fileDialog.selectedFiles().size());
		labelPicturesCount->setText( "Pictures count: " 
		                           + QString::number( tableWidget->rowCount()));
		for( int i = 0; i < fileDialog.selectedFiles().size(); i++)
		{
			 item = new QTableWidgetItem( QIcon(fileDialog.selectedFiles()[i]),
			              fileDialog.selectedFiles()[i].remove( 0, 
			              fileDialog.selectedFiles()[i].lastIndexOf( "/") + 1));
			tableWidget->setItem( offset + i, 0, item);
			item = new QTableWidgetItem( fileDialog.selectedFiles()[i]);
			tableWidget->setItem( offset + i, 1, item);
		}
	}
}

void UIVInputSettings::on_buttonClearList_clicked()
{
	tableWidget->clear();
	tableWidget->setRowCount(0);
	labelPicturesCount->setText( "Pictures count: 0");
}

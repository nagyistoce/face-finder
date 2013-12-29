#include "uiindividualrecognition.h"
#include "FaceRecognizer.h"
#include <QFileDialog>
#include <QTableWidgetItem>
//
UIIndividualRecognition::UIIndividualRecognition( VideoInput *vi,
                                                  FaceRecognizer *fr,
                                                 QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	videoInput = vi;
	faceRecognizer = fr;
	progressBar->setShown( false);
	tableWidget->setColumnCount( 3);
	QTableWidgetItem *item;
	item = new QTableWidgetItem( "file");
	tableWidget->setHorizontalHeaderItem(0, item);
	item = new QTableWidgetItem( "value");
	tableWidget->setHorizontalHeaderItem(1, item);
	item = new QTableWidgetItem( "path");
	tableWidget->setHorizontalHeaderItem(2, item);
}
//
void UIIndividualRecognition:: setId( int id)
{
	id_ = id;
	tableWidget->clear();
	tableWidget->setRowCount(0);
	labelId->setText( QString::number( id_));
	prevWidth_ = videoInput->width(); 
	prevHeight_ = videoInput->height();
	prevSource_ = videoInput->videoSource();
	prevMode_ = videoInput->scanningMode();	
	videoInput->resize( videoInput->recognizerWidth(), 
	                    videoInput->recognizerHeight());
	videoInput->setVideoSource( VideoInput::VSListSequence);
	videoInput->setScanningMode( VideoInput::SMFullImage);
}
void UIIndividualRecognition:: enableControls(bool enabled)
{
	buttonAdd->setEnabled( enabled);
	buttonClear->setEnabled( enabled);
	buttonTest->setEnabled( enabled);
	buttonLearn->setEnabled( enabled);
	closeWidget->setShown( enabled);
	progressBar->setShown( !enabled);
	if (enabled)
		stackedWidget->setCurrentWidget( tablePage);
	else
		stackedWidget->setCurrentWidget( showPage);
}

void UIIndividualRecognition::on_buttonAdd_clicked()
{
	enableControls(false);
	QFileDialog fileDialog;
	fileDialog.setFileMode( QFileDialog::ExistingFiles);
	fileDialog.setNameFilter( tr("images (*.pgm)"));
	if (fileDialog.exec())
	{
		int offset = tableWidget->rowCount();
		QTableWidgetItem *item;
		tableWidget->setRowCount( offset + fileDialog.selectedFiles().size());
		progressBar->reset();
		progressBar->setMaximum( fileDialog.selectedFiles().size());
		for( int i = 0; i < fileDialog.selectedFiles().size(); i++)
		{
			 item = new QTableWidgetItem( QIcon(fileDialog.selectedFiles()[i]),
			              fileDialog.selectedFiles()[i].remove( 0, 
			              fileDialog.selectedFiles()[i].lastIndexOf( "/") + 1));
			tableWidget->setItem( offset + i, 0, item);
			item = new QTableWidgetItem( fileDialog.selectedFiles()[i]);
			tableWidget->setItem( offset + i, 2, item);
			filesList_.push_back( std::string( fileDialog.selectedFiles()[i]
			                                   .toLocal8Bit()));
			progressBar->setValue( i + 1);
		}
		videoInput->setFilesList( filesList_);
	}
	enableControls(true);
}

void UIIndividualRecognition::on_buttonTest_clicked()
{
	enableControls(false);
	progressBar->reset();
	progressBar->setMaximum( tableWidget->rowCount());	
	for( int i = 0; i < tableWidget->rowCount(); i++)
	{
		videoInput->update();
		labelVideoInput->setPixmap( 
		                           QPixmap::fromImage( videoInput->videoFrame()));
		faceRecognizer->projectImage();
		QTableWidgetItem *item = new QTableWidgetItem( QString::number(
		                                           faceRecognizer->classify()));
		tableWidget->setItem( i, 1, item);
		progressBar->setValue( tableWidget->rowCount() + i + 1);
		qApp->processEvents(QEventLoop::AllEvents, 500);
	}
	progressBar->setShown( false);
	enableControls(true);
}

void UIIndividualRecognition::on_buttonLearn_clicked()
{
	enableControls(false);	
	progressBar->reset();	
	progressBar->setMaximum( spinLearnCount->value() * tableWidget->rowCount());
	for (int i, j = 0; j < spinLearnCount->value(); j++)	
	{
		for( i = 0; i < tableWidget->rowCount(); i++)
		{
			videoInput->update();
			labelVideoInput->setPixmap( 
		                         QPixmap::fromImage( videoInput->videoFrame()));
		    faceRecognizer->projectImage();
			faceRecognizer->setClassification( id_);
			progressBar->setValue( j * spinLearnCount->value() + i + 1);
			qApp->processEvents(QEventLoop::AllEvents, 500);
		}
	}
	enableControls(true);
}

void UIIndividualRecognition::on_buttonClear_clicked()
{
	tableWidget->clear();
	tableWidget->setRowCount(0);
	filesList_.clear();
}

void UIIndividualRecognition::on_closeButton_clicked()
{
	videoInput->resize( prevWidth_, prevHeight_);
	videoInput->setVideoSource( (VideoInput::VideoSource) prevSource_);
	videoInput->setScanningMode( (VideoInput::ScanningMode) prevMode_);
	tableWidget->clear();
	tableWidget->setRowCount(0);
	filesList_.clear();
	close();
}
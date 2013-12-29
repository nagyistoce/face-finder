#include "uirecognizerlearning.h"
#include "VideoInput.h"
#include "VideoScanner.h"
#include "FaceRecognizer.h"
#include "qpicturelisteditor.h"

//
UIRecognizerLearning::UIRecognizerLearning( VideoInput *vi,
                                            VideoScanner *vs,
                                            FaceRecognizer *fr,
                                            QPictureListEditor *ple,
                                            QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	videoInput = vi;
	videoScanner = vs;
	faceRecognizer = fr;
	pictureListEditor = ple;
	learning_ = false;
	stackedWidget->setCurrentWidget( learnSettings);
	buttonClose->setShown( false);
}
//

void UIRecognizerLearning::on_buttonStart_clicked()
{
	learning_ = true;
	buttonStart->setEnabled( false);
	buttonEdit->setEnabled( false);
	progressBar->setMaximum( spinRememberLoops->value() +
	                         spinClasificationLoops->value());
	stackedWidget->setCurrentWidget( learnProgress);
	videoInput->setFilesList( fileList_);
	int i, idx;
	QString picname;
	cout << "Remember loop" << endl;
	for (i = 0; i < spinRememberLoops->value() && learning_; i++)
	{
		videoInput->update();
		idx = videoInput->fileIndex();
		picname = QString(fileList_[idx].c_str());
		picname = picname.mid( picname.lastIndexOf('/') + 1);
		videoScanner->loadImage( videoInput->toMatrix());
		faceRecognizer->setInput( videoScanner);
		textEdit->insertPlainText( "Remember " + picname
		                           + " (" + QString::number(
		                                    faceRecognizer->remember())
		                           + ")\n" );
		progressBar->setValue( i + 1);
		qApp->processEvents(QEventLoop::AllEvents, 100);
	}
	cout << "Classification loop" << endl;
	for ( i = 0; i < spinClasificationLoops->value() && learning_; i++)
	{
		videoInput->update();
		idx = videoInput->fileIndex();
		picname = QString(fileList_[idx].c_str());
		picname = picname.mid( picname.lastIndexOf('/') + 1);
		videoScanner->loadImage( videoInput->toMatrix());
		faceRecognizer->setInput( videoScanner);
		textEdit->insertPlainText( "Set " + picname + " class id to "               
		                           + QString::number( pictureList_.value( picname))
		                           + " (" + QString::number(
		                                    faceRecognizer->classify())
		                           + ")\n" );
		faceRecognizer->setClassId( pictureList_.value( picname));
		progressBar->setValue( spinRememberLoops->value() + i + 1);
		qApp->processEvents(QEventLoop::AllEvents, 100);
	}
	if (learning_)
	{
		buttonClose->setShown(true);
		buttonCancel->setShown(false);
	} 
	else safeClose();
}

void UIRecognizerLearning::on_buttonCancel_clicked()
{
	if (!learning_) safeClose();
	else learning_ = false;
}


int UIRecognizerLearning:: safeExec()
{
	prevWidth_ = videoInput->width(); 
	prevHeight_ = videoInput->height();
	prevSource_ = videoInput->videoSource();
	prevMode_ = videoScanner->scanningMode();	
	videoInput->resize( faceRecognizer->imageWidth(), 
	                    faceRecognizer->imageHeight());
	videoInput->setVideoSource( VideoInput::VSListRandom);
	videoScanner->setImageSize( faceRecognizer->imageWidth(), 
	                            faceRecognizer->imageHeight());
	videoScanner->setScanningMode( VideoScanner::SMFullImage);
	return exec();
}

void UIRecognizerLearning:: safeClose()
{
	textEdit->clear();
	stackedWidget->setCurrentWidget( learnSettings);
	buttonStart->setEnabled( true);
	buttonEdit->setEnabled( true);
	videoInput->resize( prevWidth_, prevHeight_);
	videoInput->setVideoSource( (VideoInput::VideoSource) prevSource_);
	videoScanner->setImageSize( prevWidth_, prevHeight_);
	videoScanner->setScanningMode( (VideoScanner::ScanningMode) prevMode_);
	done( learning_ ? QDialog::Accepted : QDialog::Rejected);
}

void UIRecognizerLearning::on_buttonClose_clicked()
{
	buttonClose->setShown(false);
	buttonCancel->setShown(true);
	safeClose();
}


void UIRecognizerLearning::on_buttonEdit_clicked()
{
	pictureListEditor->setList( fileList_, pictureList_);
	pictureListEditor->exec();
	pictureListEditor->getList( fileList_, pictureList_);
	labelPicturesNumber->setText( QString::number( fileList_.size()));
}

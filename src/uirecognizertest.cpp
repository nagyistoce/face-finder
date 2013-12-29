#include "uirecognizertest.h"
#include "qpicturelisteditor.h"
#include "VideoInput.h"
#include "VideoScanner.h"
#include "FaceRecognizer.h"
//
UIRecognizerTest::UIRecognizerTest( VideoInput *vi,
                                    VideoScanner *vs,
                                    FaceRecognizer *fr,
                                    QPictureListEditor *ple,
                                    QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	pictureListEditor = ple;
	videoInput = vi;
	videoScanner = vs;
	faceRecognizer = fr;
	testing_ = false;
	all_ = 0;
	good_ = 0;
	bad_ = 0;
	not_ = 0;
	connect( this, SIGNAL(testRequest()), this, SLOT(performTest()));
}
//

void UIRecognizerTest::on_buttonEdit_clicked()
{
	pictureListEditor->setList( fileList_, pictureList_);
	pictureListEditor->exec();
	pictureListEditor->getList( fileList_, pictureList_);
	progressBar->setMaximum( fileList_.size());
	labelPicturesNumber->setText( QString::number( fileList_.size()));
}

void UIRecognizerTest::on_buttonStart_clicked()
{
	if (testing_)
	{
		testing_ = false;
		buttonStart->setText( "Start" );
		buttonClose->setEnabled( true);
	}
	else
	{
		testing_ = true;
		buttonStart->setText( "Stop" );
		buttonClose->setEnabled( false);
		emit testRequest();
	}
}

void UIRecognizerTest::on_buttonClose_clicked()
{
	safeClose();
}

void UIRecognizerTest:: performTest()
{
	testDuration_.start();
	videoInput->setFilesList( fileList_);
	unsigned int i, id, idx, goodId;
	QString picname;
	all_ = good_ = bad_ = not_ = 0;
	for (i = 0; i <  fileList_.size() && testing_; i++)
	{
		videoInput->update();
		idx = videoInput->fileIndex();
		picname = QString( fileList_[idx].c_str() );
		picname = picname.mid( picname.lastIndexOf('/') +1 );
		goodId = pictureList_.value( picname);
		videoScanner->loadImage( videoInput->toMatrix());
		recognitionDuration_.start();
		faceRecognizer->setInput( videoScanner);		
		id = faceRecognizer->classify();
		recognitionDuration_.stop();		
		++all_;
		if (id == goodId)
			++good_;
		else
		{
			if (id)
				++bad_;
			else
				++not_;
		}
		updateStats();
		progressBar->setValue( i + 1);
		qApp->processEvents(QEventLoop::AllEvents, 100);
	}
	testDuration_.stop();
	testing_ = false;
	buttonStart->setText( "Start" );
	buttonClose->setEnabled( true);
}

int UIRecognizerTest:: safeExec()
{
	prevWidth_ = videoInput->width(); 
	prevHeight_ = videoInput->height();
	prevSource_ = videoInput->videoSource();
	prevMode_ = videoScanner->scanningMode();	
	videoInput->resize( faceRecognizer->imageWidth(), 
	                    faceRecognizer->imageHeight());
	videoInput->setVideoSource( VideoInput::VSListSequence);
	videoScanner->setImageSize( faceRecognizer->imageWidth(), 
	                            faceRecognizer->imageHeight());
	videoScanner->setScanningMode( VideoScanner::SMFullImage);
	return exec();
}

void UIRecognizerTest:: safeClose()
{
	videoInput->resize( prevWidth_, prevHeight_);
	videoInput->setVideoSource( (VideoInput::VideoSource) prevSource_);
	videoScanner->setImageSize( prevWidth_, prevHeight_);
	videoScanner->setScanningMode( (VideoScanner::ScanningMode) prevMode_);
	done( QDialog::Accepted);
}

void UIRecognizerTest:: updateStats()
{
	labelTested->setText( QString::number( all_));
	labelGood->setText( QString::number( good_));
	labelPGood->setText( "(" + QString::number( (double) 100 * good_ / all_, 'f', 2) + "%)");
	labelBad->setText( QString::number( bad_));
	labelPBad->setText( "(" + QString::number( (double) 100 * bad_ / all_, 'f', 2)+ "%)");
	labelNot->setText( QString::number( not_));
	labelPNot->setText( "(" + QString::number( (double) 100 * not_ / all_, 'f', 2)+ "%)");
	labelDuration->setText( QString::number( testDuration_.update() / 1e6) 
	                        + " [s]");
	labelProcessTime->setText( QString::number( 
	                                   recognitionDuration_.averageTime()) 
	                           + " [us]");
}
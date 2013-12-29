#include "uirecognitionsettings.h"
#include "uigetvalue.h"
#include "uirecognizerlearning.h"
#include "uirecognizertest.h"
#include "FaceRecognizer.h"
#include "VideoInput.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>

//
UIRecognitionSettings::UIRecognitionSettings( FaceRecognizer *fr,
                                              VideoInput *vi, 
                                              UIGetValue *gv,
	                                          UIRecognizerLearning *rl,
	                                          UIRecognizerTest *rt,
                                              QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	uiGetValue = gv;
	uiRecognizerLearning = rl;
	uiRecognizerTest = rt;
	faceRecognizer = fr;
	videoInput = vi;
	indexEigenface = 0;
	connect( spinClassesNumber, SIGNAL(valueChanged(int)), 
	         this,              SLOT( resizeEnable()));
	connect( spinNeuronsPerClass, SIGNAL(valueChanged(int)), 
	         this,                SLOT( resizeEnable()));
}
//
void UIRecognitionSettings::updateSettings()
{
	doubleSpinBox->setValue( faceRecognizer->faceThreshold());
	spinLearningSpeed->setValue( faceRecognizer->initialSpeed());
	labelEigenfacesNumber->setText( QString::number(
	                                    faceRecognizer->numberOfEigenfaces() ));
	labelEigenfaceWidth->setText( QString::number(
	                                            faceRecognizer->imageWidth() ));
	labeEigenfaceHeight->setText( QString::number(
	                                           faceRecognizer->imageHeight() )); 
	eigenfaceUpdate();
	sliderEigenfaces->setMaximum( faceRecognizer->numberOfEigenfaces() - 1);
	spinClassesNumber->setValue( faceRecognizer->individuals());
	spinNeuronsPerClass->setValue( faceRecognizer->neuronsPerIndividual());
	labelKohonenLayer->setText( QString::number(
	                                           faceRecognizer->layerKohonen()));
	labelGrossbergLayer->setText( QString::number( 
	                                         faceRecognizer->layerGrossberg()));
}

void UIRecognitionSettings:: eigenfaceUpdate()
{
	labelEigenfaceIndex->setText( "eigenface no. " + 
	                             QString::number( indexEigenface + 1));
	QImage pic(
	           faceRecognizer->eigenface( indexEigenface),
	           faceRecognizer->imageWidth(), 
			   faceRecognizer->imageHeight(),
			   faceRecognizer->imageWidth(),
			   QImage::Format_Indexed8
			  );
	pic.setColorTable( videoInput->colorTable());
	pixmapEigenface->setPixmap( QPixmap::fromImage( pic));	
}

void UIRecognitionSettings::on_buttonLoadRecognizer_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setFileMode( QFileDialog::ExistingFile);
	fileDialog.setNameFilter( tr("Neural Network Weights (*.nnw)"));
	if (fileDialog.exec())
	{
		buttonLoadRecognizer->setEnabled( false);
		buttonSaveRecognizer->setEnabled( false);
		labelRecognizerFile->setText( tr("Loading..."));		
		qApp->processEvents(QEventLoop::AllEvents, 100);		
		try
		{
			faceRecognizer->load( fileDialog.selectedFiles()[0].toLocal8Bit());
			labelRecognizerFile->setText( tr("Loaded from: ") +
			              fileDialog.selectedFiles()[0].remove(0, 
			              fileDialog.selectedFiles()[0].lastIndexOf("/") + 1) );
			labelRecognizerFile->setToolTip( fileDialog.selectedFiles()[0]);
		}
		catch( exception &e)
		{
			QMessageBox::warning(this, tr("Loading"), e.what());
			labelRecognizerFile->setText( tr("Loaded from: no file"));
		}		
		QMessageBox::information( this, tr("Loading"), 
		                     tr("faceRecognizer weights loaded successfully."));
		buttonLoadRecognizer->setEnabled( true);
		buttonSaveRecognizer->setEnabled( true); 
		updateSettings();
	}
}

void UIRecognitionSettings::on_buttonSaveRecognizer_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	fileDialog.setFileMode( QFileDialog::AnyFile);
	fileDialog.setNameFilter( tr("Neural Network Weights (*.nnw)"));
	if (fileDialog.exec())
	{
		buttonLoadRecognizer->setEnabled( false);
		buttonSaveRecognizer->setEnabled( false);		
		QString loadedFile = labelRecognizerFile->text(); 
		labelRecognizerFile->setText( tr("Saving..."));
		qApp->processEvents(QEventLoop::AllEvents, 100);
		faceRecognizer->save( fileDialog.selectedFiles()[0].toLocal8Bit());
		QMessageBox::information( this, tr("Saving"), 
		                      tr("faceRecognizer weights saved successfully."));
		labelRecognizerFile->setText( loadedFile);
		buttonLoadRecognizer->setEnabled( true);
		buttonSaveRecognizer->setEnabled( true);;
	}
}

void UIRecognitionSettings::on_buttonResizeRecognizer_clicked()
{
	QMessageBox msgBox;
	msgBox.setText(tr("<b>Neural network will be reset!</b>"));
	msgBox.setInformativeText(tr("Do you still want to resize recognizer?"));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);
	msgBox.setDetailedText(tr("Resizing requires building completly new network\
	. All current values will be lost. It is advise to save current recognizer \
	weights before resizing."));
	if (msgBox.exec() == QMessageBox::Yes)
	{		
		faceRecognizer->resizeRecognizer( spinClassesNumber->value(),
		                        spinNeuronsPerClass->value());	
		updateSettings();
		buttonResizeRecognizer->setEnabled( false );
		emit recognizerOutputSize( log2( spinClassesNumber->value() ) + 1);
	}
}

void UIRecognitionSettings::on_buttonEigenfacesLoad_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setFileMode( QFileDialog::ExistingFile);
	fileDialog.setNameFilter( tr("Comma Seperated Values (*.csv)"));
	QMessageBox::information( this, tr("Eigenface format"), 
		tr("First specify eigenfaces width and height. Then choose float matrix\
		 file, which rows are next eigenfaces."));
	int width = uiGetValue->askForInt("Eigenfaces", 
	                                  "Enter eigenfaces image width:",
	                                  faceRecognizer->imageWidth());
	int height = uiGetValue->askForInt("Eigenfaces", 
	                                   "Enter eigenfaces image height:",
	                                   faceRecognizer->imageHeight());
	if (fileDialog.exec())
	{
		buttonEigenfacesLoad->setEnabled( false);		
		labelEigenfacesFile->setText( tr("Loading..."));		
		qApp->processEvents(QEventLoop::AllEvents, 100);		
		try
		{
			faceRecognizer->loadEigenfaces( width, height,
			               fileDialog.selectedFiles()[0].toLocal8Bit() );
			labelEigenfacesFile->setText( "Loaded from: " +
			              fileDialog.selectedFiles()[0].remove(0, 
			              fileDialog.selectedFiles()[0].lastIndexOf("/") + 1) );
			labelEigenfacesFile->setToolTip( fileDialog.selectedFiles()[0]);
			updateSettings();
			QMessageBox::information( this, tr("Loading"), 
			                             tr("Eigenfaces loaded successfully."));
		}
		catch( exception &e)
		{
			QMessageBox::warning(this, tr("Loading"), e.what());
			labelEigenfacesFile->setText( tr("Loaded from: no file"));
		}		
		buttonEigenfacesLoad->setEnabled( true); 
	}
}

void UIRecognitionSettings:: resizeEnable()
{
	bool enable = (spinClassesNumber->value() != 
	               faceRecognizer->individuals()) ||
	              (spinNeuronsPerClass->value() != 
	               faceRecognizer->neuronsPerIndividual());
	buttonResizeRecognizer->setEnabled( enable);
}

void UIRecognitionSettings::on_sliderEigenfaces_valueChanged(int value)
{
	indexEigenface = value;
	eigenfaceUpdate();
}

void UIRecognitionSettings::on_buttonRecognizerReset_clicked()
{
	faceRecognizer->reset();
}

void UIRecognitionSettings::on_buttonRecognizerLearn_clicked()
{
	uiRecognizerLearning->safeExec();
}

void UIRecognitionSettings::on_buttonRecognizerTest_clicked()
{
	uiRecognizerTest->safeExec();
}


void UIRecognitionSettings::on_doubleSpinBox_valueChanged(double threshold)
{
	faceRecognizer->setFaceThreshold( threshold);
}

void UIRecognitionSettings::on_buttonEigenfaces_clicked()
{
	stackedWidget->setCurrentWidget( pageEigenfaces);
}

void UIRecognitionSettings::on_buttonNetwork_clicked()
{
	stackedWidget->setCurrentWidget( pageNetwork);
}

void UIRecognitionSettings::on_buttonMisc_clicked()
{
	stackedWidget->setCurrentWidget( pageMisc);
}

void UIRecognitionSettings::on_buttonLoadAvg_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setFileMode( QFileDialog::ExistingFile);
	fileDialog.setNameFilter( tr("Comma Seperated Values (*.csv)"));
	if (fileDialog.exec())
	{
		QString filename = fileDialog.selectedFiles()[0];
		Matrix avg;
		avg.loadFromCsv( filename.toLocal8Bit());
		if (avg.isVector())
		{
			if (avg.length() == faceRecognizer->numberOfEigenfaces())
			{
				faceRecognizer->setProjectionAvg( avg);
				labelProjectionAvg->setText("face projection AVG: " +
				                  filename.mid( filename.lastIndexOf('/') + 1));
			}
			else
				QMessageBox::warning(this, tr("Projection Avg"), 
				"Vector should have " + 
				QString::number(faceRecognizer->numberOfEigenfaces()) +
				" samples.");
		}
		else
			QMessageBox::warning(this, tr("Projection Avg"), 
				"Selected file doesn't contain vector.");
	}
}

void UIRecognitionSettings::on_buttonLoadStd_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setFileMode( QFileDialog::ExistingFile);
	fileDialog.setNameFilter( tr("Comma Seperated Values (*.csv)"));
	if (fileDialog.exec())
	{
		QString filename = fileDialog.selectedFiles()[0];
		Matrix std;
		std.loadFromCsv( filename.toLocal8Bit());
		if (std.isVector())
		{
			if (std.length() == faceRecognizer->numberOfEigenfaces())
				{
					faceRecognizer->setProjectionStd( std);
					labelProjectionStd->setText("face projection STD: " +
					              filename.mid( filename.lastIndexOf('/') + 1));
				}
			else
				QMessageBox::warning(this, tr("Projection Std"), 
				"Vector should have " + 
				QString::number(faceRecognizer->numberOfEigenfaces()) +
				" samples.");
		}
		else
			QMessageBox::warning(this, tr("Projection Std"), 
				"Selected file doesn't contain vector.");
	}
}

void UIRecognitionSettings::on_spinLearningSpeed_valueChanged( double speed)
{
	faceRecognizer->setInitialSpeed( speed );
}

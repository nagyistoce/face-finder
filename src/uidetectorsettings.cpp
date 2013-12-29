#include "uidetectorsettings.h"
#include <QFileDialog>
#include <QMessageBox>
#include "VideoInput.h"
#include "FaceDetector.h"
//
UIDetectorSettings::UIDetectorSettings( VideoInput *vi, FaceDetector *fd, 
                                        QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	videoInput = vi;
	faceDetector = fd;
}
//
void UIDetectorSettings::updateSettings()
{	
	editInputWidth->setValue( videoInput->detectorWidth());
	editInputHeight->setValue( videoInput->detectorHeight());
	editLayerSize->setValue( faceDetector->layerSize());
	spinPositionStep->setValue( videoInput->scanRect.step());
	spinAreaRatio->setValue( videoInput->scanRect.ratio());
	spinMinX->setValue( videoInput->scanRect.minX());
	spinMinY->setValue( videoInput->scanRect.minY());
	spinMaxX->setValue( videoInput->scanRect.maxX());
	spinMaxY->setValue( videoInput->scanRect.maxY());
	spinFaceMaxWidth->setValue( videoInput->scanRect.maxWidth());
	spinFaceMaxHeight->setValue( videoInput->scanRect.maxHeight());
	spinFaceMinWidth->setValue( videoInput->scanRect.minWidth());
	spinFaceMinHeight->setValue( videoInput->scanRect.minHeight());
	spinFaceX->setValue( videoInput->scanRect.defaultX());
	spinFaceY->setValue( videoInput->scanRect.defaultY());
}

void UIDetectorSettings::on_buttonBox_accepted()
{
	//setting mode
	if (detectionMethodCombo->currentIndex() == 0) //eigenfaces detection
		mode = ComputationThread:: CMRecognizer;
	else //bpnn detection
	{
		if (radioScanMode->isChecked())
			mode = ComputationThread:: CMDetectorScanning;
		if (radioLearnMode->isChecked())
			mode = ComputationThread:: CMDetectorLearning;
		if (radioIdleMode->isChecked())
			mode = ComputationThread:: CMIdle;
	}
	emit changingComputationMode( mode);
	//scanning mode
	VideoInput::ScanningMode scanMode = VideoInput::SMNone;
	if (radioScannerFixed->isChecked())
		scanMode = VideoInput::SMFixed;
	if (radioScannerVariable->isChecked())	
	{
		if (checkVariablePosition->isChecked())
			scanMode = (VideoInput::ScanningMode) 
			           (scanMode | VideoInput::SMPosition);
		if (checkVariableArea->isChecked())
			scanMode = (VideoInput::ScanningMode) 
			           (scanMode | VideoInput::SMDistance);
	}
	emit changingScanningMode( scanMode);
	//resizing detector
	bool mustResize = false;	
	if (editInputWidth->value()  != videoInput->width()   ||
	    editInputHeight->value() != videoInput->height())
	    {
			videoInput->setDetectorWidth( editInputWidth->value()); 
			videoInput->setDetectorHeight( editInputHeight->value());
			mustResize = true;	    	
    	}
	if (editLayerSize->value() != faceDetector->layerSize())
		mustResize = true;
	if (mustResize)
		faceDetector->resizeDetector( editLayerSize->value(), videoInput);	
	//close dialog	
	close();
}

void UIDetectorSettings::on_buttonBox_rejected()
{
	close();
}


void UIDetectorSettings::on_buttonDetectorLoad_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setFileMode( QFileDialog::ExistingFile);
	fileDialog.setNameFilter( tr("Neural Network Weights (*.nnw)"));
	if (fileDialog.exec())
	{
		buttonDetectorLoad->setEnabled( false);
		buttonDetectorSave->setEnabled( false);
		labelDetectorFile->setText( tr("Loading..."));		
		qApp->processEvents(QEventLoop::AllEvents, 100);		
		try
		{
			faceDetector->load( 
			               fileDialog.selectedFiles()[0].toLocal8Bit() );
			labelDetectorFile->setText( tr("Loaded from: ") +
			              fileDialog.selectedFiles()[0].remove(0, 
			              fileDialog.selectedFiles()[0].lastIndexOf("/") + 1) );
			labelDetectorFile->setToolTip( fileDialog.selectedFiles()[0]);
		}
		catch( exception &e)
		{
			QMessageBox::warning(this, tr("Loading"), e.what());
			labelDetectorFile->setText( tr("Loaded from: no file"));
		}		
		QMessageBox::information( this, tr("Loading"), 
		                      tr("faceDetector weights loaded successfully."));
		buttonDetectorLoad->setEnabled( true);
		buttonDetectorSave->setEnabled( true); 
	}
}

void UIDetectorSettings::on_buttonDetectorSave_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	fileDialog.setFileMode( QFileDialog::AnyFile);
	fileDialog.setNameFilter( tr("Neural Network Weights (*.nnw)"));
	if (fileDialog.exec())
	{
		buttonDetectorLoad->setEnabled( false);
		buttonDetectorSave->setEnabled( false);		
		QString loadedFile = labelDetectorFile->text(); 
		labelDetectorFile->setText( tr("Saving..."));
		qApp->processEvents(QEventLoop::AllEvents, 100);
		faceDetector->save( fileDialog.selectedFiles()[0].toLocal8Bit());
		QMessageBox::information( this, tr("Saving"), 
		                        tr("faceDetector weights saved successfully."));
		labelDetectorFile->setText( loadedFile);
		buttonDetectorLoad->setEnabled( true);
		buttonDetectorSave->setEnabled( true);
	}
}

void UIDetectorSettings::on_buttonNetworkResize_clicked()
{
	QMessageBox msgBox;
	msgBox.setText(tr("<b>Neural network will be reset!</b>"));
	msgBox.setInformativeText(tr("Do you still want to resize detector?"));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);
	msgBox.setDetailedText(tr("Resizing requires building completly new network\
	. All current values will be lost. It is advise to save current detector \
	before resizing."));
	if (msgBox.exec() == QMessageBox::Yes)
	{
		videoInput->resize( editInputWidth->value(), editInputHeight->value());
		faceDetector->resizeDetector( editLayerSize->value(),
		                              videoInput);	
	}
}

void UIDetectorSettings:: resizeEnable()
{
	bool enable = (editInputWidth->value() != 
	               videoInput->width()) ||
	              (editInputHeight->value() != 
	               videoInput->height()) ||
	              (editLayerSize->value() != 
	               faceDetector->layerSize());
	buttonNetworkResize->setEnabled( enable);
}

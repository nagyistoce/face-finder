#include "uiscannersettings.h"
#include <QFileDialog>
#include <QMessageBox>
#include "VideoScanner.h"
//
UIScannerSettings::UIScannerSettings( VideoScanner *vi,
                                      QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	videoScanner = vi;
}

void UIScannerSettings::updateSettings()
{	
	spinPositionStep->setValue( videoScanner->step());
	spinAreaRatio->setValue( videoScanner->ratio());
	spinMinX->setValue( videoScanner->minX());
	spinMinY->setValue( videoScanner->minY());
	spinMaxX->setValue( videoScanner->maxX());
	spinMaxY->setValue( videoScanner->maxY());
	spinFaceMaxWidth->setValue( videoScanner->maxWidth());
	spinFaceMaxHeight->setValue( videoScanner->maxHeight());
	spinFaceMinWidth->setValue( videoScanner->minWidth());
	spinFaceMinHeight->setValue( videoScanner->minHeight());
	spinFaceX->setValue( videoScanner->defaultX());
	spinFaceY->setValue( videoScanner->defaultY());
}

void UIScannerSettings:: accept()
{
	//scanning mode
	VideoScanner::ScanningMode scanMode = VideoScanner::SMNone;
	if (radioScannerFixed->isChecked())
		scanMode = VideoScanner::SMFixed;
	if (radioScannerVariable->isChecked())	
	{
		if (checkVariablePosition->isChecked())
			scanMode = (VideoScanner::ScanningMode) 
			           (scanMode | VideoScanner::SMPosition);
		if (checkVariableArea->isChecked())
			scanMode = (VideoScanner::ScanningMode) 
			           (scanMode | VideoScanner::SMDistance);
	}
	videoScanner->setScanningMode( scanMode);

	//scanner properties
	videoScanner->setStep( spinPositionStep->value());
	videoScanner->setRatio( spinAreaRatio->value());
	videoScanner->setMinX( spinMinX->value());
	videoScanner->setMinY( spinMinY->value());
	videoScanner->setMaxX( spinMaxX->value());
	videoScanner->setMaxY( spinMaxY->value());
	videoScanner->setMaxWidth( spinFaceMaxWidth->value());
	videoScanner->setMaxHeight( spinFaceMaxHeight->value());
	videoScanner->setMinWidth( spinFaceMinWidth->value());
	videoScanner->setMinHeight( spinFaceMinHeight->value());
	videoScanner->setDefaultX( spinFaceX->value());
	videoScanner->setDefaultY( spinFaceY->value());
	
	done( QDialog::Accepted );
}

void UIScannerSettings:: reject()
{
	done( QDialog::Rejected );
}


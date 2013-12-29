#ifndef UIDETECTORSETTINGS_H
#define UIDETECTORSETTINGS_H
//
#include <QDialog>
#include "ui_detector_settings.h"
#include "computationthread.h"
class VideoInput;
class FaceDetector;
//
class UIDetectorSettings : public QDialog, public Ui::dsDialog
{
Q_OBJECT
public:
	UIDetectorSettings( VideoInput *vi, FaceDetector *fd, 
	                    QWidget * parent = 0, Qt::WFlags f = 0 );
	void updateSettings();
signals:
	void changingComputationMode( ComputationThread::ComputationMode);
	void changingScanningMode( VideoInput::ScanningMode);
private slots:
	void on_buttonDetectorLoad_clicked();
	void on_buttonDetectorSave_clicked();
	void on_buttonNetworkResize_clicked();
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	void resizeEnable();
private:
	VideoInput * videoInput;
	FaceDetector * faceDetector;
	ComputationThread::ComputationMode mode;
};
#endif






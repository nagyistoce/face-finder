#ifndef UIRECOGNIZERTEST_H
#define UIRECOGNIZERTEST_H
//
#include <QDialog>
#include "ui_recognizer_testing.h"
#include <QMap>
#include <vector>
#include <string>
#include "MicroCounter.h"

class QPictureListEditor;
class VideoInput;
class VideoScanner;
class FaceRecognizer; 


class UIRecognizerTest : public QDialog, public Ui::rtDialog
{
Q_OBJECT
public:
	UIRecognizerTest( VideoInput *vi, VideoScanner *vs, FaceRecognizer *fr,
	                  QPictureListEditor *ple, 
	                  QWidget * parent = 0, Qt::WFlags f = 0 );
	int safeExec();
signals:
	void testRequest();
private slots:
	void on_buttonEdit_clicked();
	void on_buttonStart_clicked();
	void on_buttonClose_clicked();
	void performTest();
private:
	void safeClose();
	void updateStats();
	QPictureListEditor *pictureListEditor;
	VideoInput *videoInput;
	VideoScanner *videoScanner;
	FaceRecognizer *faceRecognizer;
	int prevWidth_, prevHeight_, prevSource_, prevMode_;
	QMap<QString,int> pictureList_;	
	std::vector<std::string> fileList_;
	bool testing_;
	int all_, good_, bad_, not_;
	MicroCounter testDuration_, recognitionDuration_;
};
#endif






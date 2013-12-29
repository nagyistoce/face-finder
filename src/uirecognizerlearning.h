#ifndef UIRECOGNIZERLEARNING_H
#define UIRECOGNIZERLEARNING_H
//
#include <QDialog>
#include <QMap>
#include <vector>
#include <string>
#include "ui_recognizer_learning.h"

class VideoInput;
class VideoScanner;
class FaceRecognizer;
class QPictureListEditor;

class UIRecognizerLearning : public QDialog, public Ui::rlDialog
{
Q_OBJECT
public:
	UIRecognizerLearning( VideoInput *vi, VideoScanner *vs, FaceRecognizer *fr,
	                      QPictureListEditor *ple,
	                      QWidget * parent = 0, Qt::WFlags f = 0 );
	int safeExec();
private slots:
	void on_buttonEdit_clicked();
	void on_buttonClose_clicked();
	void on_buttonStart_clicked();
	void on_buttonCancel_clicked();
private:
	void safeClose();
	bool learning_;	
	VideoInput *videoInput;
	VideoScanner *videoScanner;
	FaceRecognizer *faceRecognizer;
	int prevWidth_, prevHeight_, prevSource_, prevMode_;
	QMap<QString,int> pictureList_;	
	std::vector<std::string> fileList_;
	QPictureListEditor *pictureListEditor;
};
#endif







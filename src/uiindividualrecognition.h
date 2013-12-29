#ifndef UIINDIVIDUALRECOGNITION_H
#define UIINDIVIDUALRECOGNITION_H
//
#include <QWidget>
#include "ui_individual_recognition.h"
#include "VideoInput.h"
//
class FaceRecognizer;

class UIIndividualRecognition : public QDialog, public Ui_irDialog
{
Q_OBJECT
public:
	UIIndividualRecognition( VideoInput *vi, FaceRecognizer *fr, 
	                         QWidget * parent = 0, Qt::WFlags f = 0 );
	void setId( int id);
private slots:
	void on_buttonAdd_clicked();
	void on_buttonTest_clicked();
	void on_buttonLearn_clicked();
	void on_buttonClear_clicked();
	void on_closeButton_clicked();
private:
	void enableControls(bool enabled);
	int id_;
	VideoInput *videoInput;
	FaceRecognizer *faceRecognizer;
	int prevWidth_, prevHeight_, prevSource_, prevMode_;
	std::vector<string> filesList_;
};
#endif






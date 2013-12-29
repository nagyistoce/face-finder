#ifndef UIRECOGNITIONSETTINGS_H
#define UIRECOGNITIONSETTINGS_H
//
#include <QDialog>
#include "ui_recognition_settings.h"

class FaceRecognizer;
class VideoInput;
class UIRecognizerLearning;
class UIRecognizerTest;
class UIGetValue;

//
class UIRecognitionSettings : public QDialog, public Ui::RecognitionSettingsDialog
{
Q_OBJECT
public:
	UIRecognitionSettings( FaceRecognizer *fr,
	                       VideoInput *vi,
	                       UIGetValue *gv,
	                       UIRecognizerLearning *rl,
	                       UIRecognizerTest *rt,
	                       QWidget * parent = 0, Qt::WFlags f = 0 );
	void updateSettings();
signals:
	void recognizerOutputSize( int size);
private slots:
	void on_spinLearningSpeed_valueChanged(double );
	void on_buttonEigenfaces_clicked();
	void on_buttonNetwork_clicked();
	void on_buttonMisc_clicked();
	void on_buttonLoadAvg_clicked();
	void on_buttonLoadStd_clicked();
	void on_doubleSpinBox_valueChanged(double );
	void on_buttonRecognizerTest_clicked();
	void on_buttonRecognizerReset_clicked();
	void on_buttonRecognizerLearn_clicked();
	void on_sliderEigenfaces_valueChanged(int value);
	void on_buttonEigenfacesLoad_clicked();
	void on_buttonLoadRecognizer_clicked();
	void on_buttonSaveRecognizer_clicked();
	void on_buttonResizeRecognizer_clicked();
	void resizeEnable();
private:
	void eigenfaceUpdate();
	UIRecognizerLearning *uiRecognizerLearning; 
	UIRecognizerTest *uiRecognizerTest;
	UIGetValue *uiGetValue;
	int indexEigenface;
	FaceRecognizer *faceRecognizer;
	VideoInput *videoInput;
};
#endif









#ifndef UIDATABASESETTINGS_H
#define UIDATABASESETTINGS_H
//
#include <QDialog>
#include "ui_database_settings.h"
#include "FaceDatabase.h" // need to be included here, because Individual object
                          // is needed in UIDatabaseSettings body.
class UIAddIndividual;
class UIGetValue;

//
class UIDatabaseSettings : public QDialog, public Ui::databaseDialog
{
Q_OBJECT
public:
	UIDatabaseSettings( FaceDatabase *fdb, UIGetValue *gv, UIAddIndividual *ai,
	                    QWidget * parent = 0, Qt::WFlags f = 0 );
	void updateSettings();
	
private slots:
	void on_buttonClear_clicked();
	void on_buttonIndividualAdd_clicked();
	void on_buttonDatabaseLoad_clicked();
	void on_buttonDatabaseSave_clicked();
	void on_buttonIndividualPrev_clicked();
	void on_buttonIndividualNext_clicked();
	void on_buttonIndividualFindId_clicked();
	void on_buttonIndividualFindName_clicked();
	void on_buttonIndividualEdit_clicked();
	void on_buttonIndividualRemove_clicked();
	
private:
	void updateIndividual();
	void updateDatabase();
	void enableControls(bool enable);
	int indexDatabase;
	FaceDatabase *faceDatabase;
	UIAddIndividual *uiAddIndividual;
	UIGetValue *uiGetValue;	
	Individual individual;	
};
#endif







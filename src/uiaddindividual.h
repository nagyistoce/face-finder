#ifndef UIADDINDIVIDUAL_H
#define UIADDINDIVIDUAL_H
//
#include <QDialog>
#include "ui_add_individual.h"
#include "FaceDatabase.h"
//
class UIAddIndividual : public QDialog, public Ui::aiDialog
{
Q_OBJECT
public:
	UIAddIndividual( QWidget * parent = 0, Qt::WFlags f = 0 );
	void updateSettings( int id);
	const Individual& individual() const;
	void setIndividual( const Individual&);
private slots:
	void on_buttonPictureLoad_clicked();
	void on_buttonInfoLoad_clicked();
	void on_buttonAdd_clicked();
	void on_buttonCancel_clicked();
private:
	Individual individual_;
};

inline const Individual& UIAddIndividual:: individual() const
{
	return individual_;
}
#endif






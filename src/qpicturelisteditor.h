#ifndef QPICTURELISTEDITOR_H
#define QPICTURELISTEDITOR_H
//
#include <QDialog>
#include <QMap>
#include <QList>
#include <QString>
#include "ui_picture_list_editor.h"
#include <vector>
#include <string>
//
class UIGetValue;

class QPictureListEditor : public QDialog, public Ui::PictureListEditor
{
Q_OBJECT
public:
	QPictureListEditor( UIGetValue *gv, 
	                    QWidget * parent = 0, Qt::WFlags f = 0 );
	void accept();
	void reject();
	void setList( std::vector<std::string>& paths, QMap<QString,int>& names );
	void getList( std::vector<std::string>& paths, QMap<QString,int>& names );
public slots:
	void setMaxId( int max);
private slots:
	void on_generateButton_clicked();
	void on_buttonRemove_clicked();
	void on_buttonClear_clicked();
	void on_buttonSave_clicked();
	void on_buttonLoad_clicked();
	void on_buttonAdd_clicked();
	void on_tableWidget_cellClicked(int row, int column);
private:
	void updateList(QString id, const QList<QString>& list);
	UIGetValue *uiGetValue;
	int lastId_, maxId_;
	std::vector<std::string> fileList_;
	QMap<QString,int> pictureList_;
};

inline void QPictureListEditor:: setMaxId( int max)
{
	maxId_ = max;
}
#endif






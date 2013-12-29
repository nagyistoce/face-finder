#include "qpicturelisteditor.h"
#include "uigetvalue.h"
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QFile>
#include <QDataStream>
#include <QMessageBox>
#include <fstream>
#include <string>
#include <iostream>
//
QPictureListEditor::QPictureListEditor( UIGetValue *gv,
                                        QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	uiGetValue = gv;
	tableWidget->setColumnCount( 3);
	QTableWidgetItem *item;
	item = new QTableWidgetItem( "picture");
	tableWidget->setHorizontalHeaderItem(0, item);
	item = new QTableWidgetItem( "class id");
	tableWidget->setHorizontalHeaderItem(1, item);
	item = new QTableWidgetItem( "path");
	tableWidget->setHorizontalHeaderItem(2, item);
	tableWidget->setRowCount( 0);
	fileList_.clear();
	pictureList_.clear();
	lastId_ = 0;
}
//

void QPictureListEditor::on_buttonRemove_clicked()
{
	// TODO
}

void QPictureListEditor::on_buttonClear_clicked()
{
	tableWidget->clear();
	tableWidget->setRowCount(0);
	fileList_.clear();
}

void QPictureListEditor::on_buttonSave_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setFileMode( QFileDialog::AnyFile);
	fileDialog.setAcceptMode( QFileDialog::AcceptSave);
	fileDialog.setNameFilter( tr("Image Files List (*.ifl)"));
	if (fileDialog.exec())
	{
		QFile file( fileDialog.selectedFiles()[0]);
		file.open( QIODevice::WriteOnly);
		QDataStream dataStream( &file);
		QStringList list;
		QString id = tableWidget->item( 0, 1)->text();
		for (int i = 0; i < tableWidget->rowCount(); i++)
		{
			if (id == tableWidget->item( i, 1)->text())
				list.append( tableWidget->item( i, 2)->text());
			else
			{
				dataStream << id;
				dataStream << list;
				id = tableWidget->item( i, 1)->text();
				list.clear();
				list.append( tableWidget->item( i, 2)->text());
			}
		}
		//save last pair
		dataStream << id;
		dataStream << list;
		file.close();
	}
}

void QPictureListEditor::on_buttonLoad_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setFileMode( QFileDialog::ExistingFile);
	fileDialog.setNameFilter( tr("Image Files List (*.ifl)"));
	if (fileDialog.exec())
	{
		QFile file( fileDialog.selectedFiles()[0]);
		file.open( QIODevice::ReadOnly);
		QDataStream dataStream( &file);
		QStringList list;
		QString id;
		while (!dataStream.atEnd())
		{
			dataStream >> id;
			dataStream >> list;
			updateList( id, list);
			list.clear();
		}
		file.close();
	}
}

void QPictureListEditor::on_buttonAdd_clicked()
{
	lastId_ = uiGetValue->askForInt( "Individual id?", 
	                        "Enter class number to assign with with pictures:", 
	                         lastId_, 0, maxId_);
	QString id = QString::number( lastId_);
	QFileDialog fileDialog;
	fileDialog.setFileMode( QFileDialog::ExistingFiles);
	fileDialog.setNameFilter( tr("images (*.pgm)"));
	if (fileDialog.exec())
	{
		updateList( id, fileDialog.selectedFiles());
	}
}

void QPictureListEditor::on_tableWidget_cellClicked(int row, int /*column*/)
{
	labelPicture->setPixmap( QPixmap(tableWidget->item(row, 2)->text()));
}

void QPictureListEditor::accept()
{
	pictureList_.clear();
	fileList_.clear();
	for (int i = 0; i < tableWidget->rowCount(); i++)
	{
		fileList_.push_back( std::string( tableWidget->item(i, 2)->
		                                  text().toLocal8Bit() ) );
		pictureList_.insert( tableWidget->item(i, 0)->text(),
		                     tableWidget->item(i, 1)->text().toInt() );
	}
	done( QDialog::Accepted);
}

void QPictureListEditor::reject()
{
	done( QDialog::Rejected);
}

void QPictureListEditor::updateList(QString id, const QList<QString>& list)
{
	int offset = tableWidget->rowCount();
	QTableWidgetItem *item;
	tableWidget->setRowCount( offset + list.size());
	for( int i = 0; i < list.size(); i++)
	{
		QString name = list[i];
		name.remove( 0, list[i].lastIndexOf( "/") + 1);
		item = new QTableWidgetItem( QIcon(list[i]), name);		                     ;
		tableWidget->setItem( offset + i, 0, item);
		item = new QTableWidgetItem(id);
		tableWidget->setItem( offset + i, 1, item);
		item = new QTableWidgetItem( list[i]);
		tableWidget->setItem( offset + i, 2, item);		
	}
}

void QPictureListEditor::setList( std::vector<std::string>& paths, 
                                  QMap<QString,int>& names )
{	
	fileList_ = paths;
	pictureList_ = names;
	tableWidget->clear();
	tableWidget->setRowCount( fileList_.size());
	QTableWidgetItem *item;
	QString name, path;
	for (int i = 0; i < tableWidget->rowCount(); i++)
	{
		path = QString( fileList_[i].c_str());
		name = path.mid( path.lastIndexOf('/') + 1);
		item = new QTableWidgetItem( QIcon(path), name);		                     ;
		tableWidget->setItem( i, 0, item);
		item = new QTableWidgetItem(pictureList_.value( name));
		tableWidget->setItem( i, 1, item);
		item = new QTableWidgetItem( path);
		tableWidget->setItem( i, 2, item);		
	}
}

void QPictureListEditor::getList( std::vector<std::string>& paths, 
                                  QMap<QString,int>& names )
{
	paths = fileList_;
	names = pictureList_;
}

void QPictureListEditor::on_generateButton_clicked()
{
	int id;
	std::string path, fpath;
	QFileDialog fileDialog;
	fileDialog.setFileMode( QFileDialog::AnyFile);
	fileDialog.setAcceptMode( QFileDialog::AcceptSave);
	fileDialog.setNameFilter( tr("Standard Picture List (*.spl)"));
	if (fileDialog.exec())
	{
		QString qstring = fileDialog.selectedFiles()[0];
		fpath = std::string( (const char*) qstring.toLocal8Bit(),
		                      qstring.length() );
	}	
	std::fstream f(fpath.c_str(), std::ios::out);
	if (!f) 
	{
		QMessageBox::critical( this, tr("Generating spl"), 
		                     tr("File couldn't be open."));
		return;
	}
	for (int i = 0; i < tableWidget->rowCount(); i++)
	{
		id = tableWidget->item(i, 1)->text().toInt();
		path = std::string( tableWidget->item(i, 2)->text().toLocal8Bit());		
		f << id << path << std::endl;
	}
}

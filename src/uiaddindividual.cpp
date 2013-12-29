#include "uiaddindividual.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QByteArray>
#include <QTextCodec>
//
UIAddIndividual::UIAddIndividual( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
}
void UIAddIndividual:: setIndividual( const Individual& individual)
{
	individual_ = individual;
}
//
void UIAddIndividual:: updateSettings( int id)
{
	editName->selectAll();
	editName->insert( QTextCodec::codecForLocale()->toUnicode(
	                individual_.name().c_str(), individual_.name().length()));
	spinId->setValue( id);
	labelPicture->setPixmap( QPixmap::fromImage( QImage::fromData(
	(const uchar*) individual_.imageAddress(), individual_.imageLength(), 0)));
	textEditInfo->setPlainText( QTextCodec::codecForLocale()->toUnicode(
	    individual_.description().c_str(), individual_.description().length()));	
}

void UIAddIndividual::on_buttonPictureLoad_clicked()
{
	QFileDialog fileDialog;	
	fileDialog.setFileMode( QFileDialog::ExistingFile);
	fileDialog.setNameFilter( tr("Image files (*.jpg *.png *pgm)"));
	if (fileDialog.exec())
	{
		buttonPictureLoad->setEnabled( false);
		qApp->processEvents(QEventLoop::AllEvents, 100);
		try
		{
			individual_.setImage( fileDialog.selectedFiles()[0].toLocal8Bit() );
			labelPicture->setPixmap( QPixmap::fromImage( QImage::fromData(
			                          (const uchar*) individual_.imageAddress(), 
			                          individual_.imageLength(), 0)));
			
		}
		catch( exception &e)
		{
			QMessageBox::warning(this, tr("Loading"), e.what());
		}
		buttonPictureLoad->setEnabled( true); 
	}
}

void UIAddIndividual::on_buttonInfoLoad_clicked()
{
	QFileDialog fileDialog;	
	fileDialog.setFileMode( QFileDialog::ExistingFile);	
	if (fileDialog.exec())
	{
		QFile file( fileDialog.selectedFiles()[0]);
		QByteArray data = file.readAll();
		textEditInfo->setText( QString(data));
	}
}

void UIAddIndividual::on_buttonAdd_clicked()
{
	individual_.setId( spinId->value());
	individual_.setName( (std::string) editName->text().toLocal8Bit() );
	individual_.setDescription( (std::string)
	                                textEditInfo->toPlainText().toLocal8Bit() );
	done( QDialog::Accepted);
}

void UIAddIndividual::on_buttonCancel_clicked()
{
	done( QDialog::Rejected);
}


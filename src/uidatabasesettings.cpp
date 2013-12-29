#include "uidatabasesettings.h"
#include "uigetvalue.h"
#include "uiaddindividual.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
//
UIDatabaseSettings::UIDatabaseSettings( FaceDatabase *fdb,
                                        UIGetValue *gv,
                                        UIAddIndividual *ai,
                                        QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	uiGetValue = gv;
	uiAddIndividual = ai;
	faceDatabase = fdb;
	indexDatabase  = 0;
	individual = faceDatabase->prev();
}

void UIDatabaseSettings::updateSettings()
{
	updateDatabase();
	updateIndividual();
}

void UIDatabaseSettings:: updateDatabase()
{
	labelDatabaseSize->setText( QString::number( faceDatabase->count()));
	enableControls( faceDatabase->count() != 0);
}

void UIDatabaseSettings:: updateIndividual()
{
	pixmapIndividual->setPixmap( QPixmap::fromImage( QImage::fromData(
	  (const uchar*)individual.imageAddress(), individual.imageLength(), 0)));
	labelIndividualName->setText( QTextCodec::codecForLocale()->toUnicode( 
	                 individual.name().c_str(), individual.name().length()));
	labelIndividualId->setText( QString::number( individual.id()));
	textIndividualInfo->setText( QTextCodec::codecForLocale()->toUnicode( 
	individual.description().c_str(), individual.description().length())); 
}

void UIDatabaseSettings:: enableControls(bool enable)
{
	buttonIndividualPrev->setEnabled( enable);
	buttonIndividualNext->setEnabled( enable);
	buttonIndividualEdit->setEnabled( enable);
	buttonIndividualRemove->setEnabled( enable);
	buttonIndividualFindId->setEnabled( enable);
	buttonIndividualFindName->setEnabled( enable);
}

void UIDatabaseSettings::on_buttonIndividualAdd_clicked()
{
	uiAddIndividual->updateSettings( faceDatabase->count() + 1);
	if (uiAddIndividual->exec())
	{
		individual  = uiAddIndividual->individual();
		faceDatabase->addIndividual( individual);
		updateDatabase();
		updateIndividual();
	}
}

void UIDatabaseSettings::on_buttonDatabaseLoad_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setFileMode( QFileDialog::ExistingFile);
	fileDialog.setNameFilter( tr("Face Database (*.fdb)"));
	if (fileDialog.exec())
	{
		buttonDatabaseLoad->setEnabled( false);
		buttonDatabaseSave->setEnabled( false);
		labelDatabaseFile->setText( tr("Loading..."));		
		qApp->processEvents(QEventLoop::AllEvents, 100);		
		try
		{
			faceDatabase->load( fileDialog.selectedFiles()[0].toLocal8Bit() );
			labelDatabaseFile->setText( tr("Loaded from: ") +
			              fileDialog.selectedFiles()[0].remove(0, 
			              fileDialog.selectedFiles()[0].lastIndexOf("/") + 1) );
			labelDatabaseFile->setToolTip( fileDialog.selectedFiles()[0]);
			updateDatabase();			
			individual = faceDatabase->prev();
			updateIndividual();
		}
		catch( exception &e)
		{
			QMessageBox::warning(this, tr("Loading"), e.what());
			labelDatabaseFile->setText( tr("Loaded from: no file"));
		}		
		QMessageBox::information( this, tr("Loading"), 
		                      tr("faceDatabase loaded successfully."));
		buttonDatabaseLoad->setEnabled( true);
		buttonDatabaseSave->setEnabled( true); 
	}
}

void UIDatabaseSettings::on_buttonDatabaseSave_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	fileDialog.setFileMode( QFileDialog::AnyFile);
	fileDialog.setNameFilter( tr("Face Database (*.fdb)"));
	if (fileDialog.exec())
	{
		buttonDatabaseLoad->setEnabled( false);
		buttonDatabaseSave->setEnabled( false);		
		QString loadedFile = labelDatabaseFile->text(); 
		labelDatabaseFile->setText( tr("Saving..."));
		qApp->processEvents(QEventLoop::AllEvents, 100);
		faceDatabase->save( fileDialog.selectedFiles()[0].toLocal8Bit());
		QMessageBox::information( this, tr("Saving"), 
		                        tr("faceDatabase saved successfully."));
		labelDatabaseFile->setText( loadedFile);
		buttonDatabaseLoad->setEnabled( true);
		buttonDatabaseSave->setEnabled( true);
	}
}

void UIDatabaseSettings::on_buttonIndividualPrev_clicked()
{
	enableControls( false);
	individual = faceDatabase->prev();
	updateIndividual();
	enableControls( true);
}

void UIDatabaseSettings::on_buttonIndividualNext_clicked()
{
	enableControls( false);
	individual = faceDatabase->next();
	updateIndividual();
	enableControls( true);
}

void UIDatabaseSettings::on_buttonIndividualFindId_clicked()
{
	try
	{
		individual = faceDatabase->findId( uiGetValue->askForInt(
		                                   "Find id ...",
		                                   "Type id number you want to find:", 
		                                   0));
		updateIndividual();
	}
	catch ( exception &e) //assuming that id wasnt found
	{
		QMessageBox::information( this, "Find id ...", QString(e.what()));
	}
}

void UIDatabaseSettings::on_buttonIndividualFindName_clicked()
{
	try
	{
		individual = faceDatabase->findName( (std::string) 
		                                     uiGetValue->askForString(
		                                     "Find name ...",
		                                     "Type name you want to find:",
		                                     "").toLocal8Bit());
		updateIndividual();
	}
	catch ( exception &e) //assuming that name wasnt found
	{
		QMessageBox::information( this, "Find name ...", QString(e.what()));
	}
}

void UIDatabaseSettings::on_buttonIndividualEdit_clicked()
{
	enableControls( false);
	uiAddIndividual->setIndividual( individual);
	uiAddIndividual->updateSettings( individual.id());
	if (uiAddIndividual->exec())
	{
		individual  = uiAddIndividual->individual();
		faceDatabase->editCurrent( individual);		
		updateIndividual();
	}	
	enableControls( true);
}

void UIDatabaseSettings::on_buttonIndividualRemove_clicked()
{
	enableControls( false);
	faceDatabase->removeCurrent();
	updateDatabase();
	individual = faceDatabase->prev();
	updateIndividual();	 
	enableControls( true);
}


void UIDatabaseSettings::on_buttonClear_clicked()
{
	faceDatabase->clear();
	updateDatabase();
	individual = Individual();
	updateIndividual();
}


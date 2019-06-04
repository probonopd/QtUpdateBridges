#include <QApplication>
#include <SoftwareUpdateDialog.hpp>
#include <QCryptographicHash>
#include <QScreen>

static bool isSkipThisVersion(const QString &id,QSettings *settings){
	QCryptographicHash hasher(QCryptographicHash::Md5);
	hasher.addData(id.toUtf8());
	QString hash(hasher.result().toHex());
	return settings->value(hash).toBool();
}

SoftwareUpdateDialog::SoftwareUpdateDialog(QPixmap icon , QWidget *parent)
	: QDialog(parent)	
{
	m_Icon = icon;
	m_Settings = new QSettings(QSettings::UserScope ,
			           QString::fromUtf8("updatedeployqt") , 
				   QApplication::applicationName() , 
				   this);
	m_Ui.setupUi(this);
	
	connect(&m_Timer , &QTimer::timeout , this , &SoftwareUpdateDialog::pInit);
	connect(m_Ui.remindMeLater , &QPushButton::clicked , this , &SoftwareUpdateDialog::handleRemindMeLater);
	connect(m_Ui.skipVersionBtn , &QPushButton::clicked , this , &SoftwareUpdateDialog::handleSkipThisUpdate);
	connect(m_Ui.installUpdateBtn , &QPushButton::clicked , this , &SoftwareUpdateDialog::handleInstallUpdate);	
	return;
}

SoftwareUpdateDialog::~SoftwareUpdateDialog()
{
	m_Settings->deleteLater();
}

void SoftwareUpdateDialog::init(const QString &AppName , const QString &OldVersion ,
		                const QString &NewVersion , const QString &ReleaseNotes){

	m_Id.clear();
	m_Id.append(AppName);
        m_Id.append(OldVersion);
	m_Id.append(NewVersion);
	m_Id.append(ReleaseNotes);

	/* Check if the user choose to skip this version update. */
	if(isSkipThisVersion(m_Id , m_Settings)){
		setResult(QDialog::Rejected);
		emit rejected();
		return;
	}

	/* Check if we want to automatically download and install update. */
	if(m_Settings->value("autoUpdateOnFuture").toBool()){
		setResult(QDialog::Accepted);
		emit accepted();
		return;
	}

	/* Set the title , description , and release notes. */
	(m_Ui.titleLbl)->setText(m_TitleTemplate.arg(AppName));
	(m_Ui.descLbl)->setText(m_VersionDescTemplate.arg(AppName , NewVersion , OldVersion));
	(m_Ui.releaseNotesTxtBox)->setHtml(ReleaseNotes);


	/* Set Icons if available, else use updatedeployqt logo. */
	if(!m_Icon.isNull()){
			setWindowIcon(m_Icon);
			(m_Ui.softwareIcon)->setPixmap(m_Icon);
	}else{
		/* use updatedeploy logos. */
	}


	/* Center the dialog in the primary screen. */
	this->move(QGuiApplication::primaryScreen()->geometry().center() - this->rect().center());
	this->show();
}

/* private handles */

void SoftwareUpdateDialog::pInit(){
	this->move(QGuiApplication::primaryScreen()->geometry().center() - this->rect().center());
	this->show();
	m_Timer.stop();
}

void SoftwareUpdateDialog::handleRemindMeLater(){
	this->hide();
	m_Timer.setInterval(3600 * 1000); /* 1 hour interval */
	m_Timer.setSingleShot(true);
	m_Timer.start();
}

void SoftwareUpdateDialog::handleInstallUpdate(){
	this->hide();
	setResult(QDialog::Accepted);
	emit accepted();
}

void SoftwareUpdateDialog::handleSkipThisUpdate(){
	this->hide();
	
	QCryptographicHash hasher(QCryptographicHash::Md5);
	hasher.addData(m_Id.toUtf8());
	QString hash(hasher.result().toHex());
	m_Settings->setValue(hash , true);

	setResult(QDialog::Rejected);
	emit rejected();
}

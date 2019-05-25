#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QScreen>
#include <QDebug>
#include <AppImageUpdaterDialog>
#include <Bridge.hpp>

/*
 * This can be modified when compiled to machine code. */
const char *no_auto_update = "68b329da9893e34099c7d8ad5cb9c940"; /* default false */
const char *integrate_menu_to_qobject = "bfa40825ef36e05bbc2c561595829a92";

using AppImageUpdaterBridge::AppImageUpdaterDialog;

static bool integrate_menu(QWidget *widget , AppImageUpdaterDialog *dialog){
	auto menuBar = qobject_cast<QMenuBar*>(widget);
	if(!menuBar){
		return -1;
	}
	auto checkForUpdateAction = menuBar->addAction(QString::fromUtf8("Check for Update"));
	QObject::connect(dialog , &AppImageUpdaterDialog::init , 
			checkForUpdateAction , &QAction::trigger , Qt::QueuedConnection);
	return 0;
}

Bridge::Bridge(QObject *parent)
	: QObject(parent)
{
    auto instance = qobject_cast<QApplication*>(QApplication::instance());
    /* If the instance is not gui then we don't have to do anything. */
    if(!instance){
	    return;
    }
    QPixmap logo;
    logo = (instance->windowIcon()).pixmap(100 , 100);
    m_Dialog.reset(new AppImageUpdaterDialog(logo , nullptr,
		                                  AppImageUpdaterDialog::ShowProgressDialog|
						  AppImageUpdaterDialog::ShowUpdateConfirmationDialog|
						  AppImageUpdaterDialog::ShowFinishedDialog|
						  AppImageUpdaterDialog::ShowErrorDialog/*|
						  AppImageUpdaterDialog::AlertWhenAuthorizationIsRequired*/));
    connect(m_Dialog.data() , &AppImageUpdaterDialog::quit , instance , &QApplication::quit);
    m_Dialog->move(QGuiApplication::primaryScreen()->geometry().center() - m_Dialog->rect().center());
}

void Bridge::initAutoUpdate()
{
	if(m_Dialog.isNull()){
		return;
	}

	
	/*
	 * Lets assume '8cfaddf5b1a24d1fd31cab97b01f1f87' means true */
	if(!qstrcmp(no_auto_update , "8cfaddf5b1a24d1fd31cab97b01f1f87")){
		/* In this case , do not init right away , we 
		 * need to attach this to a menu element in the main widget 
		 * of the Qt Application in question. */

		qDebug() << "AppImageUpdaterBridge::INFO: no auto update enabled";

		bool integrated = false;

		qDebug() << "AppImageUpdaterBridge::INFO: searching for QObject(" << integrate_menu_to_qobject 
			 << ")";
		foreach (QWidget *widget, QApplication::allWidgets()){
			/* if the qobject name is given , then this qobject is 
			 * assumed to be the main QMenuBar of the application. */
			if(widget->objectName() == integrate_menu_to_qobject){
				/* integrate and exit */
				integrated = integrate_menu(widget , m_Dialog.data());
				break;
			}	

		}

		if(integrated){
			return;
		}

		qDebug() << "AppImageUpdaterBridge::INFO: cannot find the required QObject";
		qDebug() << "AppImageUpdaterBridge::INFO: trying to integrate into available QMenuBar";
		foreach (QWidget *widget , QApplication::allWidgets()){
			/* Try and check if its QMenuBar ,
			 * if so then simply integrate */
			if(!(integrated = integrate_menu(widget , m_Dialog.data()))){
				
				break;
			}
			QCoreApplication::processEvents();
		}

		if(integrated){
			return;
		}

		qDebug() << "AppImageUpdaterBridge::INFO: cannot find QMenuBar , trying to integrate into QMainWindow";
		foreach (QWidget *widget , QApplication::allWidgets()){
			auto mainWindow = qobject_cast<QMainWindow*>(widget);
			if(!mainWindow){
				QCoreApplication::processEvents();
				continue;
			}
			auto menuBar = new QMenuBar(mainWindow);
        		menuBar->setObjectName(QString::fromUtf8("menuBar"));
			menuBar->setGeometry(QRect(0, 0, mainWindow->size().width() , 25));
			integrated = integrate_menu(menuBar , m_Dialog.data());
			QCoreApplication::processEvents();
			break;
		}

		if(integrated){
			return;
		}

		/* If we are still not integrated then we will go berserk! */
		qDebug() << "AppImageUpdaterBridge::INFO: cannot integrate into QMainWindow , going berserk!";
		foreach (QWidget *widget , QApplication::allWidgets()){
			auto menuBar = new QMenuBar(widget);
        		menuBar->setObjectName(QString::fromUtf8("menuBar"));
			menuBar->setGeometry(QRect(0, 0, widget->size().width() , 25));
			integrated = integrate_menu(menuBar , m_Dialog.data());
			QCoreApplication::processEvents();
		}
		return; /* return control */	
	}
 
	m_Dialog->init();
	return;
}

#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QScreen>
#include <QDebug>
#include <AppImageUpdaterBridge>
#include <AppImageUpdaterDialog>
#include <SoftwareUpdateDialog.hpp>
#include <Bridge.hpp>


/* settings on how the bridge should operate. */
const char *interval = "75629552e6e8286442676be60e7da67d";
const char *qmenu_name = "871abbc22416bb25429594dec45caf1f";
const char *qmenubar_name = "bfa40825ef36e05bbc2c561595829a92";
const char *qpushbutton_name = "930b29debfb164461b39342d59e2565c";
const char *booleans = "4c6160c2d6bfeba1";

/*
 * Note: If the char at the given index is 1 and nothing else , then it means 
 * it is set.
 *
 *
 * booleans[0] - If set then auto update check is enabled
 * booleans[1] - If set then auto update check should occur on startup of the 
 *               application.
 * booleans[2] - If set then auto update check should occur on close of the
 *               application.
 * booleans[3] - If set then auto update check should be cyclic.
 * booleans[4] - If set then manual update check is enabled. (Both manual and auto can enabled).
 * booleans[5] - If set then QMenu QObject name is available.
 * booleans[6] - If set then QMenuBar QObject name is available.
 * booleans[7] - If set then QPushButton QObject name is available.
 * booleans[8] - If set then interval for startup auto update check and cyclic auto update check 
 *               is available.
*/

#define AUTO_UPDATE_CHECK 0
#define MANUAL_UPDATE_CHECK 4
#define QMENU_GIVEN 5
#define QMENUBAR_GIVEN 6
#define QPUSHBUTTON_GIVEN 7
#define INTERVAL_GIVEN 8

using AppImageUpdaterBridge::AppImageDeltaRevisioner;
using AppImageUpdaterBridge::AppImageUpdaterDialog;

#define IS_SET(s , i) ((s[i] == 1) ? true : false)

static bool integrate_menu(QWidget *widget , Bridge *obj){
	auto menu = qobject_cast<QMenu*>(widget);
	if(!menu){
		return false;
	}
	auto checkForUpdateAction = menu->addAction(QString::fromUtf8("Check for Update"));
	QObject::connect(checkForUpdateAction , &QAction::triggered ,
		         obj , &Bridge::checkForUpdate);
	return true;
}

static bool integrate_menubar(QWidget *widget , Bridge *obj){
	auto menubar = qobject_cast<QMenuBar*>(widget);
	if(!menubar){
		return false;
	}
	auto checkForUpdateAction = menubar->addAction(QString::fromUtf8("Check for Update"));
	QObject::connect(checkForUpdateAction , &QAction::triggered , 
			obj , &Bridge::checkForUpdate);
	return true;
}

static bool integrate_pushbutton(QWidget *widget , Bridge *obj){
	auto pushbutton = qobject_cast<QPushButton*>(widget);
	if(!pushbutton){
		return false;
	}
	QObject::connect(pushbutton , &QPushButton::clicked , obj , &Bridge::checkForUpdate);
	return true;
}


Bridge::Bridge(QObject *parent)
	: QObject(parent)
{
    auto instance = qobject_cast<QApplication*>(QApplication::instance());
    /* If the instance is not gui then we don't have to do anything. */
    if(!instance){
	    return;
    }

    if(IS_SET(booleans , AUTO_UPDATE_CHECK)){
	    m_Updater.reset(new AppImageDeltaRevisioner);
	    connect(m_Updater.data() , &AppImageDeltaRevisioner::updateAvailable , this , &Bridge::handleUpdate);
	    return;
    }
    connect(&m_Timer , &QTimer::timeout , this , &Bridge::tryIntegrate); /* to retry until integration was successful.*/
}

void Bridge::checkForUpdate(){
	if(!m_UpdateDialog.isNull()){
		m_UpdateDialog->move(QGuiApplication::primaryScreen()->geometry().center() - m_UpdateDialog->rect().center());
		m_UpdateDialog->init();
		return;
	}
	
	QPixmap icon = QApplication::windowIcon().pixmap(100 , 100);
	if(icon.isNull()){
		icon = QPixmap(":/logo.png");
	}

	m_UpdateDialog.reset(new AppImageUpdaterDialog(icon));
	m_UpdateDialog->move(QGuiApplication::primaryScreen()->geometry().center() - m_UpdateDialog->rect().center());
	m_UpdateDialog->init();
}

void Bridge::handleUpdate(bool isAval , QJsonObject info){
	if(!isAval){
		return;
	}



}

void Bridge::tryIntegrate(){
	m_Timer.stop();
	bool integrated = false;
	if(IS_SET(booleans , QMENU_GIVEN)){
		qDebug() << "AppImageUpdaterBridge::INFO: QMenu object name given.";
		foreach (QWidget *widget, QApplication::allWidgets()){
			if(widget->objectName() == qmenu_name){
				integrated = integrate_menu(widget , this);
				if(integrated){
					/* forget about this */
					((char*)booleans)[QMENU_GIVEN] = 0;
				}
				break;
			}
		}
	}

	if(IS_SET(booleans , QMENUBAR_GIVEN)){
		qDebug() << "AppImageUpdaterBridge::INFO: QMenuBar object name given.";
		foreach (QWidget *widget, QApplication::allWidgets()){
			if(widget->objectName() == qmenu_name){
				integrated = integrate_menubar(widget , this);
				if(integrated){
					((char*)booleans)[QMENUBAR_GIVEN] = 0;
				}
				break;
			}
		}
	}

	if(IS_SET(booleans , QPUSHBUTTON_GIVEN)){
		qDebug() << "AppImageUpdaterBridge::INFO: QPushButton object name given.";
		foreach (QWidget *widget, QApplication::allWidgets()){
			if(widget->objectName() == qmenu_name){
				integrated = integrate_pushbutton(widget , this);
				if(integrated){
					((char*)booleans)[QPUSHBUTTON_GIVEN] = 0;
				}
				break;
			}
		}
	}

	if(!integrated){
		m_Timer.setInterval(5000);
		m_Timer.setSingleShot(true);
		m_Timer.start();
	}
	return;
}

void Bridge::initAutoUpdate()
{
	if(IS_SET(booleans , MANUAL_UPDATE_CHECK)){	
		tryIntegrate();
		return;
	}
	return;
}

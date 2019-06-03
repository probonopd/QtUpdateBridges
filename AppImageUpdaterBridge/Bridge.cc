#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QScreen>
#include <QDebug>
#include <AppImageUpdaterBridge>
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

#define IS_SET(s , i) ((s[i] == 1) ? true : false)

static bool integrate_menu(QWidget *widget , AppImageUpdaterDialog *dialog){
	auto menuBar = qobject_cast<QMenuBar*>(widget);
	if(!menuBar){
		return false;
	}
	auto checkForUpdateAction = menuBar->addAction(QString::fromUtf8("Check for Update"));
	QObject::connect(checkForUpdateAction , &QAction::triggered , 
			dialog , &AppImageUpdaterDialog::init , 
			Qt::QueuedConnection);
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
    m_Updater.reset(new AppImageDeltaRevisioner);
}

void Bridge::initAutoUpdate()
{
	if(m_Updater.isNull()){
		return;
	}

	if(IS_SET(booleans , MANUAL_UPDATE_CHECK)){	
		qDebug() << "AppImageUpdaterBridge::INFO: manual update enabled";
		bool integrated = false;

		if(IS_SET(booleans , QMENU_GIVEN)){
			qDebug() << "AppImageUpdaterBridge::INFO: QMenu object name given.";
			foreach (QWidget *widget, QApplication::allWidgets()){
				if(widget->objectName() == qmenu_name){
					integrated = integrate_menu(widget , m_Dialog.data());
					break;
				}
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
			if((integrated = integrate_menu(widget , m_Dialog.data()))){	
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

			/* First lets check if the main window has any QMenuBar. */
			auto children = mainWindow->children();
			foreach(QObject *bar , children){
				if((integrated = integrate_menu(widget , m_Dialog.data()))){	
					break;
				}
				QCoreApplication::processEvents();
			}

			if(integrated){
				break;
			}

			/* Its confirmed that we don't have any QMenuBar. */
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
		qDebug() << "AppImageUpdaterBridge::INFO: find anyway to integrate QMenuBar , giving up.";
		return; /* return control */	
	}
 
	m_Dialog->init();
	return;
}

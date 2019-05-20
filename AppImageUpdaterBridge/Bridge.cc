#include <QApplication>
#include <AppImageUpdaterDialog>
#include <Bridge.hpp>

using AppImageUpdaterBridge::AppImageUpdaterDialog;

Bridge::Bridge(QObject *parent)
	: QObject(parent)
{
    auto instance = (QApplication*)QApplication::instance();
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
}

void Bridge::initAutoUpdate()
{
	if(m_Dialog.isNull()){
		return;
	}
	m_Dialog->init();
	return;
}

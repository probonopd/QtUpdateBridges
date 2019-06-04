#ifndef BRIDGE_HPP_INCLUDED
#define BRIDGE_HPP_INCLUDED
#include <QScopedPointer>
#include <QJsonObject>
#include <QTimer>
#include <BridgeInterface.hpp>
#include <SoftwareUpdateDialog.hpp>
#include <AppImageUpdaterBridge>
#include <AppImageUpdaterDialog>

class Bridge : public QObject , BridgeInterface {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID BridgeInterface_iid FILE "Bridge.json")
	Q_INTERFACES(BridgeInterface)
public:
	Bridge(QObject *parent = nullptr);

public Q_SLOTS:
	void checkForUpdate();
private Q_SLOTS:
	void initAutoUpdate();
	void tryIntegrate();
	void handleUpdate(bool , QJsonObject);
private:
	bool b_IntegratedQMenu = false,
	     b_IntegratedQMenuBar = false,
	     b_IntegratedQPushButton = false;
	QTimer m_Timer;
	QScopedPointer<AppImageUpdaterBridge::AppImageUpdaterDialog> m_UpdateDialog;
	QScopedPointer<SoftwareUpdateDialog> m_SoftwareUpdateDialog;
	QScopedPointer<AppImageUpdaterBridge::AppImageDeltaRevisioner> m_Updater;
};

#endif // BRIDGE_HPP_INCLUDED 

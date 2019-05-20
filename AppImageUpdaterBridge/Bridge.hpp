#ifndef BRIDGE_HPP_INCLUDED
#define BRIDGE_HPP_INCLUDED
#include <QScopedPointer>
#include <BridgeInterface.hpp>
#include <AppImageUpdaterDialog>

class Bridge : public QObject , BridgeInterface {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID BridgeInterface_iid FILE "Bridge.json")
	Q_INTERFACES(BridgeInterface)
public:
	Bridge(QObject *parent = nullptr);
private Q_SLOTS:
	void initAutoUpdate();
private:
	QScopedPointer<AppImageUpdaterBridge::AppImageUpdaterDialog> m_Dialog;
};

#endif // BRIDGE_HPP_INCLUDED 

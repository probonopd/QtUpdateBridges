#ifndef BRIDGE_INTERFACE_HPP_INCLUDED 
#define BRIDGE_INTERFACE_HPP_INCLUDED 
#include <QtPlugin>

class BridgeInterface {
public:
	virtual ~BridgeInterface() {}
public Q_SLOTS:
	virtual void initAutoUpdate() = 0;
};

#ifndef BridgeInterface_iid
#define BridgeInterface_iid "com.antony-jr.AppImageUpdaterBridge"
#endif 

Q_DECLARE_INTERFACE(BridgeInterface , BridgeInterface_iid);

#endif // BRIDGE_INTERFACE_HPP_INCLUDED

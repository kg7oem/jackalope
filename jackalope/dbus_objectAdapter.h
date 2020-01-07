#ifndef DBUS_OBJECTADAPTER_H
#define DBUS_OBJECTADAPTER_H

#include <dbus-cxx.h>
#include <memory>
#include <stdint.h>
#include <string>
#include "dbus_objectAdaptee.h"
class dbus_objectAdapter
 : public DBus::Object {
protected:
dbus_objectAdapter(dbus_objectAdaptee* adaptee, std::string path = "/object" );
public:
    static std::shared_ptr<dbus_objectAdapter> create(dbus_objectAdaptee* adaptee, std::string path = "/object" );
};
#endif /* DBUS_OBJECTADAPTER_H */

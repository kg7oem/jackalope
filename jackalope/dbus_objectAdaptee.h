#ifndef DBUS_OBJECTADAPTEE_H
#define DBUS_OBJECTADAPTEE_H

#include <string>
class dbus_objectAdaptee
{
public:
virtual std::string peek(std::string name ) = 0;
virtual void poke(std::string name, std::string value ) = 0;
};
#endif /* DBUS_OBJECTADAPTEE_H */

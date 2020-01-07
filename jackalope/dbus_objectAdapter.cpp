#include "dbus_objectAdapter.h"

dbus_objectAdapter::dbus_objectAdapter(dbus_objectAdaptee* adaptee, std::string path ) : DBus::Object( path ){
DBus::MethodBase::pointer temp_method;
temp_method = this->create_method<std::string,std::string>( "object", "peek",sigc::mem_fun( adaptee, &dbus_objectAdaptee::peek ) );
temp_method->set_arg_name( 0, "name" );
temp_method = this->create_method<void,std::string,std::string>( "object", "poke",sigc::mem_fun( adaptee, &dbus_objectAdaptee::poke ) );
temp_method->set_arg_name( 0, "name" );
temp_method->set_arg_name( 1, "value" );

}
std::shared_ptr<dbus_objectAdapter> dbus_objectAdapter::create(dbus_objectAdaptee* adaptee, std::string path ){
return std::shared_ptr<dbus_objectAdapter>( new dbus_objectAdapter( adaptee, path ) );

}

/*----- PROTECTED REGION ID(CounterChannel.cpp) ENABLED START -----*/
//=============================================================================
//
// file :        CounterChannel.cpp
//
// description : C++ source for the CounterChannel class and its commands.
//               The class is derived from Device. It represents the
//               CORBA servant object which will be accessed from the
//               network. All commands which can be executed on the
//               CounterChannel are implemented in this file.
//
// project :
//
// This file is part of Tango device class.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
//
//
//=============================================================================
//                This file is generated by POGO
//        (Program Obviously used to Generate tango Object)
//=============================================================================


#include <CounterChannel.h>
#include <CounterChannelClass.h>

#include "QMesydaqDetectorInterface.h"

/*----- PROTECTED REGION END -----*/	//	CounterChannel.cpp

/**
 *  CounterChannel class description:
 *    Base class for channels that control monitor or single detector counts.
 */

//================================================================
//  The following table gives the correspondence
//  between command and method names.
//
//  Command name   |  Method name
//================================================================
//  State          |  Inherited (no method)
//  Status         |  Inherited (no method)
//  On             |  Inherited (no method)
//  Off            |  Inherited (no method)
//  GetProperties  |  Inherited (no method)
//  SetProperties  |  Inherited (no method)
//  Reset          |  Inherited (no method)
//  Start          |  Inherited (no method)
//  Stop           |  Inherited (no method)
//  Clear          |  Inherited (no method)
//  Resume         |  Inherited (no method)
//  Prepare        |  Inherited (no method)
//================================================================

//================================================================
//  Attributes managed are:
//================================================================
//  version       |  Tango::DevString	Scalar
//  active        |  Tango::DevBoolean	Scalar
//  value         |  Tango::DevULong64	Scalar
//  preselection  |  Tango::DevULong64	Scalar
//================================================================

namespace CounterChannel_ns
{
/*----- PROTECTED REGION ID(CounterChannel::namespace_starting) ENABLED START -----*/

//	static initializations

/*----- PROTECTED REGION END -----*/	//	CounterChannel::namespace_starting

//--------------------------------------------------------
/**
 *	Method      : CounterChannel::CounterChannel()
 *	Description : Constructors for a Tango device
 *                implementing the classCounterChannel
 */
//--------------------------------------------------------
CounterChannel::CounterChannel(Tango::DeviceClass *cl, string &s)
 : DetectorChannel(cl, s.c_str())
{
	/*----- PROTECTED REGION ID(CounterChannel::constructor_1) ENABLED START -----*/
	init_device();

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::constructor_1
}
//--------------------------------------------------------
CounterChannel::CounterChannel(Tango::DeviceClass *cl, const char *s)
 : DetectorChannel(cl, s)
{
	/*----- PROTECTED REGION ID(CounterChannel::constructor_2) ENABLED START -----*/
	init_device();

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::constructor_2
}
//--------------------------------------------------------
CounterChannel::CounterChannel(Tango::DeviceClass *cl, const char *s, const char *d)
 : DetectorChannel(cl, s, d)
{
	/*----- PROTECTED REGION ID(CounterChannel::constructor_3) ENABLED START -----*/
	init_device();

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::constructor_3
}

//--------------------------------------------------------
/**
 *	Method      : CounterChannel::delete_device()
 *	Description : will be called at device destruction or at init command
 */
//--------------------------------------------------------
void CounterChannel::delete_device()
{
	DEBUG_STREAM << "CounterChannel::delete_device() " << device_name << endl;
	/*----- PROTECTED REGION ID(CounterChannel::delete_device) ENABLED START -----*/

	//	Delete device allocated objects

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::delete_device
	delete[] attr_value_read;
	delete[] attr_preselection_read;

	if (Tango::Util::instance()->is_svr_shutting_down()==false  &&
		Tango::Util::instance()->is_device_restarting(device_name)==false &&
		Tango::Util::instance()->is_svr_starting()==false)
	{
		//	If not shutting down call delete device for inherited object
		DetectorChannel_ns::DetectorChannel::delete_device();
	}
}

//--------------------------------------------------------
/**
 *	Method      : CounterChannel::init_device()
 *	Description : will be called at device initialization.
 */
//--------------------------------------------------------
void CounterChannel::init_device()
{
	DEBUG_STREAM << "CounterChannel::init_device() create device " << device_name << endl;
	/*----- PROTECTED REGION ID(CounterChannel::init_device_before) ENABLED START -----*/

	//	Initialization before get_device_property() call

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::init_device_before

	if (Tango::Util::instance()->is_svr_starting() == false  &&
		Tango::Util::instance()->is_device_restarting(device_name)==false)
	{
		//	If not starting up call init device for inherited object
		DetectorChannel_ns::DetectorChannel::init_device();
	}

	//	Get the device properties from database
	get_device_property();

	attr_value_read = new Tango::DevULong64[1];
	attr_preselection_read = new Tango::DevULong64[1];
	/*----- PROTECTED REGION ID(CounterChannel::init_device) ENABLED START -----*/
	//	Initialize device

	attr_value_read[0] = 0;
	attr_preselection_read[0] = 0;

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::init_device
}

//--------------------------------------------------------
/**
 *	Method      : CounterChannel::get_device_property()
 *	Description : Read database to initialize property data members.
 */
//--------------------------------------------------------
void CounterChannel::get_device_property()
{
	/*----- PROTECTED REGION ID(CounterChannel::get_device_property_before) ENABLED START -----*/

	//	Initialize property data members
	channel = 0;

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::get_device_property_before


	//	Read device properties from database.
	Tango::DbData	dev_prop;
	dev_prop.push_back(Tango::DbDatum("channel"));

	//	is there at least one property to be read ?
	if (dev_prop.size()>0)
	{
		//	Call database and extract values
		if (Tango::Util::instance()->_UseDb)
			get_db_device()->get_property(dev_prop);

		//	get instance on CounterChannelClass to get class property
		Tango::DbDatum	def_prop, cl_prop;
		CounterChannelClass	*ds_class =
			(static_cast<CounterChannelClass *>(get_device_class()));
		int	i = -1;

		//	Try to initialize channel from class property
		cl_prop = ds_class->get_class_property(dev_prop[++i].name);
		if (cl_prop.is_empty()==false) cl_prop  >>  channel;
		else {
			//	Try to initialize channel from default device value
			def_prop = ds_class->get_default_device_property(dev_prop[i].name);
			if (def_prop.is_empty()==false) def_prop  >>  channel;
		}
		//	And try to extract channel value from database
		if (dev_prop[i].is_empty()==false) dev_prop[i]  >>  channel;

	}

	/*----- PROTECTED REGION ID(CounterChannel::get_device_property_after) ENABLED START -----*/
	//	Check device property data members init
	if (!check_channel_value(channel))
	{
		channel = 0;
#if 0
		// Don't throw an exception here which leads to an endless recursion
		::Tango::ApiDataExcept::throw_exception("Value error",
							"channel value not in [0..5, 100]",
							"CounterChannel::get_device_property()");
#endif
	}

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::get_device_property_after
}

//--------------------------------------------------------
/**
 *	Method      : CounterChannel::always_executed_hook()
 *	Description : method always executed before any command is executed
 */
//--------------------------------------------------------
void CounterChannel::always_executed_hook()
{
	DEBUG_STREAM << "CounterChannel::always_executed_hook()  " << device_name << endl;
	/*----- PROTECTED REGION ID(CounterChannel::always_executed_hook) ENABLED START -----*/

	//	code always executed before all requests
	DetectorChannel::always_executed_hook();

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::always_executed_hook
}

//--------------------------------------------------------
/**
 *	Method      : CounterChannel::read_attr_hardware()
 *	Description : Hardware acquisition for attributes
 */
//--------------------------------------------------------
void CounterChannel::read_attr_hardware(TANGO_UNUSED(vector<long> &attr_list))
{
	DEBUG_STREAM << "CounterChannel::read_attr_hardware(vector<long> &attr_list) entering... " << endl;
	/*----- PROTECTED REGION ID(CounterChannel::read_attr_hardware) ENABLED START -----*/

	//	Add your own code
	DetectorChannel::read_attr_hardware(attr_list);

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::read_attr_hardware
}
//--------------------------------------------------------
/**
 *	Method      : CounterChannel::write_attr_hardware()
 *	Description : Hardware writing for attributes
 */
//--------------------------------------------------------
void CounterChannel::write_attr_hardware(TANGO_UNUSED(vector<long> &attr_list))
{
	DEBUG_STREAM << "CounterChannel::write_attr_hardware(vector<long> &attr_list) entering... " << endl;
	/*----- PROTECTED REGION ID(CounterChannel::write_attr_hardware) ENABLED START -----*/

	//	Add your own code
	DetectorChannel::write_attr_hardware(attr_list);

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::write_attr_hardware
}

//--------------------------------------------------------
/**
 *	Read attribute value related method
 *	Description: Current counter value.
 *
 *	Data type:	Tango::DevULong64
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void CounterChannel::read_value(Tango::Attribute &attr)
{
	DEBUG_STREAM << "CounterChannel::read_value(Tango::Attribute &attr) entering... " << endl;
	/*----- PROTECTED REGION ID(CounterChannel::read_value) ENABLED START -----*/
	//	Set the attribute value
	if (!m_interface)
		::Tango::Except::throw_exception("Runtime error",
						 "Control interface not initialized",
						 "CounterChannel::read_value()");
	attr_value_read[0] = m_interface->readCounter(channel);
	attr.set_value(attr_value_read);

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::read_value
}
//--------------------------------------------------------
/**
 *	Read attribute preselection related method
 *	Description: Current preset value.
 *
 *	Data type:	Tango::DevULong64
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void CounterChannel::read_preselection(Tango::Attribute &attr)
{
	DEBUG_STREAM << "CounterChannel::read_preselection(Tango::Attribute &attr) entering... " << endl;
	/*----- PROTECTED REGION ID(CounterChannel::read_preselection) ENABLED START -----*/
	//	Set the attribute value
	if (!m_interface)
		::Tango::Except::throw_exception("Runtime error",
						 "Control interface not initialized",
						 "CounterChannel::read_preselection()");
	attr_preselection_read[0] = m_interface->preSelection(channel);
	attr.set_value(attr_preselection_read);

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::read_preselection
}
//--------------------------------------------------------
/**
 *	Write attribute preselection related method
 *	Description: Current preset value.
 *
 *	Data type:	Tango::DevULong64
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void CounterChannel::write_preselection(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "CounterChannel::write_preselection(Tango::WAttribute &attr) entering... " << endl;
	//	Retrieve write value
	Tango::DevULong64	w_val;
	attr.get_write_value(w_val);
	/*----- PROTECTED REGION ID(CounterChannel::write_preselection) ENABLED START -----*/
	if (!m_interface)
		::Tango::Except::throw_exception("Runtime error",
						 "Control interface not initialized",
						 "CounterChannel::write_preselection()");
	m_interface->setPreSelection(channel, w_val);

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::write_preselection
}

//--------------------------------------------------------
/**
 *	Method      : CounterChannel::add_dynamic_attributes()
 *	Description : Create the dynamic attributes if any
 *                for specified device.
 */
//--------------------------------------------------------
void CounterChannel::add_dynamic_attributes()
{
	/*----- PROTECTED REGION ID(CounterChannel::add_dynamic_attributes) ENABLED START -----*/

	//	Add your own code to create and add dynamic attributes if any
	DetectorChannel::add_dynamic_attributes();

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::add_dynamic_attributes
}

//--------------------------------------------------------
/**
 *	Method      : CounterChannel::add_dynamic_commands()
 *	Description : Create the dynamic commands if any
 *                for specified device.
 */
//--------------------------------------------------------
void CounterChannel::add_dynamic_commands()
{
	/*----- PROTECTED REGION ID(CounterChannel::add_dynamic_commands) ENABLED START -----*/

	//	Add your own code to create and add dynamic commands if any
	DetectorChannel::add_dynamic_commands();

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::add_dynamic_commands
}

/*----- PROTECTED REGION ID(CounterChannel::namespace_ending) ENABLED START -----*/

//	Additional Methods
//--------------------------------------------------------
/**
 *	Read attribute active related method
 *	Description: If this channel can finish the measurement when preselection is reached.
 *
 *	Data type:	Tango::DevBoolean
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void CounterChannel::read_active(Tango::Attribute &attr)
{
	DEBUG_STREAM << "DetectorChannel::read_active(Tango::Attribute &attr) entering... " << endl;
	//	Set the attribute value
	if (!m_interface)
		::Tango::Except::throw_exception("Runtime error",
						 "Control interface not initialized",
						 "CounterChannel::read_active()");
	attr_active_read[0] = isMaster();
	attr.set_value(attr_active_read);

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::read_active
}
//--------------------------------------------------------
/**
 *	Write attribute active related method
 *	Description: If this channel can finish the measurement when preselection is reached.
 *
 *	Data type:	Tango::DevBoolean
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void CounterChannel::write_active(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "DetectorChannel::write_active(Tango::WAttribute &attr) entering... " << endl;
	//	Retrieve write value
	Tango::DevBoolean	w_val;
	attr.get_write_value(w_val);
	/*----- PROTECTED REGION ID(DetectorChannel::write_active) ENABLED START -----*/
	if (!m_interface)
		::Tango::Except::throw_exception("Runtime error",
						 "Control interface not initialized",
						 "CounterChannel::write_active()");
	m_interface->selectCounter(channel, w_val, m_interface->preSelection(channel));

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::write_active
}
//--------------------------------------------------------
/**
 *	Command GetProperties related method
 *	Description: Returns a string list of properties and their values, in the form [prop1, val1, prop2, val2, ...].
 *
 *	@returns
 */
//--------------------------------------------------------
Tango::DevVarStringArray *CounterChannel::get_properties()
{
	Tango::DevVarStringArray *argout;
	DEBUG_STREAM << "CounterChannel::GetProperties()  - " << device_name << endl;
	/*----- PROTECTED REGION ID(CounterChannel::get_properties) ENABLED START -----*/

	argout = DetectorChannel::get_properties();
	int i = argout->length();
	argout->length(i + 2);

	(*argout)[i] = CORBA::string_dup("channel");
	char channel_str[64];
	snprintf(channel_str, sizeof(channel_str) - 1, "%d", channel);
	(*argout)[i + 1] = CORBA::string_dup(channel_str);

	/*----- PROTECTED REGION END -----*/	//	CounterChannel::get_properties
	return argout;
}

Tango::DevBoolean CounterChannel::update_properties(const Tango::DevVarStringArray *argin)
{
	for (unsigned int i = 0; i < argin->length(); i += 2)
	{
		std::string propertyName((*argin)[i]);
		std::string arg((*argin)[i + 1]);
		Tango::DbDatum data(propertyName);
		data << arg;
	/*----- PROTECTED REGION ID(ImageChannel::update_properties) ENABLED START -----*/
		if (propertyName == "channel")
		{
			Tango::DevUShort val;
			data >> val;
			if (!check_channel_value(val))
				::Tango::ApiDataExcept::throw_exception("Value error",
							"channel value not in [0..5, 100]",
							"CounterChannel::update_properties()");
			set_channel(val);
		}
	/*----- PROTECTED REGION END -----*/	//	CounterChannel::update_properties
	}
	return DetectorChannel::update_properties(argin);
}

bool CounterChannel::isMaster(void)
{
	return bool(m_interface->counterSelected(channel));
}


bool CounterChannel::check_channel_value(const Tango::DevUShort val)
{
	return val <=5 || val == 100;
}

void CounterChannel::set_channel(const Tango::DevUShort val)
{
	channel = val;
}

/*----- PROTECTED REGION END -----*/	//	CounterChannel::namespace_ending
} //	namespace

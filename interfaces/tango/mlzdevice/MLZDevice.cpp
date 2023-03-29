/*----- PROTECTED REGION ID(MLZDevice.cpp) ENABLED START -----*/
static const char *RcsId = "$Id:  $";
//=============================================================================
//
// file :	MLZDevice.cpp
//
// description : C++ source for the MLZDevice class and its commands.
//	       The class is derived from Device. It represents the
//	       CORBA servant object which will be accessed from the
//	       network. All commands which can be executed on the
//	       MLZDevice are implemented in this file.
//
// project :     MLZ Base Device
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
// $Author:  $
//
// $Revision:  $
// $Date:  $
//
// $HeadURL:  $
//
//=============================================================================
//		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//=============================================================================


#include "MLZDevice.h"
#include "MLZDeviceClass.h"

/*----- PROTECTED REGION END -----*/	//	MLZDevice.cpp

/**
 *  MLZDevice class description:
 *    This is the root of the abstract base class inheritance tree.
 */

//================================================================
//  The following table gives the correspondence
//  between command and method names.
//
//  Command name   |  Method name
//================================================================
//  State          |  Inherited (no method)
//  Status         |  Inherited (no method)
//  On             |  on
//  Off            |  off
//  GetProperties  |  get_properties
//  SetProperties  |  set_properties
//  Reset          |  reset
//================================================================

//================================================================
//  Attributes managed is:
//================================================================
//  version  |  Tango::DevString	Scalar
//================================================================

namespace MLZDevice_ns
{
/*----- PROTECTED REGION ID(MLZDevice::namespace_starting) ENABLED START -----*/

//	static initializations

/*----- PROTECTED REGION END -----*/	//	MLZDevice::namespace_starting

//--------------------------------------------------------
/**
 *	Method      : MLZDevice::MLZDevice()
 *	Description : Constructors for a Tango device
 *                implementing the classMLZDevice
 */
//--------------------------------------------------------
MLZDevice::MLZDevice(Tango::DeviceClass *cl, std::string &s)
 : TANGO_BASE_CLASS(cl, s.c_str())
{
	/*----- PROTECTED REGION ID(MLZDevice::constructor_1) ENABLED START -----*/
	init_device();

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::constructor_1
}
//--------------------------------------------------------
MLZDevice::MLZDevice(Tango::DeviceClass *cl, const char *s)
 : TANGO_BASE_CLASS(cl, s)
{
	/*----- PROTECTED REGION ID(MLZDevice::constructor_2) ENABLED START -----*/
	init_device();

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::constructor_2
}
//--------------------------------------------------------
MLZDevice::MLZDevice(Tango::DeviceClass *cl, const char *s, const char *d)
 : TANGO_BASE_CLASS(cl, s, d)
{
	/*----- PROTECTED REGION ID(MLZDevice::constructor_3) ENABLED START -----*/
	init_device();

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::constructor_3
}

//--------------------------------------------------------
/**
 *	Method      : MLZDevice::delete_device()
 *	Description : will be called at device destruction or at init command
 */
//--------------------------------------------------------
void MLZDevice::delete_device()
{
	DEBUG_STREAM << "MLZDevice::delete_device() " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(MLZDevice::delete_device) ENABLED START -----*/

	//	Delete device allocated objects

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::delete_device
	delete[] attr_version_read;
}

//--------------------------------------------------------
/**
 *	Method      : MLZDevice::init_device()
 *	Description : will be called at device initialization.
 */
//--------------------------------------------------------
void MLZDevice::init_device()
{
	DEBUG_STREAM << "MLZDevice::init_device() create device " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(MLZDevice::init_device_before) ENABLED START -----*/

	//	Initialization before get_device_property() call

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::init_device_before

	//	No device property to be read from database

	attr_version_read = new Tango::DevString[1];
	/*----- PROTECTED REGION ID(MLZDevice::init_device) ENABLED START -----*/

	//	Initialize device
	*attr_version_read = Tango::string_dup(VERSION);

	set_state(Tango::ON);

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::init_device
}


//--------------------------------------------------------
/**
 *	Method      : MLZDevice::always_executed_hook()
 *	Description : method always executed before any command is executed
 */
//--------------------------------------------------------
void MLZDevice::always_executed_hook()
{
	DEBUG_STREAM << "MLZDevice::always_executed_hook()  " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(MLZDevice::always_executed_hook) ENABLED START -----*/

	//	code always executed before all requests

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::always_executed_hook
}

//--------------------------------------------------------
/**
 *	Method      : MLZDevice::read_attr_hardware()
 *	Description : Hardware acquisition for attributes
 */
//--------------------------------------------------------
void MLZDevice::read_attr_hardware(TANGO_UNUSED(std::vector<long> &attr_list))
{
	DEBUG_STREAM << "MLZDevice::read_attr_hardware(std::vector<long> &attr_list) entering... " << ENDLOG;
	/*----- PROTECTED REGION ID(MLZDevice::read_attr_hardware) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::read_attr_hardware
}

//--------------------------------------------------------
/**
 *	Read attribute version related method
 *	Description: This attribute contains the version of the device class and its parent classes (recursively). The format is "module1 version1, module2 version2, ...".
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void MLZDevice::read_version(Tango::Attribute &attr)
{
	DEBUG_STREAM << "MLZDevice::read_version(Tango::Attribute &attr) entering... " << ENDLOG;
	/*----- PROTECTED REGION ID(MLZDevice::read_version) ENABLED START -----*/
	//	Set the attribute value
	attr.set_value(attr_version_read);

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::read_version
}

//--------------------------------------------------------
/**
 *	Method      : MLZDevice::add_dynamic_attributes()
 *	Description : Create the dynamic attributes if any
 *                for specified device.
 */
//--------------------------------------------------------
void MLZDevice::add_dynamic_attributes()
{
	/*----- PROTECTED REGION ID(MLZDevice::add_dynamic_attributes) ENABLED START -----*/

	//	Add your own code to create and add dynamic attributes if any

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::add_dynamic_attributes
}

//--------------------------------------------------------
/**
 *	Command On related method
 *	Description: Switches the main function of the device on.
 *
 */
//--------------------------------------------------------
void MLZDevice::on()
{
	DEBUG_STREAM << "MLZDevice::On()  - " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(MLZDevice::on) ENABLED START -----*/
	//	Add your own code

	set_state(Tango::ON);

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::on
}
//--------------------------------------------------------
/**
 *	Command Off related method
 *	Description: Switches the main function of the device off.
 *
 */
//--------------------------------------------------------
void MLZDevice::off()
{
	DEBUG_STREAM << "MLZDevice::Off()  - " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(MLZDevice::off) ENABLED START -----*/
	//	Add your own code

	set_state(Tango::OFF);

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::off
}
//--------------------------------------------------------
/**
 *	Command GetProperties related method
 *	Description: Returns a string list of properties and their values, in the form [prop1, val1, prop2, val2, ...].
 *
 *	@returns
 */
//--------------------------------------------------------
Tango::DevVarStringArray *MLZDevice::get_properties()
{
	Tango::DevVarStringArray *argout;
	DEBUG_STREAM << "MLZDevice::GetProperties()  - " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(MLZDevice::get_properties) ENABLED START -----*/

	ERROR_STREAM << "MLZDevice::GetProperties()  - " << device_name << ENDLOG;
	argout = new Tango::DevVarStringArray;
	argout->length(0);

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::get_properties
	return argout;
}
//--------------------------------------------------------
/**
 *	Command SetProperties related method
 *	Description: Set properties on the device. The argument must have the same form as the return value of "GetProperties", but not all properties have to be present.
 *
 *	@param argin
 *	@returns True if the properties were saved persistently, False if they were only set for the current session.
 */
//--------------------------------------------------------
Tango::DevBoolean MLZDevice::set_properties(const Tango::DevVarStringArray *argin)
{
	Tango::DevBoolean argout;
	DEBUG_STREAM << "MLZDevice::SetProperties()  - " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(MLZDevice::set_properties) ENABLED START -----*/

	if (argin->length() % 2)
		::Tango::ApiDataExcept::throw_exception("Value error",
							"number of parameter strings is not multiple of 2",
							"CounterChannel::set_properties()");

	argout = update_properties(argin);

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::set_properties
	return argout;
}
//--------------------------------------------------------
/**
 *	Command Reset related method
 *	Description: Resets the device to overcome a FAULT state.
 *
 */
//--------------------------------------------------------
void MLZDevice::reset()
{
	DEBUG_STREAM << "MLZDevice::Reset()  - " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(MLZDevice::reset) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::reset
}
//--------------------------------------------------------
/**
 *	Method      : MLZDevice::add_dynamic_commands()
 *	Description : Create the dynamic commands if any
 *                for specified device.
 */
//--------------------------------------------------------
void MLZDevice::add_dynamic_commands()
{
	/*----- PROTECTED REGION ID(MLZDevice::add_dynamic_commands) ENABLED START -----*/

	//	Add your own code to create and add dynamic commands if any

	/*----- PROTECTED REGION END -----*/	//	MLZDevice::add_dynamic_commands
}

/*----- PROTECTED REGION ID(MLZDevice::namespace_ending) ENABLED START -----*/

//	Additional Methods
Tango::DevBoolean MLZDevice::update_properties(const Tango::DevVarStringArray *)
{
	return Tango::DevBoolean(true);
}

/*----- PROTECTED REGION END -----*/	//	MLZDevice::namespace_ending
} //	namespace

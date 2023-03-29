/*----- PROTECTED REGION ID(MLZDeviceClass.cpp) ENABLED START -----*/
static const char *RcsId      = "$Id:  $";
static const char *TagName    = "$Name:  $";
static const char *CvsPath    = "$Source:  $";
static const char *SvnPath    = "$HeadURL:  $";
static const char *HttpServer = "http://www.esrf.eu/computing/cs/tango/tango_doc/ds_doc/";
//=============================================================================
//
// file :	MLZDeviceClass.cpp
//
// description : C++ source for the MLZDeviceClass.
//	       A singleton class derived from DeviceClass.
//	       It implements the command and attribute list
//	       and all properties and methods required
//	       by the MLZDevice once per process.
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


#include "MLZDeviceClass.h"

/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass.cpp

//-------------------------------------------------------------------
/**
 *	Create MLZDeviceClass singleton and
 *	return it in a C function for Python usage
 */
//-------------------------------------------------------------------
extern "C" {
#ifdef _TG_WINDOWS_

__declspec(dllexport)

#endif

	Tango::DeviceClass *_create_MLZDevice_class(const char *name) {
		return MLZDevice_ns::MLZDeviceClass::init(name);
	}
}

namespace MLZDevice_ns
{
//===================================================================
//	Initialize pointer for singleton pattern
//===================================================================
MLZDeviceClass *MLZDeviceClass::_instance = NULL;

//--------------------------------------------------------
/**
 * method : 		MLZDeviceClass::MLZDeviceClass(std::string &s)
 * description : 	constructor for the MLZDeviceClass
 *
 * @param s	The class name
 */
//--------------------------------------------------------
MLZDeviceClass::MLZDeviceClass(std::string &s):Tango::DeviceClass(s)
{
	cout2 << "Entering MLZDeviceClass constructor" << std::endl;
	set_default_property();
	write_class_property();

	/*----- PROTECTED REGION ID(MLZDeviceClass::constructor) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::constructor

	cout2 << "Leaving MLZDeviceClass constructor" << std::endl;
}

//--------------------------------------------------------
/**
 * method : 		MLZDeviceClass::~MLZDeviceClass()
 * description : 	destructor for the MLZDeviceClass
 */
//--------------------------------------------------------
MLZDeviceClass::~MLZDeviceClass()
{
	/*----- PROTECTED REGION ID(MLZDeviceClass::destructor) ENABLED START -----*/

	if (_instance)
		delete _instance;

	/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::destructor

	_instance = NULL;
}


//--------------------------------------------------------
/**
 * method : 		MLZDeviceClass::init
 * description : 	Create the object if not already done.
 *                  Otherwise, just return a pointer to the object
 *
 * @param	name	The class name
 */
//--------------------------------------------------------
MLZDeviceClass *MLZDeviceClass::init(const char *name)
{
	if (_instance == NULL)
	{
		try
		{
			std::string s(name);
			_instance = new MLZDeviceClass(s);
		}
		catch (std::bad_alloc &)
		{
			throw;
		}
	}
	return _instance;
}

//--------------------------------------------------------
/**
 * method : 		MLZDeviceClass::instance
 * description : 	Check if object already created,
 *                  and return a pointer to the object
 */
//--------------------------------------------------------
MLZDeviceClass *MLZDeviceClass::instance()
{
	if (_instance == NULL)
	{
		std::cerr << "Class is not initialised !!" << std::endl;
		exit(-1);
	}
	return _instance;
}



//===================================================================
//	Command execution method calls
//===================================================================
//--------------------------------------------------------
/**
 * method : 		OnClass::execute()
 * description : 	method to trigger the execution of the command.
 *
 * @param	device	The device on which the command must be executed
 * @param	in_any	The command input data
 *
 *	returns The command output data (packed in the Any object)
 */
//--------------------------------------------------------
CORBA::Any *OnClass::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
	cout2 << "OnClass::execute(): arrived" << std::endl;
	((static_cast<MLZDevice *>(device))->on());
	return new CORBA::Any();
}

//--------------------------------------------------------
/**
 * method : 		OffClass::execute()
 * description : 	method to trigger the execution of the command.
 *
 * @param	device	The device on which the command must be executed
 * @param	in_any	The command input data
 *
 *	returns The command output data (packed in the Any object)
 */
//--------------------------------------------------------
CORBA::Any *OffClass::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
	cout2 << "OffClass::execute(): arrived" << std::endl;
	((static_cast<MLZDevice *>(device))->off());
	return new CORBA::Any();
}

//--------------------------------------------------------
/**
 * method : 		GetPropertiesClass::execute()
 * description : 	method to trigger the execution of the command.
 *
 * @param	device	The device on which the command must be executed
 * @param	in_any	The command input data
 *
 *	returns The command output data (packed in the Any object)
 */
//--------------------------------------------------------
CORBA::Any *GetPropertiesClass::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
	cout2 << "GetPropertiesClass::execute(): arrived" << std::endl;
	return insert((static_cast<MLZDevice *>(device))->get_properties());
}

//--------------------------------------------------------
/**
 * method : 		SetPropertiesClass::execute()
 * description : 	method to trigger the execution of the command.
 *
 * @param	device	The device on which the command must be executed
 * @param	in_any	The command input data
 *
 *	returns The command output data (packed in the Any object)
 */
//--------------------------------------------------------
CORBA::Any *SetPropertiesClass::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	cout2 << "SetPropertiesClass::execute(): arrived" << std::endl;
	const Tango::DevVarStringArray *argin;
	extract(in_any, argin);
	return insert((static_cast<MLZDevice *>(device))->set_properties(argin));
}

//--------------------------------------------------------
/**
 * method : 		ResetClass::execute()
 * description : 	method to trigger the execution of the command.
 *
 * @param	device	The device on which the command must be executed
 * @param	in_any	The command input data
 *
 *	returns The command output data (packed in the Any object)
 */
//--------------------------------------------------------
CORBA::Any *ResetClass::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
	cout2 << "ResetClass::execute(): arrived" << std::endl;
	((static_cast<MLZDevice *>(device))->reset());
	return new CORBA::Any();
}


//===================================================================
//	Properties management
//===================================================================
//--------------------------------------------------------
/**
 *	Method      : MLZDeviceClass::get_class_property()
 *	Description : Get the class property for specified name.
 */
//--------------------------------------------------------
Tango::DbDatum MLZDeviceClass::get_class_property(std::string &prop_name)
{
	for (unsigned int i=0 ; i<cl_prop.size() ; i++)
		if (cl_prop[i].name == prop_name)
			return cl_prop[i];
	//	if not found, returns  an empty DbDatum
	return Tango::DbDatum(prop_name);
}

//--------------------------------------------------------
/**
 *	Method      : MLZDeviceClass::get_default_device_property()
 *	Description : Return the default value for device property.
 */
//--------------------------------------------------------
Tango::DbDatum MLZDeviceClass::get_default_device_property(std::string &prop_name)
{
	for (unsigned int i=0 ; i<dev_def_prop.size() ; i++)
		if (dev_def_prop[i].name == prop_name)
			return dev_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}

//--------------------------------------------------------
/**
 *	Method      : MLZDeviceClass::get_default_class_property()
 *	Description : Return the default value for class property.
 */
//--------------------------------------------------------
Tango::DbDatum MLZDeviceClass::get_default_class_property(std::string &prop_name)
{
	for (unsigned int i=0 ; i<cl_def_prop.size() ; i++)
		if (cl_def_prop[i].name == prop_name)
			return cl_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}


//--------------------------------------------------------
/**
 *	Method      : MLZDeviceClass::set_default_property()
 *	Description : Set default property (class and device) for wizard.
 *                For each property, add to wizard property name and description.
 *                If default value has been set, add it to wizard property and
 *                store it in a DbDatum.
 */
//--------------------------------------------------------
void MLZDeviceClass::set_default_property()
{
	std::string	prop_name;
	std::string	prop_desc;
	std::string	prop_def;
	std::vector<std::string>	vect_data;

	//	Set Default Class Properties

	//	Set Default device Properties
}

//--------------------------------------------------------
/**
 *	Method      : MLZDeviceClass::write_class_property()
 *	Description : Set class description fields as property in database
 */
//--------------------------------------------------------
void MLZDeviceClass::write_class_property()
{
	//	First time, check if database used
	if (Tango::Util::_UseDb == false)
		return;

	Tango::DbData	data;
	std::string	classname = get_name();
	std::string	header;
	std::string::size_type	start, end;

	//	Put title
	Tango::DbDatum	title("ProjectTitle");
	std::string	str_title("");
	title << str_title;
	data.push_back(title);

	//	Put Description
	Tango::DbDatum	description("Description");
	std::vector<std::string>	str_desc;
	str_desc.push_back("This is the root of the abstract base class inheritance tree.");
	description << str_desc;
	data.push_back(description);

	//  Put inheritance
	Tango::DbDatum	inher_datum("InheritedFrom");
	std::vector<std::string> inheritance;
	inheritance.push_back("TANGO_BASE_CLASS");
	inher_datum << inheritance;
	data.push_back(inher_datum);

	//	Call database and and values
	get_db_class()->put_property(data);
}

//===================================================================
//	Factory methods
//===================================================================

//--------------------------------------------------------
/**
 *	Method      : MLZDeviceClass::device_factory()
 *	Description : Create the device object(s)
 *                and store them in the device list
 */
//--------------------------------------------------------
void MLZDeviceClass::device_factory(const Tango::DevVarStringArray *devlist_ptr)
{
	/*----- PROTECTED REGION ID(MLZDeviceClass::device_factory_before) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::device_factory_before

	//	Create devices and add it into the device list
	for (unsigned long i=0 ; i<devlist_ptr->length() ; i++)
	{
		cout4 << "Device name : " << (*devlist_ptr)[i].in() << std::endl;
		device_list.push_back(new MLZDevice(this, (*devlist_ptr)[i]));
	}

	//	Manage dynamic attributes if any
	erase_dynamic_attributes(devlist_ptr, get_class_attr()->get_attr_list());

	//	Export devices to the outside world
	for (unsigned long i=1 ; i<=devlist_ptr->length() ; i++)
	{
		//	Add dynamic attributes if any
		MLZDevice *dev = static_cast<MLZDevice *>(device_list[device_list.size()-i]);
		dev->add_dynamic_attributes();

		//	Check before if database used.
		if ((Tango::Util::_UseDb == true) && (Tango::Util::_FileDb == false))
			export_device(dev);
		else
			export_device(dev, dev->get_name().c_str());
	}

	/*----- PROTECTED REGION ID(MLZDeviceClass::device_factory_after) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::device_factory_after
}
//--------------------------------------------------------
/**
 *	Method      : MLZDeviceClass::attribute_factory()
 *	Description : Create the attribute object(s)
 *                and store them in the attribute list
 */
//--------------------------------------------------------
void MLZDeviceClass::attribute_factory(std::vector<Tango::Attr *> &att_list)
{
	/*----- PROTECTED REGION ID(MLZDeviceClass::attribute_factory_before) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::attribute_factory_before
	//	Attribute : version
	versionAttrib	*version = new versionAttrib();
	Tango::UserDefaultAttrProp	version_prop;
	version_prop.set_description("This attribute contains the version of the device class and its parent classes (recursively). The format is \"module1 version1, module2 version2, ...\".");
	//	label	not set for version
	//	unit	not set for version
	//	standard_unit	not set for version
	//	display_unit	not set for version
	//	format	not set for version
	//	max_value	not set for version
	//	min_value	not set for version
	//	max_alarm	not set for version
	//	min_alarm	not set for version
	//	max_warning	not set for version
	//	min_warning	not set for version
	//	delta_t	not set for version
	//	delta_val	not set for version

	version->set_default_properties(version_prop);
	//	Not Polled
	version->set_disp_level(Tango::OPERATOR);
	//	Not Memorized
	att_list.push_back(version);


	//	Create a list of static attributes
	create_static_attribute_list(get_class_attr()->get_attr_list());
	/*----- PROTECTED REGION ID(MLZDeviceClass::attribute_factory_after) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::attribute_factory_after
}
//--------------------------------------------------------
/**
 *	Method      : MLZDeviceClass::pipe_factory()
 *	Description : Create the pipe object(s)
 *                and store them in the pipe list
 */
//--------------------------------------------------------
void MLZDeviceClass::pipe_factory()
{
	/*----- PROTECTED REGION ID(MLZDeviceClass::pipe_factory_before) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::pipe_factory_before
	/*----- PROTECTED REGION ID(MLZDeviceClass::pipe_factory_after) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::pipe_factory_after
}
//--------------------------------------------------------
/**
 *	Method      : MLZDeviceClass::command_factory()
 *	Description : Create the command object(s)
 *                and store them in the command list
 */
//--------------------------------------------------------
void MLZDeviceClass::command_factory()
{
	/*----- PROTECTED REGION ID(MLZDeviceClass::command_factory_before) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::command_factory_before


	//	Command On
	OnClass	*pOnCmd =
		new OnClass("On",
			Tango::DEV_VOID, Tango::DEV_VOID,
			"",
			"",
			Tango::OPERATOR);
	command_list.push_back(pOnCmd);

	//	Command Off
	OffClass	*pOffCmd =
		new OffClass("Off",
			Tango::DEV_VOID, Tango::DEV_VOID,
			"",
			"",
			Tango::OPERATOR);
	command_list.push_back(pOffCmd);

	//	Command GetProperties
	GetPropertiesClass	*pGetPropertiesCmd =
		new GetPropertiesClass("GetProperties",
			Tango::DEV_VOID, Tango::DEVVAR_STRINGARRAY,
			"",
			"",
			Tango::OPERATOR);
	command_list.push_back(pGetPropertiesCmd);

	//	Command SetProperties
	SetPropertiesClass	*pSetPropertiesCmd =
		new SetPropertiesClass("SetProperties",
			Tango::DEVVAR_STRINGARRAY, Tango::DEV_BOOLEAN,
			"",
			"True if the properties were saved persistently, False if they were only set for the current session.",
			Tango::OPERATOR);
	command_list.push_back(pSetPropertiesCmd);

	//	Command Reset
	ResetClass	*pResetCmd =
		new ResetClass("Reset",
			Tango::DEV_VOID, Tango::DEV_VOID,
			"",
			"",
			Tango::OPERATOR);
	command_list.push_back(pResetCmd);

	/*----- PROTECTED REGION ID(MLZDeviceClass::command_factory_after) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::command_factory_after
}

//===================================================================
//	Dynamic attributes related methods
//===================================================================

//--------------------------------------------------------
/**
 * method : 		MLZDeviceClass::create_static_attribute_list
 * description : 	Create the a list of static attributes
 *
 * @param	att_list	the ceated attribute list
 */
//--------------------------------------------------------
void MLZDeviceClass::create_static_attribute_list(std::vector<Tango::Attr *> &att_list)
{
	for (unsigned long i=0 ; i<att_list.size() ; i++)
	{
		std::string att_name(att_list[i]->get_name());
		transform(att_name.begin(), att_name.end(), att_name.begin(), ::tolower);
		defaultAttList.push_back(att_name);
	}

	cout2 << defaultAttList.size() << " attributes in default list" << std::endl;

	/*----- PROTECTED REGION ID(MLZDeviceClass::create_static_att_list) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::create_static_att_list
}


//--------------------------------------------------------
/**
 * method : 		MLZDeviceClass::erase_dynamic_attributes
 * description : 	delete the dynamic attributes if any.
 *
 * @param	devlist_ptr	the device list pointer
 * @param	list of all attributes
 */
//--------------------------------------------------------
void MLZDeviceClass::erase_dynamic_attributes(const Tango::DevVarStringArray *devlist_ptr, std::vector<Tango::Attr *> &att_list)
{
	Tango::Util *tg = Tango::Util::instance();

	for (unsigned long i=0 ; i<devlist_ptr->length() ; i++)
	{
		Tango::DeviceImpl *dev_impl = tg->get_device_by_name(((std::string)(*devlist_ptr)[i]).c_str());
		MLZDevice *dev = static_cast<MLZDevice *> (dev_impl);

		std::vector<Tango::Attribute *> &dev_att_list = dev->get_device_attr()->get_attribute_list();
		std::vector<Tango::Attribute *>::iterator ite_att;
		for (ite_att=dev_att_list.begin() ; ite_att != dev_att_list.end() ; ++ite_att)
		{
			std::string att_name((*ite_att)->get_name_lower());
			if ((att_name == "state") || (att_name == "status"))
				continue;
			std::vector<std::string>::iterator ite_str = find(defaultAttList.begin(), defaultAttList.end(), att_name);
			if (ite_str == defaultAttList.end())
			{
				cout2 << att_name << " is a UNWANTED dynamic attribute for device " << (*devlist_ptr)[i] << std::endl;
				Tango::Attribute &att = dev->get_device_attr()->get_attr_by_name(att_name.c_str());
				dev->remove_attribute(att_list[att.get_attr_idx()], true, false);
				--ite_att;
			}
		}
	}
	/*----- PROTECTED REGION ID(MLZDeviceClass::erase_dynamic_attributes) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::erase_dynamic_attributes
}

//--------------------------------------------------------
/**
 *	Method      : MLZDeviceClass::get_attr_by_name()
 *	Description : returns Tango::Attr * object found by name
 */
//--------------------------------------------------------
Tango::Attr *MLZDeviceClass::get_attr_object_by_name(std::vector<Tango::Attr *> &att_list, std::string attname)
{
	std::vector<Tango::Attr *>::iterator it;
	for (it=att_list.begin() ; it<att_list.end() ; ++it)
		if ((*it)->get_name()==attname)
			return (*it);
	//	Attr does not exist
	return NULL;
}


/*----- PROTECTED REGION ID(MLZDeviceClass::Additional Methods) ENABLED START -----*/

/*----- PROTECTED REGION END -----*/	//	MLZDeviceClass::Additional Methods
} //	namespace

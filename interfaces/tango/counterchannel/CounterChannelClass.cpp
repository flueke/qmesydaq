/*----- PROTECTED REGION ID(CounterChannelClass.cpp) ENABLED START -----*/
//=============================================================================
//
// file :        CounterChannelClass.cpp
//
// description : C++ source for the CounterChannelClass.
//               A singleton class derived from DeviceClass.
//               It implements the command and attribute list
//               and all properties and methods required
//               by the CounterChannel once per process.
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


#include <CounterChannelClass.h>

/*----- PROTECTED REGION END -----*/	//	CounterChannelClass.cpp

//-------------------------------------------------------------------
/**
 *	Create CounterChannelClass singleton and
 *	return it in a C function for Python usage
 */
//-------------------------------------------------------------------
extern "C" {
#ifdef _TG_WINDOWS_

__declspec(dllexport)

#endif

	Tango::DeviceClass *_create_CounterChannel_class(const char *name) {
		return CounterChannel_ns::CounterChannelClass::init(name);
	}
}

namespace CounterChannel_ns
{
//===================================================================
//	Initialize pointer for singleton pattern
//===================================================================
CounterChannelClass *CounterChannelClass::_instance = NULL;

//--------------------------------------------------------
/**
 * method : 		CounterChannelClass::CounterChannelClass(string &s)
 * description : 	constructor for the CounterChannelClass
 *
 * @param s	The class name
 */
//--------------------------------------------------------
CounterChannelClass::CounterChannelClass(string &s):DetectorChannel_ns::DetectorChannelClass(s)
{
	cout2 << "Entering CounterChannelClass constructor" << endl;
	set_default_property();
	write_class_property();

	/*----- PROTECTED REGION ID(CounterChannelClass::constructor) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::constructor

	cout2 << "Leaving CounterChannelClass constructor" << endl;
}

//--------------------------------------------------------
/**
 * method : 		CounterChannelClass::~CounterChannelClass()
 * description : 	destructor for the CounterChannelClass
 */
//--------------------------------------------------------
CounterChannelClass::~CounterChannelClass()
{
	/*----- PROTECTED REGION ID(CounterChannelClass::destructor) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::destructor

	_instance = NULL;
}


//--------------------------------------------------------
/**
 * method : 		CounterChannelClass::init
 * description : 	Create the object if not already done.
 *                  Otherwise, just return a pointer to the object
 *
 * @param	name	The class name
 */
//--------------------------------------------------------
CounterChannelClass *CounterChannelClass::init(const char *name)
{
	if (_instance == NULL)
	{
		try
		{
			string s(name);
			_instance = new CounterChannelClass(s);
		}
		catch (bad_alloc &)
		{
			throw;
		}
	}
	return _instance;
}

//--------------------------------------------------------
/**
 * method : 		CounterChannelClass::instance
 * description : 	Check if object already created,
 *                  and return a pointer to the object
 */
//--------------------------------------------------------
CounterChannelClass *CounterChannelClass::instance()
{
	if (_instance == NULL)
	{
		cerr << "Class is not initialised !!" << endl;
		exit(-1);
	}
	return _instance;
}



//===================================================================
//	Command execution method calls
//===================================================================

//===================================================================
//	Properties management
//===================================================================
//--------------------------------------------------------
/**
 *	Method      : CounterChannelClass::get_class_property()
 *	Description : Get the class property for specified name.
 */
//--------------------------------------------------------
Tango::DbDatum CounterChannelClass::get_class_property(string &prop_name)
{
	for (unsigned int i=0 ; i<cl_prop.size() ; i++)
		if (cl_prop[i].name == prop_name)
			return cl_prop[i];
	//	if not found, returns  an empty DbDatum
	return Tango::DbDatum(prop_name);
}

//--------------------------------------------------------
/**
 *	Method      : CounterChannelClass::get_default_device_property()
 *	Description : Return the default value for device property.
 */
//--------------------------------------------------------
Tango::DbDatum CounterChannelClass::get_default_device_property(string &prop_name)
{
	for (unsigned int i=0 ; i<dev_def_prop.size() ; i++)
		if (dev_def_prop[i].name == prop_name)
			return dev_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}

//--------------------------------------------------------
/**
 *	Method      : CounterChannelClass::get_default_class_property()
 *	Description : Return the default value for class property.
 */
//--------------------------------------------------------
Tango::DbDatum CounterChannelClass::get_default_class_property(string &prop_name)
{
	for (unsigned int i=0 ; i<cl_def_prop.size() ; i++)
		if (cl_def_prop[i].name == prop_name)
			return cl_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}


//--------------------------------------------------------
/**
 *	Method      : CounterChannelClass::set_default_property()
 *	Description : Set default property (class and device) for wizard.
 *                For each property, add to wizard property name and description.
 *                If default value has been set, add it to wizard property and
 *                store it in a DbDatum.
 */
//--------------------------------------------------------
void CounterChannelClass::set_default_property()
{
	string	prop_name;
	string	prop_desc;
	string	prop_def;
	vector<string>	vect_data;

	//	Set Default Class Properties

	//	Set Default device Properties
	prop_name = "channel";
	prop_desc = "What monitor input is used.";
	prop_def  = "0";
	vect_data.clear();
	vect_data.push_back("0");
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);
}

//--------------------------------------------------------
/**
 *	Method      : CounterChannelClass::write_class_property()
 *	Description : Set class description fields as property in database
 */
//--------------------------------------------------------
void CounterChannelClass::write_class_property()
{
	//	First time, check if database used
	if (Tango::Util::_UseDb == false)
		return;

	Tango::DbData	data;
	string	classname = get_name();
	string	header;
	string::size_type	start, end;

	//	Put title
	Tango::DbDatum	title("ProjectTitle");
	string	str_title("");
	title << str_title;
	data.push_back(title);

	//	Put Description
	Tango::DbDatum	description("Description");
	vector<string>	str_desc;
	str_desc.push_back("Base class for channels that control monitor or single detector counts.");
	description << str_desc;
	data.push_back(description);

	//  Put inheritance
	Tango::DbDatum	inher_datum("InheritedFrom");
	vector<string> inheritance;
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
 *	Method      : CounterChannelClass::device_factory()
 *	Description : Create the device object(s)
 *                and store them in the device list
 */
//--------------------------------------------------------
void CounterChannelClass::device_factory(const Tango::DevVarStringArray *devlist_ptr)
{
	/*----- PROTECTED REGION ID(CounterChannelClass::device_factory_before) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::device_factory_before

	//	Create devices and add it into the device list
	for (unsigned long i=0 ; i<devlist_ptr->length() ; i++)
	{
		cout4 << "Device name : " << (*devlist_ptr)[i].in() << endl;
		device_list.push_back(new CounterChannel(this, (*devlist_ptr)[i]));
	}

	//	Manage dynamic attributes if any
	erase_dynamic_attributes(devlist_ptr, get_class_attr()->get_attr_list());

	//	Export devices to the outside world
	for (unsigned long i=1 ; i<=devlist_ptr->length() ; i++)
	{
		//	Add dynamic attributes if any
		CounterChannel *dev = static_cast<CounterChannel *>(device_list[device_list.size()-i]);
		dev->add_dynamic_attributes();

		//	Check before if database used.
		if ((Tango::Util::_UseDb == true) && (Tango::Util::_FileDb == false))
			export_device(dev);
		else
			export_device(dev, dev->get_name().c_str());
	}

	/*----- PROTECTED REGION ID(CounterChannelClass::device_factory_after) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::device_factory_after
}
//--------------------------------------------------------
/**
 *	Method      : CounterChannelClass::attribute_factory()
 *	Description : Create the attribute object(s)
 *                and store them in the attribute list
 */
//--------------------------------------------------------
void CounterChannelClass::attribute_factory(vector<Tango::Attr *> &att_list)
{
	/*----- PROTECTED REGION ID(CounterChannelClass::attribute_factory_before) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::attribute_factory_before
	//	Call atribute_factory for inherited class
	DetectorChannel_ns::DetectorChannelClass::attribute_factory(att_list);

	//	Attribute : version - Check if not concrete in inherited class
	Tango::Attr *versionAttr = get_attr_object_by_name(att_list, "version");
	if (versionAttr == NULL)
	{
	}

	//	Attribute : active - Check if not concrete in inherited class
	Tango::Attr *activeAttr = get_attr_object_by_name(att_list, "active");
	if (activeAttr == NULL)
	{
	}

	//	Attribute : value
	valueAttrib	*value = new valueAttrib();
	Tango::UserDefaultAttrProp	value_prop;
	value_prop.set_description("Current counter value.");
	//	label	not set for value
	//	unit	not set for value
	//	standard_unit	not set for value
	//	display_unit	not set for value
	//	format	not set for value
	//	max_value	not set for value
	//	min_value	not set for value
	//	max_alarm	not set for value
	//	min_alarm	not set for value
	//	max_warning	not set for value
	//	min_warning	not set for value
	//	delta_t	not set for value
	//	delta_val	not set for value
	
	value->set_default_properties(value_prop);
	//	Not Polled
	value->set_disp_level(Tango::OPERATOR);
	//	Not Memorized
	att_list.push_back(value);

	//	Attribute : preselection
	preselectionAttrib	*preselection = new preselectionAttrib();
	Tango::UserDefaultAttrProp	preselection_prop;
	preselection_prop.set_description("Current preset value.");
	//	label	not set for preselection
	//	unit	not set for preselection
	//	standard_unit	not set for preselection
	//	display_unit	not set for preselection
	//	format	not set for preselection
	//	max_value	not set for preselection
	//	min_value	not set for preselection
	//	max_alarm	not set for preselection
	//	min_alarm	not set for preselection
	//	max_warning	not set for preselection
	//	min_warning	not set for preselection
	//	delta_t	not set for preselection
	//	delta_val	not set for preselection
	
	preselection->set_default_properties(preselection_prop);
	//	Not Polled
	preselection->set_disp_level(Tango::OPERATOR);
	preselection->set_memorized();
	preselection->set_memorized_init(true);
	att_list.push_back(preselection);


	//	Create a list of static attributes
	create_static_attribute_list(get_class_attr()->get_attr_list());
	/*----- PROTECTED REGION ID(CounterChannelClass::attribute_factory_after) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::attribute_factory_after
}
//--------------------------------------------------------
/**
 *	Method      : CounterChannelClass::pipe_factory()
 *	Description : Create the pipe object(s)
 *                and store them in the pipe list
 */
//--------------------------------------------------------
void CounterChannelClass::pipe_factory()
{
	/*----- PROTECTED REGION ID(CounterChannelClass::pipe_factory_before) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::pipe_factory_before
	/*----- PROTECTED REGION ID(CounterChannelClass::pipe_factory_after) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::pipe_factory_after
}
//--------------------------------------------------------
/**
 *	Method      : CounterChannelClass::command_factory()
 *	Description : Create the command object(s)
 *                and store them in the command list
 */
//--------------------------------------------------------
void CounterChannelClass::command_factory()
{
	/*----- PROTECTED REGION ID(CounterChannelClass::command_factory_before) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::command_factory_before
	//	Call command_factory for inherited class
	DetectorChannel_ns::DetectorChannelClass::command_factory();













	/*----- PROTECTED REGION ID(CounterChannelClass::command_factory_after) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::command_factory_after
}

//===================================================================
//	Dynamic attributes related methods
//===================================================================

//--------------------------------------------------------
/**
 * method : 		CounterChannelClass::create_static_attribute_list
 * description : 	Create the a list of static attributes
 *
 * @param	att_list	the ceated attribute list
 */
//--------------------------------------------------------
void CounterChannelClass::create_static_attribute_list(vector<Tango::Attr *> &att_list)
{
	for (unsigned long i=0 ; i<att_list.size() ; i++)
	{
		string att_name(att_list[i]->get_name());
		transform(att_name.begin(), att_name.end(), att_name.begin(), ::tolower);
		defaultAttList.push_back(att_name);
	}

	cout2 << defaultAttList.size() << " attributes in default list" << endl;

	/*----- PROTECTED REGION ID(CounterChannelClass::create_static_att_list) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::create_static_att_list
}


//--------------------------------------------------------
/**
 * method : 		CounterChannelClass::erase_dynamic_attributes
 * description : 	delete the dynamic attributes if any.
 *
 * @param	devlist_ptr	the device list pointer
 * @param	list of all attributes
 */
//--------------------------------------------------------
void CounterChannelClass::erase_dynamic_attributes(const Tango::DevVarStringArray *devlist_ptr, vector<Tango::Attr *> &att_list)
{
	Tango::Util *tg = Tango::Util::instance();

	for (unsigned long i=0 ; i<devlist_ptr->length() ; i++)
	{
		Tango::DeviceImpl *dev_impl = tg->get_device_by_name(((string)(*devlist_ptr)[i]).c_str());
		CounterChannel *dev = static_cast<CounterChannel *> (dev_impl);

		vector<Tango::Attribute *> &dev_att_list = dev->get_device_attr()->get_attribute_list();
		vector<Tango::Attribute *>::iterator ite_att;
		for (ite_att=dev_att_list.begin() ; ite_att != dev_att_list.end() ; ++ite_att)
		{
			string att_name((*ite_att)->get_name_lower());
			if ((att_name == "state") || (att_name == "status"))
				continue;
			vector<string>::iterator ite_str = find(defaultAttList.begin(), defaultAttList.end(), att_name);
			if (ite_str == defaultAttList.end())
			{
				cout2 << att_name << " is a UNWANTED dynamic attribute for device " << (*devlist_ptr)[i] << endl;
				Tango::Attribute &att = dev->get_device_attr()->get_attr_by_name(att_name.c_str());
				dev->remove_attribute(att_list[att.get_attr_idx()], true, false);
				--ite_att;
			}
		}
	}
	/*----- PROTECTED REGION ID(CounterChannelClass::erase_dynamic_attributes) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::erase_dynamic_attributes
}

//--------------------------------------------------------
/**
 *	Method      : CounterChannelClass::get_attr_by_name()
 *	Description : returns Tango::Attr * object found by name
 */
//--------------------------------------------------------
Tango::Attr *CounterChannelClass::get_attr_object_by_name(vector<Tango::Attr *> &att_list, string attname)
{
	vector<Tango::Attr *>::iterator it;
	for (it=att_list.begin() ; it<att_list.end() ; ++it)
		if ((*it)->get_name()==attname)
			return (*it);
	//	Attr does not exist
	return NULL;
}


/*----- PROTECTED REGION ID(CounterChannelClass::Additional Methods) ENABLED START -----*/

/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::Additional Methods
} //	namespace

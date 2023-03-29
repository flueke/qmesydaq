/*----- PROTECTED REGION ID(CounterChannelClass.h) ENABLED START -----*/
//=============================================================================
//
// file :        CounterChannelClass.h
//
// description : Include for the CounterChannel root class.
//               This class is the singleton class for
//                the CounterChannel device class.
//               It contains all properties and methods which the
//               CounterChannel requires only once e.g. the commands.
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


#ifndef CounterChannelClass_H
#define CounterChannelClass_H

#include <tango.h>
#include <MLZDeviceClass.h>
#include <DetectorChannelClass.h>
#include <CounterChannel.h>


/*----- PROTECTED REGION END -----*/	//	CounterChannelClass.h


namespace CounterChannel_ns
{
/*----- PROTECTED REGION ID(CounterChannelClass::classes for dynamic creation) ENABLED START -----*/


/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::classes for dynamic creation

//=========================================
//	Define classes for attributes
//=========================================
//	Attribute value class definition
class valueAttrib: public Tango::Attr
{
public:
	valueAttrib():Attr("value",
			Tango::DEV_ULONG64, Tango::READ) {};
	~valueAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<CounterChannel *>(dev))->read_value(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<CounterChannel *>(dev))->is_value_allowed(ty);}
};

//	Attribute preselection class definition
class preselectionAttrib: public Tango::Attr
{
public:
	preselectionAttrib():Attr("preselection",
			Tango::DEV_ULONG64, Tango::READ_WRITE) {};
	~preselectionAttrib() {};
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<CounterChannel *>(dev))->read_preselection(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<CounterChannel *>(dev))->write_preselection(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<CounterChannel *>(dev))->is_preselection_allowed(ty);}
};


//=========================================
//	Define classes for commands
//=========================================

/**
 *	The CounterChannelClass singleton definition
 */

#ifdef _TG_WINDOWS_
class __declspec(dllexport)  CounterChannelClass : public DetectorChannel_ns::DetectorChannelClass
#else
class CounterChannelClass : public DetectorChannel_ns::DetectorChannelClass
#endif
{
	/*----- PROTECTED REGION ID(CounterChannelClass::Additionnal DServer data members) ENABLED START -----*/


	/*----- PROTECTED REGION END -----*/	//	CounterChannelClass::Additionnal DServer data members

	public:
		//	write class properties data members
		Tango::DbData	cl_prop;
		Tango::DbData	cl_def_prop;
		Tango::DbData	dev_def_prop;

		//	Method prototypes
		static CounterChannelClass *init(const char *);
		static CounterChannelClass *instance();
		~CounterChannelClass();
		Tango::DbDatum	get_class_property(std::string &);
		Tango::DbDatum	get_default_device_property(std::string &);
		Tango::DbDatum	get_default_class_property(std::string &);

	protected:
		CounterChannelClass(std::string &);
		static CounterChannelClass *_instance;
		void command_factory();
		void attribute_factory(std::vector<Tango::Attr *> &);
		void pipe_factory();
		void write_class_property();
		void set_default_property();
		void get_class_property();
		std::string get_cvstag();
		std::string get_cvsroot();

	private:
		void device_factory(const Tango::DevVarStringArray *);
		void create_static_attribute_list(std::vector<Tango::Attr *> &);
		void erase_dynamic_attributes(const Tango::DevVarStringArray *,std::vector<Tango::Attr *> &);
		std::vector<std::string>	defaultAttList;
		Tango::Attr *get_attr_object_by_name(std::vector<Tango::Attr *> &att_list, std::string attname);
};

}	//	End of namespace

#endif   //	CounterChannel_H

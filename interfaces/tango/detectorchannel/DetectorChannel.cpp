/*----- PROTECTED REGION ID(DetectorChannel.cpp) ENABLED START -----*/
//=============================================================================
//
// file :        DetectorChannel.cpp
//
// description : C++ source for the DetectorChannel class and its commands.
//               The class is derived from Device. It represents the
//               CORBA servant object which will be accessed from the
//               network. All commands which can be executed on the
//               DetectorChannel are implemented in this file.
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


#include <DetectorChannel.h>
#include <DetectorChannelClass.h>

#include "MultipleLoopApplication.h"
#include "QMesydaqDetectorInterface.h"

/*----- PROTECTED REGION END -----*/	//	DetectorChannel.cpp

/**
 *  DetectorChannel class description:
 *    Abstract base class for a detector channel.
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
//  Start          |  start
//  Stop           |  stop
//  Clear          |  clear
//  Resume         |  resume
//  Prepare        |  prepare
//================================================================

//================================================================
//  Attributes managed are:
//================================================================
//  version  |  Tango::DevString	Scalar
//  active   |  Tango::DevBoolean	Scalar
//================================================================

namespace DetectorChannel_ns
{
/*----- PROTECTED REGION ID(DetectorChannel::namespace_starting) ENABLED START -----*/

//	static initializations

/*----- PROTECTED REGION END -----*/	//	DetectorChannel::namespace_starting

//--------------------------------------------------------
/**
 *	Method      : DetectorChannel::DetectorChannel()
 *	Description : Constructors for a Tango device
 *                implementing the classDetectorChannel
 */
//--------------------------------------------------------
DetectorChannel::DetectorChannel(Tango::DeviceClass *cl, string &s)
 : MLZDevice(cl, s.c_str())
{
	/*----- PROTECTED REGION ID(DetectorChannel::constructor_1) ENABLED START -----*/

	init_device();

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::constructor_1
}
//--------------------------------------------------------
DetectorChannel::DetectorChannel(Tango::DeviceClass *cl, const char *s)
 : MLZDevice(cl, s)
{
	/*----- PROTECTED REGION ID(DetectorChannel::constructor_2) ENABLED START -----*/

	init_device();

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::constructor_2
}
//--------------------------------------------------------
DetectorChannel::DetectorChannel(Tango::DeviceClass *cl, const char *s, const char *d)
 : MLZDevice(cl, s, d)
{
	/*----- PROTECTED REGION ID(DetectorChannel::constructor_3) ENABLED START -----*/

	init_device();

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::constructor_3
}

//--------------------------------------------------------
/**
 *	Method      : DetectorChannel::delete_device()
 *	Description : will be called at device destruction or at init command
 */
//--------------------------------------------------------
void DetectorChannel::delete_device()
{
	DEBUG_STREAM << "DetectorChannel::delete_device() " << device_name << endl;
	/*----- PROTECTED REGION ID(DetectorChannel::delete_device) ENABLED START -----*/

	//	Delete device allocated objects

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::delete_device
	delete[] attr_active_read;

	if (Tango::Util::instance()->is_svr_shutting_down()==false  &&
		Tango::Util::instance()->is_device_restarting(device_name)==false &&
		Tango::Util::instance()->is_svr_starting()==false)
	{
		//	If not shutting down call delete device for inherited object
		MLZDevice_ns::MLZDevice::delete_device();
	}
}

//--------------------------------------------------------
/**
 *	Method      : DetectorChannel::init_device()
 *	Description : will be called at device initialization.
 */
//--------------------------------------------------------
void DetectorChannel::init_device()
{
	DEBUG_STREAM << "DetectorChannel::init_device() create device " << device_name << endl;
	/*----- PROTECTED REGION ID(DetectorChannel::init_device_before) ENABLED START -----*/

	MultipleLoopApplication *app = dynamic_cast<MultipleLoopApplication *>(QApplication::instance());
	if (app)
		m_interface = dynamic_cast<QMesyDAQDetectorInterface *>(app->getQtInterface());
	else
		m_interface = NULL;
	//	Initialization before get_device_property() call

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::init_device_before
	
	if (Tango::Util::instance()->is_svr_starting() == false  &&
		Tango::Util::instance()->is_device_restarting(device_name)==false)
	{
		//	If not starting up call init device for inherited object
		MLZDevice_ns::MLZDevice::init_device();
	}

	//	Get the device properties from database
	get_device_property();
	
	attr_active_read = new Tango::DevBoolean[1];
	/*----- PROTECTED REGION ID(DetectorChannel::init_device) ENABLED START -----*/

	//	Initialize device
	attr_active_read[0] = false;

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::init_device
}

//--------------------------------------------------------
/**
 *	Method      : DetectorChannel::get_device_property()
 *	Description : Read database to initialize property data members.
 */
//--------------------------------------------------------
void DetectorChannel::get_device_property()
{
	/*----- PROTECTED REGION ID(DetectorChannel::get_device_property_before) ENABLED START -----*/
	
	//	Initialize property data members
	
	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::get_device_property_before


	//	Read device properties from database.
	Tango::DbData	dev_prop;
	dev_prop.push_back(Tango::DbDatum("writelistmode"));
	dev_prop.push_back(Tango::DbDatum("writehistogram"));
	dev_prop.push_back(Tango::DbDatum("configfile"));
	dev_prop.push_back(Tango::DbDatum("calibrationfile"));
	dev_prop.push_back(Tango::DbDatum("runid"));
	dev_prop.push_back(Tango::DbDatum("lastlistfile"));
	dev_prop.push_back(Tango::DbDatum("lasthistfile"));
	dev_prop.push_back(Tango::DbDatum("lastbinnedfile"));

	//	is there at least one property to be read ?
	if (dev_prop.size()>0)
	{
		//	Call database and extract values
		if (Tango::Util::instance()->_UseDb==true)
			get_db_device()->get_property(dev_prop);
	
		//	get instance on DetectorChannelClass to get class property
		Tango::DbDatum	def_prop, cl_prop;
		DetectorChannelClass	*ds_class =
			(static_cast<DetectorChannelClass *>(get_device_class()));
		int	i = -1;

		//	Try to initialize writelistmode from class property
		cl_prop = ds_class->get_class_property(dev_prop[++i].name);
		if (cl_prop.is_empty()==false)	cl_prop  >>  writelistmode;
		else {
			//	Try to initialize writelistmode from default device value
			def_prop = ds_class->get_default_device_property(dev_prop[i].name);
			if (def_prop.is_empty()==false)	def_prop  >>  writelistmode;
		}
		//	And try to extract writelistmode value from database
		if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  writelistmode;

		//	Try to initialize writehistogram from class property
		cl_prop = ds_class->get_class_property(dev_prop[++i].name);
		if (cl_prop.is_empty()==false)	cl_prop  >>  writehistogram;
		else {
			//	Try to initialize writehistogram from default device value
			def_prop = ds_class->get_default_device_property(dev_prop[i].name);
			if (def_prop.is_empty()==false)	def_prop  >>  writehistogram;
		}
		//	And try to extract writehistogram value from database
		if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  writehistogram;

		//	Try to initialize configfile from class property
		cl_prop = ds_class->get_class_property(dev_prop[++i].name);
		if (cl_prop.is_empty()==false)	cl_prop  >>  configfile;
		else {
			//	Try to initialize configfile from default device value
			def_prop = ds_class->get_default_device_property(dev_prop[i].name);
			if (def_prop.is_empty()==false)	def_prop  >>  configfile;
		}
		//	And try to extract configfile value from database
		if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  configfile;

		//	Try to initialize calibrationfile from class property
		cl_prop = ds_class->get_class_property(dev_prop[++i].name);
		if (cl_prop.is_empty()==false)	cl_prop  >>  calibrationfile;
		else {
			//	Try to initialize calibrationfile from default device value
			def_prop = ds_class->get_default_device_property(dev_prop[i].name);
			if (def_prop.is_empty()==false)	def_prop  >>  calibrationfile;
		}
		//	And try to extract calibrationfile value from database
		if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  calibrationfile;

		//	Try to initialize runid from class property
		cl_prop = ds_class->get_class_property(dev_prop[++i].name);
		if (cl_prop.is_empty()==false)	cl_prop  >>  runid;
		else {
			//	Try to initialize runid from default device value
			def_prop = ds_class->get_default_device_property(dev_prop[i].name);
			if (def_prop.is_empty()==false)	def_prop  >>  runid;
		}
		//	And try to extract runid value from database
		if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  runid;

		//	Try to initialize lastlistfile from class property
		cl_prop = ds_class->get_class_property(dev_prop[++i].name);
		if (cl_prop.is_empty()==false)	cl_prop  >>  lastlistfile;
		else {
			//	Try to initialize lastlistfile from default device value
			def_prop = ds_class->get_default_device_property(dev_prop[i].name);
			if (def_prop.is_empty()==false)	def_prop  >>  lastlistfile;
		}
		//	And try to extract lastlistfile value from database
		if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  lastlistfile;

		//	Try to initialize lasthistfile from class property
		cl_prop = ds_class->get_class_property(dev_prop[++i].name);
		if (cl_prop.is_empty()==false)	cl_prop  >>  lasthistfile;
		else {
			//	Try to initialize lasthistfile from default device value
			def_prop = ds_class->get_default_device_property(dev_prop[i].name);
			if (def_prop.is_empty()==false)	def_prop  >>  lasthistfile;
		}
		//	And try to extract lasthistfile value from database
		if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  lasthistfile;

		//	Try to initialize lastbinnedfile from class property
		cl_prop = ds_class->get_class_property(dev_prop[++i].name);
		if (cl_prop.is_empty()==false)	cl_prop  >>  lastbinnedfile;
		else {
			//	Try to initialize lastbinnedfile from default device value
			def_prop = ds_class->get_default_device_property(dev_prop[i].name);
			if (def_prop.is_empty()==false)	def_prop  >>  lastbinnedfile;
		}
		//	And try to extract lastbinnedfile value from database
		if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  lastbinnedfile;

	}

	/*----- PROTECTED REGION ID(DetectorChannel::get_device_property_after) ENABLED START -----*/
	
	//	Check device property data members init
	
	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::get_device_property_after
}

//--------------------------------------------------------
/**
 *	Method      : DetectorChannel::always_executed_hook()
 *	Description : method always executed before any command is executed
 */
//--------------------------------------------------------
void DetectorChannel::always_executed_hook()
{
	DEBUG_STREAM << "DetectorChannel::always_executed_hook()  " << device_name << endl;
	/*----- PROTECTED REGION ID(DetectorChannel::always_executed_hook) ENABLED START -----*/

	//	code always executed before all requests

	if (!m_interface)
		set_state(Tango::FAULT);

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::always_executed_hook
}

//--------------------------------------------------------
/**
 *	Method      : DetectorChannel::read_attr_hardware()
 *	Description : Hardware acquisition for attributes
 */
//--------------------------------------------------------
void DetectorChannel::read_attr_hardware(TANGO_UNUSED(vector<long> &attr_list))
{
	DEBUG_STREAM << "DetectorChannel::read_attr_hardware(vector<long> &attr_list) entering... " << endl;
	/*----- PROTECTED REGION ID(DetectorChannel::read_attr_hardware) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::read_attr_hardware
}
//--------------------------------------------------------
/**
 *	Method      : DetectorChannel::write_attr_hardware()
 *	Description : Hardware writing for attributes
 */
//--------------------------------------------------------
void DetectorChannel::write_attr_hardware(TANGO_UNUSED(vector<long> &attr_list))
{
	DEBUG_STREAM << "DetectorChannel::write_attr_hardware(vector<long> &attr_list) entering... " << endl;
	/*----- PROTECTED REGION ID(DetectorChannel::write_attr_hardware) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::write_attr_hardware
}

//--------------------------------------------------------
/**
 *	Read attribute active related method
 *	Description: If this channel can finish the measurement when preselection is reached.
 *
 *	Data type:	Tango::DevBoolean
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void DetectorChannel::read_active(Tango::Attribute &attr)
{
	DEBUG_STREAM << "DetectorChannel::read_active(Tango::Attribute &attr) entering... " << endl;
	/*----- PROTECTED REGION ID(DetectorChannel::read_active) ENABLED START -----*/
	//	Set the attribute value
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
void DetectorChannel::write_active(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "DetectorChannel::write_active(Tango::WAttribute &attr) entering... " << endl;
	//	Retrieve write value
	Tango::DevBoolean	w_val;
	attr.get_write_value(w_val);
	/*----- PROTECTED REGION ID(DetectorChannel::write_active) ENABLED START -----*/


	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::write_active
}

//--------------------------------------------------------
/**
 *	Method      : DetectorChannel::add_dynamic_attributes()
 *	Description : Create the dynamic attributes if any
 *                for specified device.
 */
//--------------------------------------------------------
void DetectorChannel::add_dynamic_attributes()
{
	/*----- PROTECTED REGION ID(DetectorChannel::add_dynamic_attributes) ENABLED START -----*/

	//	Add your own code to create and add dynamic attributes if any

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::add_dynamic_attributes
}

//--------------------------------------------------------
/**
 *	Command Start related method
 *	Description: Starts the acquisition.
 *
 */
//--------------------------------------------------------
void DetectorChannel::start()
{
	DEBUG_STREAM << "DetectorChannel::Start()  - " << device_name << endl;
	/*----- PROTECTED REGION ID(DetectorChannel::start) ENABLED START -----*/
	//	Add your own code
	if (!m_interface)
		::Tango::Except::throw_exception("Runtime error",
						 "Control interface not initialized",
						 "DetectorChannel::start()");

	if (isMaster() && dev_state() != ::Tango::MOVING)
	{
		m_started = true;
		set_state(Tango::MOVING);
#if 0
		m_interface->setListMode(m_writeListmode, true);
		if (m_writeListmode)
		{
			m_listFilename = incNumber(m_listFilename);
			updateResource<std::string>("lastlistfile", m_listFilename);
			m_interface->setListFileName(m_listFilename.c_str());
		}

		m_interface->setHistogramMode(m_writeHistogram);
		if (m_writeHistogram)
		{
			m_histFilename = incNumber(m_histFilename);
			updateResource<std::string>("lasthistfile", m_histFilename);
			m_interface->setHistogramFileName(m_histFilename.c_str());
		}
#endif
		ERROR_STREAM << "interface::start()";
		m_interface->start();
		for (int i = 0; i < 10; ++i)
		{
			usleep(300000);
			if (m_interface->status())
				break;
		}
		m_started = false;
	}

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::start
}
//--------------------------------------------------------
/**
 *	Command Stop related method
 *	Description: Stops a running acquisition.
 *
 */
//--------------------------------------------------------
void DetectorChannel::stop()
{
	DEBUG_STREAM << "DetectorChannel::Stop()  - " << device_name << endl;
	/*----- PROTECTED REGION ID(DetectorChannel::stop) ENABLED START -----*/
	//	Add your own code
	if (!m_interface)
		::Tango::Except::throw_exception("Runtime error",
						 "Control interface not initialized",
						 "DetectorChannel::stop()");
	if (isMaster() && dev_state() == Tango::MOVING)
		m_interface->stop();

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::stop
}
//--------------------------------------------------------
/**
 *	Command Clear related method
 *	Description: Clears all values of the detector.
 *
 */
//--------------------------------------------------------
void DetectorChannel::clear()
{
	DEBUG_STREAM << "DetectorChannel::Clear()  - " << device_name << endl;
	/*----- PROTECTED REGION ID(DetectorChannel::clear) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::clear
}
//--------------------------------------------------------
/**
 *	Command Resume related method
 *	Description: Resumes a stopped data aquisition.
 *
 */
//--------------------------------------------------------
void DetectorChannel::resume()
{
	DEBUG_STREAM << "DetectorChannel::Resume()  - " << device_name << endl;
	/*----- PROTECTED REGION ID(DetectorChannel::resume) ENABLED START -----*/

	//	Add your own code
	start();

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::resume
}
//--------------------------------------------------------
/**
 *	Command Prepare related method
 *	Description: Prepares the acquisition, so that a Start command can start it immediately.
 *
 */
//--------------------------------------------------------
void DetectorChannel::prepare()
{
	DEBUG_STREAM << "DetectorChannel::Prepare()  - " << device_name << endl;
	/*----- PROTECTED REGION ID(DetectorChannel::prepare) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::prepare
}
//--------------------------------------------------------
/**
 *	Method      : DetectorChannel::add_dynamic_commands()
 *	Description : Create the dynamic commands if any
 *                for specified device.
 */
//--------------------------------------------------------
void DetectorChannel::add_dynamic_commands()
{
	/*----- PROTECTED REGION ID(DetectorChannel::add_dynamic_commands) ENABLED START -----*/

	//	Add your own code to create and add dynamic commands if any

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::add_dynamic_commands
}

/*----- PROTECTED REGION ID(DetectorChannel::namespace_ending) ENABLED START -----*/

//	Additional Methods
bool DetectorChannel::isMaster(void)
{
	return false;
}

Tango::DevState DetectorChannel::dev_state()
{
	Tango::DevState tmp(Tango::ON);
	switch (m_interface->status() | m_started)
	{
		case 1 :
			tmp = ::Tango::MOVING;
			break;
		case 0 :
		default:
			break;
	}
	set_state(tmp);
	return tmp;
}

Tango::DevVarStringArray *DetectorChannel::get_properties()
{
	Tango::DevVarStringArray *argout;
	DEBUG_STREAM << "ImageChannel::GetProperties()  - " << device_name << endl;
	/*----- PROTECTED REGION ID(DetectorChannel::get_properties) ENABLED START -----*/

	argout = MLZDevice::get_properties();
	int i = argout->length();
	argout->length(i + 8 * 2);

	Tango::DbDatum tmpData("tmp");
	std::string tmpString;

	(*argout)[i] = CORBA::string_dup("writelistmode");
	tmpData << writelistmode;
	tmpData >> tmpString;
	(*argout)[i + 1] = CORBA::string_dup(tmpString.c_str());

	(*argout)[i + 2] = CORBA::string_dup("writehistogram");
	tmpData << writehistogram;
	tmpData >> tmpString;
	(*argout)[i + 3] = CORBA::string_dup(tmpString.c_str());

	(*argout)[i + 4] = CORBA::string_dup("configfile");
	configfile = m_interface->getConfigurationFileName().toStdString();
	(*argout)[i + 5] = CORBA::string_dup(configfile.c_str());

	(*argout)[i + 6] = CORBA::string_dup("lastlistfile");
	(*argout)[i + 7] = CORBA::string_dup(lastlistfile.c_str());

	(*argout)[i + 8] = CORBA::string_dup("lasthistfile");
	(*argout)[i + 9] = CORBA::string_dup(lasthistfile.c_str());

	(*argout)[i + 10] = CORBA::string_dup("lastbinnedfile");
	(*argout)[i + 11] = CORBA::string_dup(lastbinnedfile.c_str());

	(*argout)[i + 12] = CORBA::string_dup("runid");
	runid = m_interface->getRunID();
	tmpData << runid;
	tmpData >> tmpString;
	(*argout)[i + 13] = CORBA::string_dup(tmpString.c_str());

	(*argout)[i + 14] = CORBA::string_dup("calibrationfile");
	calibrationfile = m_interface->getCalibrationFileName().toStdString();
	(*argout)[i + 15] = CORBA::string_dup(calibrationfile.c_str());

	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::get_properties
	return argout;
}

Tango::DevBoolean DetectorChannel::update_properties(const Tango::DevVarStringArray *argin)
{
	for (unsigned int i = 0; i < argin->length(); i += 2)
	{
		std::string propertyName((*argin)[i]);
		std::string arg((*argin)[i + 1]);
		Tango::DbDatum data(propertyName);
		data << arg;
	/*----- PROTECTED REGION ID(DetectorChannel::update_properties) ENABLED START -----*/
		if (propertyName == "writelistmode")
		{
			data >> writelistmode;
			if (isMaster())
				m_interface->setListMode(writelistmode, true);
		}
		else if (propertyName == "writehistogram")
		{
			data >> writehistogram;
			if (isMaster())
				m_interface->setHistogramMode(writehistogram);
		}
		else if (propertyName == "configfile")
		{
			std::string val;
			data >> val;
			if (val.empty())
				::Tango::ApiDataExcept::throw_exception("Value error",
									"configuration file name must not be empty",
									"CounterChannel::update_properties()");
			m_interface->loadConfigurationFile(val.c_str());
			configfile = val;
		}
		else if (propertyName == "lastlistfile")
		{
			data >> lastlistfile;
			if (lastlistfile.empty())
				lastlistfile = "tangolistfile00000.mdat";
			m_interface->setListFileName(lastlistfile.c_str());
		}
		else if (propertyName == "lasthistfile" && propertyName == "lastbinnedfile")
		{
			data >> lasthistfile;
			if (lasthistfile.empty())
				lasthistfile = "tangohistfile00000.mdat";
			m_interface->setHistogramFileName(lasthistfile.c_str());
			lastbinnedfile= lasthistfile;
		}
		else if (propertyName == "runid")
		{
			Tango::DevULong val;
			data >> val;
			m_interface->setRunID(val);
		}
		else if (propertyName == "calibrationfile")
		{
			std::string val;
			data >> val;
			if (val.empty())
				::Tango::ApiDataExcept::throw_exception("Value error",
									"calibration file name must not be empty",
									"CounterChannel::update_properties()");
			m_interface->loadCalibrationFile(val.c_str());
			calibrationfile = val;
		}
	/*----- PROTECTED REGION END -----*/	//	DetectorChannel::update_properties
	}
	return MLZDevice::update_properties(argin);
}

/*----- PROTECTED REGION END -----*/	//	DetectorChannel::namespace_ending
} //	namespace

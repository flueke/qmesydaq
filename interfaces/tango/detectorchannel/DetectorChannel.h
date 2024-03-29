/*----- PROTECTED REGION ID(DetectorChannel.h) ENABLED START -----*/
//=============================================================================
//
// file :        DetectorChannel.h
//
// description : Include file for the DetectorChannel class
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


#ifndef DetectorChannel_H
#define DetectorChannel_H

#include <tango.h>
#include <MLZDevice.h>

class QMesyDAQDetectorInterface;

/*----- PROTECTED REGION END -----*/	//	DetectorChannel.h

/**
 *  DetectorChannel class description:
 *    Abstract base class for a detector channel.
 */

namespace DetectorChannel_ns
{
/*----- PROTECTED REGION ID(DetectorChannel::Additional Class Declarations) ENABLED START -----*/

//	Additional Class Declarations

/*----- PROTECTED REGION END -----*/	//	DetectorChannel::Additional Class Declarations

class DetectorChannel : public MLZDevice_ns::MLZDevice
{

/*----- PROTECTED REGION ID(DetectorChannel::Data Members) ENABLED START -----*/

//	Add your own data members
protected:

	QMesyDAQDetectorInterface	*m_interface;

private:
	bool	m_started;

/*----- PROTECTED REGION END -----*/	//	DetectorChannel::Data Members

//	Device property data members
public:
	//	writelistmode:	Write a list mode file if true
	Tango::DevBoolean	writelistmode;
	//	writehistogram:	Write a histogram file if true
	Tango::DevBoolean	writehistogram;
	//	configfile:	Name of the used configuration file
	std::string	configfile;
	//	calibrationfile:	Name of the used calibration file
	std::string	calibrationfile;
	//	runid:	Number of the run
	Tango::DevULong	runid;
	//	lastlistfile:	Name of the last used list mode file
	std::string	lastlistfile;
	//	lasthistfile:	Name of the last used histogram file
	std::string	lasthistfile;
	//	lastbinnedfile:	Name of the last binned data file
	std::string	lastbinnedfile;

//	Attribute data members
public:
	Tango::DevBoolean	*attr_active_read;

//	Constructors and destructors
public:
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	DetectorChannel(Tango::DeviceClass *cl,std::string &s);
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	DetectorChannel(Tango::DeviceClass *cl,const char *s);
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device name
	 *	@param d	Device description.
	 */
	DetectorChannel(Tango::DeviceClass *cl,const char *s,const char *d);
	/**
	 * The device object destructor.
	 */
	~DetectorChannel() {delete_device();};


//	Miscellaneous methods
public:
	/*
	 *	will be called at device destruction or at init command.
	 */
	void delete_device();
	/*
	 *	Initialize the device
	 */
	virtual void init_device();
	/*
	 *	Read the device properties from database
	 */
	void get_device_property();
	/*
	 *	Always executed method before execution command method.
	 */
	virtual void always_executed_hook();


//	Attribute methods
public:
	//--------------------------------------------------------
	/*
	 *	Method      : DetectorChannel::read_attr_hardware()
	 *	Description : Hardware acquisition for attributes.
	 */
	//--------------------------------------------------------
	virtual void read_attr_hardware(std::vector<long> &attr_list);
	//--------------------------------------------------------
	/*
	 *	Method      : DetectorChannel::write_attr_hardware()
	 *	Description : Hardware writing for attributes.
	 */
	//--------------------------------------------------------
	virtual void write_attr_hardware(std::vector<long> &attr_list);

/**
 *	Attribute active related methods
 *	Description: If this channel can finish the measurement when preselection is reached.
 *
 *	Data type:	Tango::DevBoolean
 *	Attr type:	Scalar
 */
	virtual void read_active(Tango::Attribute &attr);
	virtual void write_active(Tango::WAttribute &attr);
	virtual bool is_active_allowed(Tango::AttReqType type);


	//--------------------------------------------------------
	/**
	 *	Method      : DetectorChannel::add_dynamic_attributes()
	 *	Description : Add dynamic attributes if any.
	 */
	//--------------------------------------------------------
	void add_dynamic_attributes();




//	Command related methods
public:
	/**
	 *	Command Start related method
	 *	Description: Starts the acquisition.
	 *
	 */
	virtual void start();
	virtual bool is_Start_allowed(const CORBA::Any &any);
	/**
	 *	Command Stop related method
	 *	Description: Stops a running acquisition.
	 *
	 */
	virtual void stop();
	virtual bool is_Stop_allowed(const CORBA::Any &any);
	/**
	 *	Command Clear related method
	 *	Description: Clears all values of the detector.
	 *
	 */
	virtual void clear();
	virtual bool is_Clear_allowed(const CORBA::Any &any);
	/**
	 *	Command Resume related method
	 *	Description: Resumes a stopped data aquisition.
	 *
	 */
	virtual void resume();
	virtual bool is_Resume_allowed(const CORBA::Any &any);
	/**
	 *	Command Prepare related method
	 *	Description: Prepares the acquisition, so that a Start command can start it immediately.
	 *
	 */
	virtual void prepare();
	virtual bool is_Prepare_allowed(const CORBA::Any &any);


	//--------------------------------------------------------
	/**
	 *	Method      : DetectorChannel::add_dynamic_commands()
	 *	Description : Add dynamic commands if any.
	 */
	//--------------------------------------------------------
	void add_dynamic_commands();

/*----- PROTECTED REGION ID(DetectorChannel::Additional Method prototypes) ENABLED START -----*/

//	Additional Method prototypes

public:
	virtual Tango::DevState dev_state();

	virtual Tango::DevVarStringArray *get_properties();

protected:

	virtual Tango::DevBoolean update_properties(const Tango::DevVarStringArray *argin);

	virtual bool isMaster(void);

	std::string incNumber(const std::string &);

/*----- PROTECTED REGION END -----*/	//	DetectorChannel::Additional Method prototypes
};

/*----- PROTECTED REGION ID(DetectorChannel::Additional Classes Definitions) ENABLED START -----*/

//	Additional Classes Definitions

/*----- PROTECTED REGION END -----*/	//	DetectorChannel::Additional Classes Definitions

}	//	End of namespace

#endif   //	DetectorChannel_H

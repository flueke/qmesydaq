/*----- PROTECTED REGION ID(ImageChannel.h) ENABLED START -----*/
//=============================================================================
//
// file :        ImageChannel.h
//
// description : Include file for the ImageChannel class
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


#ifndef ImageChannel_H
#define ImageChannel_H

#include <tango.h>
#include <MLZDevice.h>
#include <DetectorChannel.h>


/*----- PROTECTED REGION END -----*/	//	ImageChannel.h

/**
 *  ImageChannel class description:
 *    Abstract base class for a detector channel delivering images.
 */

namespace ImageChannel_ns
{
/*----- PROTECTED REGION ID(ImageChannel::Additional Class Declarations) ENABLED START -----*/

//	Additional Class Declarations


/*----- PROTECTED REGION END -----*/	//	ImageChannel::Additional Class Declarations

class ImageChannel : public DetectorChannel_ns::DetectorChannel
{

/*----- PROTECTED REGION ID(ImageChannel::Data Members) ENABLED START -----*/

//	Add your own data members
public:
	//	histogram:	Which histogram is used. (`raw`, `mapped`, or `amplitude`)
	std::string	histogram;

private:
	Tango::DevULong	m_histo;

	bool	m_histogramIncrement;

	bool	m_listmodeIncrement;

/*----- PROTECTED REGION END -----*/	//	ImageChannel::Data Members


//	Attribute data members
public:
	Tango::DevULong64	*attr_preselection_read;
	Tango::DevULong	*attr_detectorSize_read;
	Tango::DevULong	*attr_roiOffset_read;
	Tango::DevULong	*attr_roiSize_read;
	Tango::DevULong	*attr_binning_read;
	Tango::DevULong	*attr_zeroPoint_read;
	Tango::DevULong	*attr_value_read;

//	Constructors and destructors
public:
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	ImageChannel(Tango::DeviceClass *cl,string &s);
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	ImageChannel(Tango::DeviceClass *cl,const char *s);
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device name
	 *	@param d	Device description.
	 */
	ImageChannel(Tango::DeviceClass *cl,const char *s,const char *d);
	/**
	 * The device object destructor.
	 */
	~ImageChannel() {delete_device();};


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
	 *	Method      : ImageChannel::read_attr_hardware()
	 *	Description : Hardware acquisition for attributes.
	 */
	//--------------------------------------------------------
	virtual void read_attr_hardware(vector<long> &attr_list);
	//--------------------------------------------------------
	/*
	 *	Method      : ImageChannel::write_attr_hardware()
	 *	Description : Hardware writing for attributes.
	 */
	//--------------------------------------------------------
	virtual void write_attr_hardware(vector<long> &attr_list);

/**
 *	Attribute preselection related methods
 *	Description: Preselection for the counts in the ROI, if the channel supports active mode.
 *
 *	Data type:	Tango::DevULong64
 *	Attr type:	Scalar
 */
	virtual void read_preselection(Tango::Attribute &attr);
	virtual void write_preselection(Tango::WAttribute &attr);
	virtual bool is_preselection_allowed(Tango::AttReqType type);
/**
 *	Attribute detectorSize related methods
 *	Description: Represents the real detector size in all dimensions (max 10).
 *               
 *               The dimension with the fastest running index should come first. For an image this must be the X dimension.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
	virtual void read_detectorSize(Tango::Attribute &attr);
	virtual bool is_detectorSize_allowed(Tango::AttReqType type);
/**
 *	Attribute roiOffset related methods
 *	Description: Region of interest offset in all dimensions (max 10).
 *               
 *               The dimension order is the same as for detectorSize.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
	virtual void read_roiOffset(Tango::Attribute &attr);
	virtual void write_roiOffset(Tango::WAttribute &attr);
	virtual bool is_roiOffset_allowed(Tango::AttReqType type);
/**
 *	Attribute roiSize related methods
 *	Description: Region of interest size. Extraction of the region of interest will be done before binning!
 *               
 *               The dimension order is the same as for detectorSize.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
	virtual void read_roiSize(Tango::Attribute &attr);
	virtual void write_roiSize(Tango::WAttribute &attr);
	virtual bool is_roiSize_allowed(Tango::AttReqType type);
/**
 *	Attribute binning related methods
 *	Description: Binning to be applied to the original image. Binning will be done after extracting the region of interest.
 *               
 *               The dimension order is the same as for detectorSize.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
	virtual void read_binning(Tango::Attribute &attr);
	virtual void write_binning(Tango::WAttribute &attr);
	virtual bool is_binning_allowed(Tango::AttReqType type);
/**
 *	Attribute zeroPoint related methods
 *	Description: Position of the detectors zero point.
 *               The position is relative to the lower left edge of the detector.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
	virtual void read_zeroPoint(Tango::Attribute &attr);
	virtual bool is_zeroPoint_allowed(Tango::AttReqType type);
/**
 *	Attribute value related methods
 *	Description: This attribute can be used to get the acquisition result (image) from the detector.
 *               The data shape is defined by the roiSize and binning attributes.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 16777216
 */
	virtual void read_value(Tango::Attribute &attr);
	virtual bool is_value_allowed(Tango::AttReqType type);


	//--------------------------------------------------------
	/**
	 *	Method      : ImageChannel::add_dynamic_attributes()
	 *	Description : Add dynamic attributes if any.
	 */
	//--------------------------------------------------------
	void add_dynamic_attributes();




//	Command related methods
public:
	/**
	 *	Command GetBlock related method
	 *	Description: Gets a block of data, given by offset and length.
	 *
	 *	@param argin First element: offset of first element.
	 *               Second element: number of elements to get.
	 *	@returns 
	 */
	virtual Tango::DevVarULongArray *get_block(const Tango::DevVarULongArray *argin);
	virtual bool is_GetBlock_allowed(const CORBA::Any &any);


	//--------------------------------------------------------
	/**
	 *	Method      : ImageChannel::add_dynamic_commands()
	 *	Description : Add dynamic commands if any.
	 */
	//--------------------------------------------------------
	void add_dynamic_commands();

/*----- PROTECTED REGION ID(ImageChannel::Additional Method prototypes) ENABLED START -----*/

	virtual void start();

//	Additional Method prototypes

	virtual Tango::DevVarStringArray *get_properties();

protected:
	virtual Tango::DevBoolean update_properties(const Tango::DevVarStringArray *);

	bool check_histogram_value(const std::string &);

	void set_histogram(const std::string &);

/*----- PROTECTED REGION END -----*/	//	ImageChannel::Additional Method prototypes
};

/*----- PROTECTED REGION ID(ImageChannel::Additional Classes Definitions) ENABLED START -----*/

//	Additional Classes Definitions

/*----- PROTECTED REGION END -----*/	//	ImageChannel::Additional Classes Definitions

}	//	End of namespace

#endif   //	ImageChannel_H

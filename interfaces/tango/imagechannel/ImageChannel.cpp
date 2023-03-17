/*----- PROTECTED REGION ID(ImageChannel.cpp) ENABLED START -----*/
//=============================================================================
//
// file :        ImageChannel.cpp
//
// description : C++ source for the ImageChannel class and its commands.
//               The class is derived from Device. It represents the
//               CORBA servant object which will be accessed from the
//               network. All commands which can be executed on the
//               ImageChannel are implemented in this file.
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


#include <ImageChannel.h>
#include <ImageChannelClass.h>

#include <QList>
#include "QMesydaqDetectorInterface.h"

#include <libgen.h>

#if 0
#	include "measurement.h"
#else
//! Defines the histogram type
enum HistogramType {
	PositionHistogram = 0,		//!< raw position histogram
	AmplitudeHistogram,		//!< raw amplitude histogram
	CorrectedPositionHistogram,	//!< corrected position histogram
};
#endif

/*----- PROTECTED REGION END -----*/	//	ImageChannel.cpp

/**
 *  ImageChannel class description:
 *    Abstract base class for a detector channel delivering images.
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
//  GetBlock       |  get_block
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
//  preselection  |  Tango::DevULong64	Scalar
//  detectorSize  |  Tango::DevULong	Spectrum  ( max = 10)
//  roiOffset     |  Tango::DevULong	Spectrum  ( max = 10)
//  roiSize       |  Tango::DevULong	Spectrum  ( max = 10)
//  binning       |  Tango::DevULong	Spectrum  ( max = 10)
//  zeroPoint     |  Tango::DevULong	Spectrum  ( max = 10)
//  value         |  Tango::DevULong	Spectrum  ( max = 16777216)
//================================================================

namespace ImageChannel_ns
{
/*----- PROTECTED REGION ID(ImageChannel::namespace_starting) ENABLED START -----*/

//	static initializations

/*----- PROTECTED REGION END -----*/	//	ImageChannel::namespace_starting

//--------------------------------------------------------
/**
 *	Method      : ImageChannel::ImageChannel()
 *	Description : Constructors for a Tango device
 *                implementing the classImageChannel
 */
//--------------------------------------------------------
ImageChannel::ImageChannel(Tango::DeviceClass *cl, string &s)
 : DetectorChannel(cl, s.c_str())
{
	/*----- PROTECTED REGION ID(ImageChannel::constructor_1) ENABLED START -----*/
	init_device();

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::constructor_1
}
//--------------------------------------------------------
ImageChannel::ImageChannel(Tango::DeviceClass *cl, const char *s)
 : DetectorChannel(cl, s)
{
	/*----- PROTECTED REGION ID(ImageChannel::constructor_2) ENABLED START -----*/
	init_device();

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::constructor_2
}
//--------------------------------------------------------
ImageChannel::ImageChannel(Tango::DeviceClass *cl, const char *s, const char *d)
 : DetectorChannel(cl, s, d)
{
	/*----- PROTECTED REGION ID(ImageChannel::constructor_3) ENABLED START -----*/
	init_device();

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::constructor_3
}

//--------------------------------------------------------
/**
 *	Method      : ImageChannel::delete_device()
 *	Description : will be called at device destruction or at init command
 */
//--------------------------------------------------------
void ImageChannel::delete_device()
{
	DEBUG_STREAM << "ImageChannel::delete_device() " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::delete_device) ENABLED START -----*/

	//	Delete device allocated objects

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::delete_device
	delete[] attr_preselection_read;
	delete[] attr_detectorSize_read;
	delete[] attr_roiOffset_read;
	delete[] attr_roiSize_read;
	delete[] attr_binning_read;
	delete[] attr_zeroPoint_read;
	delete[] attr_value_read;

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
 *	Method      : ImageChannel::init_device()
 *	Description : will be called at device initialization.
 */
//--------------------------------------------------------
void ImageChannel::init_device()
{
	DEBUG_STREAM << "ImageChannel::init_device() create device " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::init_device_before) ENABLED START -----*/

	//	Initialization before get_device_property() call

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::init_device_before

	if (Tango::Util::instance()->is_svr_starting() == false  &&
		Tango::Util::instance()->is_device_restarting(device_name)==false)
	{
		//	If not starting up call init device for inherited object
		DetectorChannel_ns::DetectorChannel::init_device();
	}

	//	Get the device properties from database
	get_device_property();

	attr_preselection_read = new Tango::DevULong64[1];
	attr_detectorSize_read = new Tango::DevULong[10];
	attr_roiOffset_read = new Tango::DevULong[10];
	attr_roiSize_read = new Tango::DevULong[10];
	attr_binning_read = new Tango::DevULong[10];
	attr_zeroPoint_read = new Tango::DevULong[10];
	attr_value_read = new Tango::DevULong[16777216];
	/*----- PROTECTED REGION ID(ImageChannel::init_device) ENABLED START -----*/

	//	Initialize device
	for (int i = 0; i < 10; ++i)
	{
		attr_roiOffset_read[i] = 0;
		attr_roiSize_read[i] = 1;
		attr_detectorSize_read[i] = 1;
		attr_zeroPoint_read[i] = 0;
		attr_binning_read[i] = 1;
	}
	attr_preselection_read[0] = 0;

	m_histogramIncrement = true;
	m_listmodeIncrement = true;

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::init_device
}

//--------------------------------------------------------
/**
 *	Method      : ImageChannel::get_device_property()
 *	Description : Read database to initialize property data members.
 */
//--------------------------------------------------------
void ImageChannel::get_device_property()
{
	/*----- PROTECTED REGION ID(ImageChannel::get_device_property_before) ENABLED START -----*/

	//	Initialize property data members

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::get_device_property_before


	//	Read device properties from database.
	Tango::DbData	dev_prop;
	dev_prop.push_back(Tango::DbDatum("histogram"));

	//	is there at least one property to be read ?
	if (dev_prop.size()>0)
	{
		//	Call database and extract values
		if (Tango::Util::instance()->_UseDb==true)
			get_db_device()->get_property(dev_prop);

		//	get instance on ImageChannelClass to get class property
		Tango::DbDatum	def_prop, cl_prop;
		ImageChannelClass	*ds_class =
			(static_cast<ImageChannelClass *>(get_device_class()));
		int	i = -1;

		//	Try to initialize histogram from class property
		cl_prop = ds_class->get_class_property(dev_prop[++i].name);
		if (cl_prop.is_empty()==false)	cl_prop  >>  histogram;
		else {
			//	Try to initialize histogram from default device value
			def_prop = ds_class->get_default_device_property(dev_prop[i].name);
			if (def_prop.is_empty()==false)	def_prop  >>  histogram;
		}
		//	And try to extract histogram value from database
		if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  histogram;

	}

	/*----- PROTECTED REGION ID(ImageChannel::get_device_property_after) ENABLED START -----*/

	//	Check device property data members init
	if (!check_histogram_value(histogram))
		histogram = "mapped";
#if 0
		// Don't throw an exception here which leads to an endless recursion
		::Tango::ApiDataExcept::throw_exception("Value error",
							"histogram value not in 'raw', 'mapped', or 'amplitude'",
							"CounterChannel::get_device_property()");
#endif
	set_histogram(histogram);

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::get_device_property_after
}

//--------------------------------------------------------
/**
 *	Method      : ImageChannel::always_executed_hook()
 *	Description : method always executed before any command is executed
 */
//--------------------------------------------------------
void ImageChannel::always_executed_hook()
{
	DEBUG_STREAM << "ImageChannel::always_executed_hook()  " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::always_executed_hook) ENABLED START -----*/

	//	code always executed before all requests
	DetectorChannel::always_executed_hook();

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::always_executed_hook
}

//--------------------------------------------------------
/**
 *	Method      : ImageChannel::read_attr_hardware()
 *	Description : Hardware acquisition for attributes
 */
//--------------------------------------------------------
void ImageChannel::read_attr_hardware(TANGO_UNUSED(vector<long> &attr_list))
{
	DEBUG_STREAM << "ImageChannel::read_attr_hardware(vector<long> &attr_list) entering... " << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::read_attr_hardware) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::read_attr_hardware
}
//--------------------------------------------------------
/**
 *	Method      : ImageChannel::write_attr_hardware()
 *	Description : Hardware writing for attributes
 */
//--------------------------------------------------------
void ImageChannel::write_attr_hardware(TANGO_UNUSED(vector<long> &attr_list))
{
	DEBUG_STREAM << "ImageChannel::write_attr_hardware(vector<long> &attr_list) entering... " << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::write_attr_hardware) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::write_attr_hardware
}

//--------------------------------------------------------
/**
 *	Read attribute preselection related method
 *	Description: Preselection for the counts in the ROI, if the channel supports active mode.
 *
 *	Data type:	Tango::DevULong64
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void ImageChannel::read_preselection(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ImageChannel::read_preselection(Tango::Attribute &attr) entering... " << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::read_preselection) ENABLED START -----*/
	//	Set the attribute value
	attr.set_value(attr_preselection_read);

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::read_preselection
}
//--------------------------------------------------------
/**
 *	Write attribute preselection related method
 *	Description: Preselection for the counts in the ROI, if the channel supports active mode.
 *
 *	Data type:	Tango::DevULong64
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void ImageChannel::write_preselection(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "ImageChannel::write_preselection(Tango::WAttribute &attr) entering... " << ENDLOG;
	//	Retrieve write value
	Tango::DevULong64	w_val;
	attr.get_write_value(w_val);
	/*----- PROTECTED REGION ID(ImageChannel::write_preselection) ENABLED START -----*/


	/*----- PROTECTED REGION END -----*/	//	ImageChannel::write_preselection
}
//--------------------------------------------------------
/**
 *	Read attribute detectorSize related method
 *	Description: Represents the real detector size in all dimensions (max 10).
 *
 *               The dimension with the fastest running index should come first. For an image this must be the X dimension.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
//--------------------------------------------------------
void ImageChannel::read_detectorSize(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ImageChannel::read_detectorSize(Tango::Attribute &attr) entering... " << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::read_detectorSize) ENABLED START -----*/
	//	Set the attribute value
	QSize s = m_interface->readHistogramSize(m_histo);
	attr_detectorSize_read[0] = s.width();
	attr_detectorSize_read[1] = s.height();
	attr.set_value(attr_detectorSize_read, 2);

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::read_detectorSize
}
//--------------------------------------------------------
/**
 *	Read attribute roiOffset related method
 *	Description: Region of interest offset in all dimensions (max 10).
 *
 *               The dimension order is the same as for detectorSize.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
//--------------------------------------------------------
void ImageChannel::read_roiOffset(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ImageChannel::read_roiOffset(Tango::Attribute &attr) entering... " << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::read_roiOffset) ENABLED START -----*/
	//	Set the attribute value

	attr.set_value(attr_roiOffset_read, 2);

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::read_roiOffset
}
//--------------------------------------------------------
/**
 *	Write attribute roiOffset related method
 *	Description: Region of interest offset in all dimensions (max 10).
 *
 *               The dimension order is the same as for detectorSize.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
//--------------------------------------------------------
void ImageChannel::write_roiOffset(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "ImageChannel::write_roiOffset(Tango::WAttribute &attr) entering... " << ENDLOG;
	//	Retrieve number of write values
	int	w_length = attr.get_write_value_length();

	//	Retrieve pointer on write values (Do not delete !)
	const Tango::DevULong	*w_val;
	attr.get_write_value(w_val);
	/*----- PROTECTED REGION ID(ImageChannel::write_roiOffset) ENABLED START -----*/
	for (int i = 0; i < 2 && i < w_length; ++i)
	{
#if 0
		// no ROI handling at the moment
		attr_roiOffset_read[i] = w_val[i];
#endif
	}

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::write_roiOffset
}
//--------------------------------------------------------
/**
 *	Read attribute roiSize related method
 *	Description: Region of interest size. Extraction of the region of interest will be done before binning!
 *
 *               The dimension order is the same as for detectorSize.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
//--------------------------------------------------------
void ImageChannel::read_roiSize(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ImageChannel::read_roiSize(Tango::Attribute &attr) entering... " << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::read_roiSize) ENABLED START -----*/
	//	Set the attribute value

	QSize s = m_interface->readHistogramSize(m_histo);
	attr_roiSize_read[0] = s.width();
	attr_roiSize_read[1] = s.height();
	attr.set_value(attr_roiSize_read, 2);
	// attr.set_value(attr_roiSize_read, 10);

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::read_roiSize
}
//--------------------------------------------------------
/**
 *	Write attribute roiSize related method
 *	Description: Region of interest size. Extraction of the region of interest will be done before binning!
 *
 *               The dimension order is the same as for detectorSize.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
//--------------------------------------------------------
void ImageChannel::write_roiSize(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "ImageChannel::write_roiSize(Tango::WAttribute &attr) entering... " << ENDLOG;
	//	Retrieve number of write values
	int	w_length = attr.get_write_value_length();

	//	Retrieve pointer on write values (Do not delete !)
	const Tango::DevULong	*w_val;
	attr.get_write_value(w_val);
	/*----- PROTECTED REGION ID(ImageChannel::write_roiSize) ENABLED START -----*/


	/*----- PROTECTED REGION END -----*/	//	ImageChannel::write_roiSize
}
//--------------------------------------------------------
/**
 *	Read attribute binning related method
 *	Description: Binning to be applied to the original image. Binning will be done after extracting the region of interest.
 *
 *               The dimension order is the same as for detectorSize.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
//--------------------------------------------------------
void ImageChannel::read_binning(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ImageChannel::read_binning(Tango::Attribute &attr) entering... " << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::read_binning) ENABLED START -----*/
	//	Set the attribute value

	attr.set_value(attr_binning_read, 2);

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::read_binning
}
//--------------------------------------------------------
/**
 *	Write attribute binning related method
 *	Description: Binning to be applied to the original image. Binning will be done after extracting the region of interest.
 *
 *               The dimension order is the same as for detectorSize.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
//--------------------------------------------------------
void ImageChannel::write_binning(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "ImageChannel::write_binning(Tango::WAttribute &attr) entering... " << ENDLOG;
	//	Retrieve number of write values
	int	w_length = attr.get_write_value_length();

	//	Retrieve pointer on write values (Do not delete !)
	const Tango::DevULong	*w_val;
	attr.get_write_value(w_val);
	/*----- PROTECTED REGION ID(ImageChannel::write_binning) ENABLED START -----*/
	for (int i = 0; i < 2 && i < w_length; ++i)
	{
		// No binning at the moment
#if 0
		attr_binning_read[i] = w_val[i];
#endif
	}

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::write_binning
}
//--------------------------------------------------------
/**
 *	Read attribute zeroPoint related method
 *	Description: Position of the detectors zero point.
 *               The position is relative to the lower left edge of the detector.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 10
 */
//--------------------------------------------------------
void ImageChannel::read_zeroPoint(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ImageChannel::read_zeroPoint(Tango::Attribute &attr) entering... " << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::read_zeroPoint) ENABLED START -----*/
	//	Set the attribute value
	attr.set_value(attr_zeroPoint_read, 2);

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::read_zeroPoint
}
//--------------------------------------------------------
/**
 *	Read attribute value related method
 *	Description: This attribute can be used to get the acquisition result (image) from the detector.
 *               The data shape is defined by the roiSize and binning attributes.
 *
 *	Data type:	Tango::DevULong
 *	Attr type:	Spectrum max = 16777216
 */
//--------------------------------------------------------
void ImageChannel::read_value(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ImageChannel::read_value(Tango::Attribute &attr) entering... " << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::read_value) ENABLED START -----*/
	//	Set the attribute value

	QSize s = m_interface->readHistogramSize(m_histo);
	QList<quint64> tmpList = m_interface->readHistogram(m_histo);

	// Flipping back the data got from Mesydaq interface
	std::vector<Tango::DevULong> tmp1;
	for (int i = s.height(); i > 0; --i)
	{
		int idx = (i - 1) * s.width();
		for (int j = 0; j < s.width(); ++j, ++idx)
			tmp1.push_back(Tango::DevULong(tmpList[idx]));
	}
	int idx(0);
	for (std::vector<Tango::DevULong>::const_iterator it = tmp1.begin(); it != tmp1.end(); ++it, ++idx)
		attr_value_read[idx] = Tango::DevULong(*it);
	attr.set_value(attr_value_read, tmpList.length()); // max: 16777216 = 4096 x 4096

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::read_value
}

//--------------------------------------------------------
/**
 *	Method      : ImageChannel::add_dynamic_attributes()
 *	Description : Create the dynamic attributes if any
 *                for specified device.
 */
//--------------------------------------------------------
void ImageChannel::add_dynamic_attributes()
{
	/*----- PROTECTED REGION ID(ImageChannel::add_dynamic_attributes) ENABLED START -----*/

	//	Add your own code to create and add dynamic attributes if any

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::add_dynamic_attributes
}

//--------------------------------------------------------
/**
 *	Command GetBlock related method
 *	Description: Gets a block of data, given by offset and length.
 *
 *	@param argin First element: offset of first element.
 *               Second element: number of elements to get.
 *	@returns
 */
//--------------------------------------------------------
Tango::DevVarULongArray *ImageChannel::get_block(const Tango::DevVarULongArray *argin)
{
	Tango::DevVarULongArray *argout;
	DEBUG_STREAM << "ImageChannel::GetBlock()  - " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::get_block) ENABLED START -----*/

	//	Add your own code
	if (argin->length() != 2)
	{
	}
	argout = new Tango::DevVarULongArray(0);

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::get_block
	return argout;
}
//--------------------------------------------------------
/**
 *	Method      : ImageChannel::add_dynamic_commands()
 *	Description : Create the dynamic commands if any
 *                for specified device.
 */
//--------------------------------------------------------
void ImageChannel::add_dynamic_commands()
{
	/*----- PROTECTED REGION ID(ImageChannel::add_dynamic_commands) ENABLED START -----*/

	//	Add your own code to create and add dynamic commands if any

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::add_dynamic_commands
}

/*----- PROTECTED REGION ID(ImageChannel::namespace_ending) ENABLED START -----*/

//	Additional Methods
//--------------------------------------------------------
/**
 *	Command GetProperties related method
 *	Description: Returns a string list of properties and their values, in the form [prop1, val1, prop2, val2, ...].
 *
 *	@returns
 */
//--------------------------------------------------------
Tango::DevVarStringArray *ImageChannel::get_properties()
{
	Tango::DevVarStringArray *argout;
	DEBUG_STREAM << "ImageChannel::GetProperties()  - " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(ImageChannel::get_properties) ENABLED START -----*/

	argout = DetectorChannel::get_properties();
	int i = argout->length();
	argout->length(i + 2);

	(*argout)[i] = CORBA::string_dup("histogram");
	(*argout)[i + 1] = CORBA::string_dup(histogram.c_str());

	/*----- PROTECTED REGION END -----*/	//	ImageChannel::get_properties
	return argout;
}

Tango::DevBoolean ImageChannel::update_properties(const Tango::DevVarStringArray *argin)
{
	for (unsigned int i = 0; i < argin->length(); i += 2)
	{
		std::string propertyName((*argin)[i]);
		std::string arg((*argin)[i + 1]);
		Tango::DbDatum data(propertyName);
		data << arg;
	/*----- PROTECTED REGION ID(ImageChannel::update_properties) ENABLED START -----*/
		if (propertyName == "histogram")
		{
			std::string val;
			data >> val;
			if (!check_histogram_value(val))
				::Tango::ApiDataExcept::throw_exception("Value error",
									"histogram value not in 'raw', 'mapped', or 'amplitude'",
									"CounterChannel::update_properties()");
			set_histogram(val);
		}
		else if (propertyName == "writelistmode")
		{
			data >> writelistmode;
			m_interface->setListMode(writelistmode, true);
			m_listmodeIncrement = false;
		}
		else if (propertyName == "writehistogram")
		{
			data >> writehistogram;
			m_interface->setHistogramMode(writehistogram);
			m_histogramIncrement = false;
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
			m_listmodeIncrement = false;
		}
		else if (propertyName == "lasthistfile" || propertyName == "lastbinnedfile")
		{
			data >> lasthistfile;
			if (lasthistfile.empty())
				lasthistfile = "tangohistfile00000.mtxt";
			m_interface->setHistogramFileName(lasthistfile.c_str());
			lastbinnedfile = lasthistfile;
			m_histogramIncrement = false;
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
	/*----- PROTECTED REGION END -----*/	//	ImageChannel::update_properties
	}
	return DetectorChannel::update_properties(argin);
}

//--------------------------------------------------------
/**
 *	Command Start related method
 *	Description: Starts the acquisition.
 *
 */
//--------------------------------------------------------
void ImageChannel::start()
{
	DEBUG_STREAM << "DetectorChannel::Start()  - " << device_name << ENDLOG;
	/*----- PROTECTED REGION ID(DetectorChannel::start) ENABLED START -----*/
	//	Add your own code
	if (!m_interface)
		::Tango::Except::throw_exception("Runtime error",
						 "Control interface not initialized",
						 "ImageChannel::start()");

	if (dev_state() != ::Tango::MOVING)
	{
		m_interface->setHistogramMode(writehistogram);
		if (writehistogram)
		{
			if (m_histogramIncrement)
				lastbinnedfile = lasthistfile = incNumber(lasthistfile);
			m_interface->setHistogramFileName(lasthistfile.c_str());
		}
		m_interface->setListMode(writelistmode, true);
		if (writelistmode)
		{
			if (m_listmodeIncrement)
				lastlistfile = incNumber(lastlistfile);
			m_interface->setListFileName(lastlistfile.c_str());
		}
	}
	m_histogramIncrement = true;
	m_listmodeIncrement = true;
	/*----- PROTECTED REGION END -----*/	//	ImageChannel::start
	DetectorChannel::start();
}

bool ImageChannel::check_histogram_value(const std::string &val)
{
	return val == "raw" || val == "mapped" || val == "amplitude";
}

void ImageChannel::set_histogram(const std::string &val)
{
	if (val == "raw")
		m_histo = PositionHistogram;
	else if (val == "mapped")
		m_histo = CorrectedPositionHistogram;
	else if (val == "amplitude")
		m_histo = AmplitudeHistogram;
	histogram = val;
}

/*----- PROTECTED REGION END -----*/	//	ImageChannel::namespace_ending
} //	namespace

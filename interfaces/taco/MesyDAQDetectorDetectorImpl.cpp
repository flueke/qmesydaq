// Interface to the QMesyDAQ software
// Copyright (C) 2009-2015 Jens Kr�ger

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

// TACODEVEL CODEGEN INCLUDES BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <TACOConverters.h>
#include <TACOCommands.h>

#include "MesyDAQDetectorDetector.h"

#include <Admin.h>
#include <iostream>

// TACODEVEL CODEGEN INCLUDES END

#include <TACOStringConverters.h>

#include "QMesydaqDetectorInterface.h"

#include "libgen.h"

DevVoid MesyDAQ::Detector::Detector::start() throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Detector::Detector::start()" << ENDLOG;
	try
	{
		if (!m_interface)
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
#if 1
		m_listFilename = incNumber(m_listFilename);
#else
		m_listFilename = runNumber(m_listFilename);
#endif
		updateResource<std::string>("lastlistfile", m_listFilename);
		m_interface->setListFileName(m_listFilename.c_str());
		m_interface->setListMode(m_writeListmode, true);
#if 1
		m_histFilename = incNumber(m_histFilename);
#else
		m_histFilename = runNumber(m_histFilename);
#endif
		updateResource<std::string>("lasthistfile", m_histFilename);
		m_interface->setHistogramFileName(m_histFilename.c_str());
		m_interface->setHistogramMode(m_writeHistogram);
		INFO_STREAM << "interface::start()" << ENDLOG;
		m_interface->start();
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::Detector::Detector::start()");
	}
}

DevVoid MesyDAQ::Detector::Detector::stop() throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Detector::Detector::stop()" << ENDLOG;
	try
	{
		if (!m_interface)
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		m_interface->stop();
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::Detector::Detector::stop()");
	}
}

DevVoid MesyDAQ::Detector::Detector::resume() throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Detector::Detector::resume()" << ENDLOG;
	try
	{
		if (!m_interface)
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		m_interface->resume();
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::Detector::Detector::resume()");
	}
}

DevVoid MesyDAQ::Detector::Detector::clear() throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Detector::Detector::clear()" << ENDLOG;
	try
	{
		if (!m_interface)
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		m_interface->clear();
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::Detector::Detector::clear()");
	}
}

DevVoid MesyDAQ::Detector::Detector::setPreselection(const DevDouble input) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Detector::Detector::setPrelection(" << input << ")" << ENDLOG;
	try
	{
		if (!m_interface)
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		m_interface->setPreSelection(input);
#warning TODO preselection values
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::Detector::Detector::setPreselection()");
	}
}

DevDouble MesyDAQ::Detector::Detector::preselection() throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Detector::Detector::preselection()" << ENDLOG;
	static DevDouble retVal(0.0);
	try
	{
		if (!m_interface)
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		retVal = m_interface->preSelection();
#warning TODO preselection values
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::Detector::Detector::preselection()");
	}
	return retVal;
}

const std::vector<DevULong> &MesyDAQ::Detector::Detector::read() throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Detector::Detector::read()" << ENDLOG;
	static std::vector<DevULong> tmp;
	tmp.clear();
	try
	{
		if (!m_interface)
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");

		QList<quint64> tmpList;
// complete histogram
		if (true)
		{
			INFO_STREAM << "MesyDAQ::Detector::Detector::read(" << m_histo << ")" << ENDLOG;
			QSize s = m_interface->readHistogramSize(m_histo);
			tmpList = m_interface->readHistogram(m_histo);

			tmp.push_back(s.width());
			tmp.push_back(s.height());
			tmp.push_back(1);
		}
// spectrogram
		else
		{
// 1 1 1 value
			tmpList = m_interface->readDiffractogram();
			tmp.push_back(tmpList.count());
			tmp.push_back(1);
			tmp.push_back(1);
		}
		for (QList<quint64>::const_iterator it = tmpList.begin(); it != tmpList.end(); ++it)
			tmp.push_back(quint32(*it));
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::Detector::Detector::read()");
	}
	return tmp;
}

void MesyDAQ::Detector::Detector::deviceInit(void) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Detector::Detector::deviceInit()" << ENDLOG;
	// Please implement this for the startup
	try
	{
		Base::deviceInit();
//		Server::deviceUpdate("histogram");
//		Server::deviceUpdate("lastbinnedfile");
//		Server::deviceUpdate("runid");
//		Server::deviceUpdate("histogram");
//		Server::deviceUpdate("configfile");
		m_interface->init();
		setDeviceState(::TACO::State::DEVICE_NORMAL);
	}
	catch (const ::TACO::Exception &e)
	{
		setDeviceState(::TACO::State::FAULT);
		throw;
	}
}

DevShort MesyDAQ::Detector::Detector::deviceState(void) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Detector::Detector::deviceState()" << ENDLOG;
	if (!m_interface)
		return ::TACO::State::FAULT;
	switch (m_interface->status())
	{
		case 1 :
			return ::TACO::State::COUNTING;
		default:
		case 0 :
			return ::TACO::Server::deviceState();
	}
}

void MesyDAQ::Detector::Detector::deviceUpdate(void) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Detector::Detector::deviceUpdate()" << ENDLOG;

	Base::deviceUpdate();
}

void MesyDAQ::Detector::Detector::deviceQueryResource(void) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Detector::Detector::deviceQueryResource()" << ENDLOG;
	Base::deviceQueryResource();
}

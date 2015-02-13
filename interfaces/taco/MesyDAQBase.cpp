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

#include "MesyDAQBase.h"

#include <Admin.h>
#include <iostream>

// TACODEVEL CODEGEN INCLUDES END

#include "MultipleLoopApplication.h"
#include "QMesydaqDetectorInterface.h"

	// TACODEVEL CODEGEN BASE CLASS CONSTRUCTOR CALLS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

MesyDAQ::Base::Base(const std::string& name, DevLong& error) throw (::TACO::Exception)
	: ::TACO::Server(name, error)

	// TACODEVEL CODEGEN BASE CLASS CONSTRUCTOR CALLS END

	, m_interface(NULL)
	, m_runid(0)
	, m_histo(0)
	, m_writeListmode(true)
	, m_writeHistogram(true)

	/* , MyFirstBaseClass(), MySecondBaseClass(), ... */
	// TACODEVEL CODEGEN CONSTRUCTOR CODE BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.
{
	setDeviceVersion(VERSION);

	// TACODEVEL CODEGEN CONSTRUCTOR CODE END

	addResource("lastlistfile", D_STRING_TYPE, "name of the last/currently used list mode data file");
	addResource("lasthistfile", D_STRING_TYPE, "name of the last/currently used histogram data file");
	addResource("lastbinnedfile", D_STRING_TYPE, "name of the last/currently used (binned) histogram data file");
	addResource("writelistmode", D_BOOLEAN_TYPE, "write listmode data");
	addResource("writehistogram", D_BOOLEAN_TYPE, "write histogram data");
	addResource("runid", D_ULONG_TYPE, "number of the last/currently data acquisition run");
	addResource("histogram", D_STRING_TYPE, "type of the histogram (raw|mapped|amplitude)");
	addResource("configfile", D_STRING_TYPE, "currently used configuration file");

	MultipleLoopApplication *app = dynamic_cast<MultipleLoopApplication*>(QApplication::instance());
	if (app)
		m_interface = dynamic_cast<QMesyDAQDetectorInterface*>(app->getQtInterface());
	// TACODEVEL CODEGEN CONSTRUCTOR FINISH CODE BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.
	try
	{
		// deviceInit();
		if (Server::deviceState() != ::TACO::State::INIT)
			setDeviceState(::TACO::State::DEVICE_NORMAL);
		logStream->noticeStream() << GetClassName() << " : " << deviceName() << " : init complete." << log4cpp::eol;
	}
	catch (const ::TACO::Exception &e)
	{
		logStream->fatalStream() << GetClassName() << " : " << deviceName() << " : init failed. " << e.what() << log4cpp::eol;
		Server::setDeviceState(DEVON_NOT_REACHED);
	}
	// TACODEVEL CODEGEN CONSTRUCTOR FINISH CODE END
}

MesyDAQ::Base::~Base() throw ()
{
	// VOID
}

// TACODEVEL CODEGEN TACO METHOD DEFINITIONS BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.
// TACODEVEL CODEGEN TACO METHOD DEFINITIONS END

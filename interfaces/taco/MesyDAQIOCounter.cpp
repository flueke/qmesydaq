// Interface to the QMesyDAQ software
// Copyright (C) 2012-2016 Jens Krüger

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

#include "MesyDAQIOCounter.h"

#include <Admin.h>
#include <iostream>

// TACODEVEL CODEGEN INCLUDES END

#include "MultipleLoopApplication.h"

#include "TACOQMesydaqInterface.h"

	// TACODEVEL CODEGEN BASE CLASS CONSTRUCTOR CALLS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

MesyDAQ::IO::Counter::Counter(const std::string& name, DevLong& error) throw (::TACO::Exception)
	: Base(name, error)

	// TACODEVEL CODEGEN BASE CLASS CONSTRUCTOR CALLS END

	, m_channel(0)

	/* , MyFirstBaseClass(), MySecondBaseClass(), ... */

	// TACODEVEL CODEGEN CONSTRUCTOR CODE BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.
{
	addDeviceType(::IO::COUNTER_ID);
	addCommand(::TACO::Command::SET_MODE, &tacoSetMode, D_LONG_TYPE, D_VOID_TYPE, WRITE_ACCESS);
	addCommand(::TACO::Command::MODE, &tacoMode, D_VOID_TYPE, D_LONG_TYPE, READ_ACCESS);
	addCommand(::TACO::Command::TIME_BASE, &tacoTimeBase, D_VOID_TYPE, D_DOUBLE_TYPE, READ_ACCESS);
	addCommand(::TACO::Command::SET_TIME_BASE, &tacoSetTimeBase, D_DOUBLE_TYPE, D_VOID_TYPE, WRITE_ACCESS);
	addCommand(::TACO::Command::ENABLE_MASTER, &tacoEnableMaster, D_BOOLEAN_TYPE, D_VOID_TYPE, WRITE_ACCESS);
	addCommand(::TACO::Command::IS_MASTER, &tacoIsMaster, D_VOID_TYPE, D_BOOLEAN_TYPE, READ_ACCESS);
	addCommand(::TACO::Command::READ_U_LONG, &tacoRead, D_VOID_TYPE, D_ULONG_TYPE, READ_ACCESS);
	addCommand(::TACO::Command::PRESELECTION_ULONG, &tacoPreselection, D_VOID_TYPE, D_ULONG_TYPE, READ_ACCESS);
	addCommand(::TACO::Command::SET_PRESELECTION_ULONG, &tacoSetPreselection, D_ULONG_TYPE, D_VOID_TYPE, WRITE_ACCESS);

	setDeviceVersion(VERSION);

	// TACODEVEL CODEGEN CONSTRUCTOR CODE END

	addResource("channel", D_ULONG_TYPE, "Channel number of the counter (100 for events)");

	// TACODEVEL CODEGEN CONSTRUCTOR FINISH CODE BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.
	try
	{
		deviceInit();
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

MesyDAQ::IO::Counter::~Counter() throw ()
{
	// VOID
}

// TACODEVEL CODEGEN TACO METHOD DEFINITIONS BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

void MesyDAQ::IO::Counter::tacoSetMode(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception)
{
	Counter* s = dynamic_cast<Counter*>(server);
	if (s == 0)
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	s->setMode(::TACO::convert(static_cast<DevLong*>(argin)));
}

void MesyDAQ::IO::Counter::tacoMode(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception)
{
	Counter* s = dynamic_cast<Counter*>(server);
	if (s == 0)
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	::TACO::assign(static_cast<DevLong*>(argout), s->mode());
}

void MesyDAQ::IO::Counter::tacoTimeBase(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception)
{
	Counter* s = dynamic_cast<Counter*>(server);
	if (s == 0)
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	::TACO::assign(static_cast<DevDouble*>(argout), s->timeBase());
}

void MesyDAQ::IO::Counter::tacoSetTimeBase(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception)
{
	Counter* s = dynamic_cast<Counter*>(server);
	if (s == 0)
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	s->setTimeBase(::TACO::convert(static_cast<DevDouble*>(argin)));
}

void MesyDAQ::IO::Counter::tacoEnableMaster(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception)
{
	Counter* s = dynamic_cast<Counter*>(server);
	if (s == 0)
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	s->enableMaster(::TACO::convert(static_cast<DevBoolean*>(argin)));
}

void MesyDAQ::IO::Counter::tacoIsMaster(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception)
{
	Counter* s = dynamic_cast<Counter*>(server);
	if (s == 0)
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	::TACO::assign(static_cast<DevBoolean*>(argout), s->isMaster());
}

void MesyDAQ::IO::Counter::tacoRead(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception)
{
	Counter* s = dynamic_cast<Counter*>(server);
	if (s == 0)
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	::TACO::assign(static_cast<DevULong*>(argout), s->read());
}

void MesyDAQ::IO::Counter::tacoPreselection(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception)
{
	Counter* s = dynamic_cast<Counter*>(server);
	if (s == 0)
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	::TACO::assign(static_cast<DevULong*>(argout), s->preselection());
}

void MesyDAQ::IO::Counter::tacoSetPreselection(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception)
{
	Counter* s = dynamic_cast<Counter*>(server);
	if (s == 0)
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	s->setPreselection(::TACO::convert(static_cast<DevULong*>(argin)));
}

// TACODEVEL CODEGEN TACO METHOD DEFINITIONS END

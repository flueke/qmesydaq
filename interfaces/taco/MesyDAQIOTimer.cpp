// Interface to the QMesyDAQ software
// Copyright (C) 2012-2013 Jens Krüger

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

#include "MesyDAQIOTimer.h"

#include <Admin.h>
#include <iostream>

// TACODEVEL CODEGEN INCLUDES END

#include "MultipleLoopApplication.h"
#include "QMesydaqDetectorInterface.h"

	// TACODEVEL CODEGEN BASE CLASS CONSTRUCTOR CALLS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

MesyDAQ::IO::Timer::Timer( const std::string& name, DevLong& error) throw (::TACO::Exception)
	: ::TACO::Server( name, error)

	// TACODEVEL CODEGEN BASE CLASS CONSTRUCTOR CALLS END

        , m_interface(NULL)

	/* , MyFirstBaseClass(), MySecondBaseClass(), ... */
{
	// TACODEVEL CODEGEN CONSTRUCTOR CODE BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

	addDeviceType(::IO::TIMER_ID);
	addCommand(::TACO::Command::START, &tacoStart, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS);
	addCommand(::TACO::Command::STOP, &tacoStop, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS);
	addCommand(::TACO::Command::SET_PRESELECTION_DOUBLE, &tacoSetPreselection, D_DOUBLE_TYPE, D_VOID_TYPE, WRITE_ACCESS);
	addCommand(::TACO::Command::RESUME, &tacoResume, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS);
	addCommand(::TACO::Command::CLEAR, &tacoClear, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS);
	addCommand(::TACO::Command::PRESELECTION_DOUBLE, &tacoPreselection, D_VOID_TYPE, D_DOUBLE_TYPE, READ_ACCESS);
	addCommand(::TACO::Command::SET_MODE, &tacoSetMode, D_LONG_TYPE, D_VOID_TYPE, WRITE_ACCESS);
	addCommand(::TACO::Command::MODE, &tacoMode, D_VOID_TYPE, D_LONG_TYPE, READ_ACCESS);
	addCommand(::TACO::Command::ENABLE_MASTER, &tacoEnableMaster, D_BOOLEAN_TYPE, D_VOID_TYPE, WRITE_ACCESS);
	addCommand(::TACO::Command::IS_MASTER, &tacoIsMaster, D_VOID_TYPE, D_BOOLEAN_TYPE, READ_ACCESS);
	addCommand(::TACO::Command::READ_DOUBLE, &tacoRead, D_VOID_TYPE, D_DOUBLE_TYPE, READ_ACCESS);

	setDeviceVersion(VERSION);

	// TACODEVEL CODEGEN CONSTRUCTOR CODE END
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
        MultipleLoopApplication *app = dynamic_cast<MultipleLoopApplication*>(QApplication::instance());
        if (app)
		m_interface = dynamic_cast<QMesyDAQDetectorInterface*>(app->getQtInterface());
}

MesyDAQ::IO::Timer::~Timer() throw ()
{
	// VOID
}

// TACODEVEL CODEGEN TACO METHOD DEFINITIONS BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

void MesyDAQ::IO::Timer::tacoStart(::TACO::Server *server, DevArgument , DevArgument ) throw (::TACO::Exception)
{
	Timer* s = dynamic_cast<Timer*>( server);
	if (s != 0) {
		s->start();
	} else {
		throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void MesyDAQ::IO::Timer::tacoStop(::TACO::Server *server, DevArgument , DevArgument ) throw (::TACO::Exception)
{
	Timer* s = dynamic_cast<Timer*>( server);
	if (s != 0) {
		s->stop();
	} else {
		throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void MesyDAQ::IO::Timer::tacoSetPreselection(::TACO::Server *server, DevArgument argin, DevArgument ) throw (::TACO::Exception)
{
	Timer* s = dynamic_cast<Timer*>( server);
	if (s != 0) {
		s->setPreselection( ::TACO::convert( static_cast<DevDouble*>( argin)));
	} else {
		throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void MesyDAQ::IO::Timer::tacoResume(::TACO::Server *server, DevArgument , DevArgument ) throw (::TACO::Exception)
{
	Timer* s = dynamic_cast<Timer*>( server);
	if (s != 0) {
		s->resume();
	} else {
		throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void MesyDAQ::IO::Timer::tacoClear(::TACO::Server *server, DevArgument , DevArgument ) throw (::TACO::Exception)
{
	Timer* s = dynamic_cast<Timer*>( server);
	if (s != 0) {
		s->clear();
	} else {
		throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void MesyDAQ::IO::Timer::tacoPreselection(::TACO::Server *server, DevArgument , DevArgument argout) throw (::TACO::Exception)
{
	Timer* s = dynamic_cast<Timer*>( server);
	if (s != 0) {
		::TACO::assign( static_cast<DevDouble*>( argout), s->preselection());
	} else {
		throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void MesyDAQ::IO::Timer::tacoSetMode(::TACO::Server *server, DevArgument argin, DevArgument ) throw (::TACO::Exception)
{
	Timer* s = dynamic_cast<Timer*>( server);
	if (s != 0) {
		s->setMode( ::TACO::convert( static_cast<DevLong*>( argin)));
	} else {
		throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void MesyDAQ::IO::Timer::tacoMode(::TACO::Server *server, DevArgument , DevArgument argout) throw (::TACO::Exception)
{
	Timer* s = dynamic_cast<Timer*>( server);
	if (s != 0) {
		::TACO::assign( static_cast<DevLong*>( argout), s->mode());
	} else {
		throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void MesyDAQ::IO::Timer::tacoEnableMaster(::TACO::Server *server, DevArgument argin, DevArgument ) throw (::TACO::Exception)
{
	Timer* s = dynamic_cast<Timer*>( server);
	if (s != 0) {
		s->enableMaster( ::TACO::convert( static_cast<DevBoolean*>( argin)));
	} else {
		throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void MesyDAQ::IO::Timer::tacoIsMaster(::TACO::Server *server, DevArgument , DevArgument argout) throw (::TACO::Exception)
{
	Timer* s = dynamic_cast<Timer*>( server);
	if (s != 0) {
		::TACO::assign( static_cast<DevBoolean*>( argout), s->isMaster());
	} else {
		throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void MesyDAQ::IO::Timer::tacoRead(::TACO::Server *server, DevArgument , DevArgument argout) throw (::TACO::Exception)
{
	Timer* s = dynamic_cast<Timer*>( server);
	if (s != 0) {
		::TACO::assign( static_cast<DevDouble*>( argout), s->read());
	} else {
		throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

// TACODEVEL CODEGEN TACO METHOD DEFINITIONS END

// Interface to the QMesyDAQ software
// Copyright (C) 2012-2014 Jens Krüger

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

#ifndef MESY_D_A_Q_I_O_TIMER_H
#define MESY_D_A_Q_I_O_TIMER_H

// TACODEVEL CODEGEN INCLUDES BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <IOCommon.h>
#include <TACOServer.h>

// TACODEVEL CODEGEN INCLUDES END

#include "MesyDAQBase.h"

namespace MesyDAQ {
	namespace IO {
		class Timer;
	}
}

class MesyDAQ::IO::Timer
	// TACODEVEL CODEGEN BASE CLASSES BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

	: public Base

	// TACODEVEL CODEGEN BASE CLASSES END

	/* , MyFirstBaseClass, MySecondBaseClass, ... */
	// TACODEVEL CODEGEN METHOD DECLARATIONS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.
{
public:
	Timer(const std::string& name, DevLong& error) throw (::TACO::Exception);

	~Timer() throw ();

protected:
	virtual DevVoid setPreselection(const DevDouble input) throw (::TACO::Exception);

	virtual DevDouble preselection() throw (::TACO::Exception);

	virtual DevVoid setMode(const DevLong input) throw (::TACO::Exception);

	virtual DevLong mode() throw (::TACO::Exception);

	virtual DevVoid enableMaster(const bool input) throw (::TACO::Exception);

	virtual bool isMaster() throw (::TACO::Exception);

	virtual DevDouble read() throw (::TACO::Exception);

	virtual void deviceInit(void) throw (::TACO::Exception);

	virtual void deviceReset(void) throw (::TACO::Exception);

	virtual DevShort deviceState(void) throw (::TACO::Exception);

	virtual const char *GetClassName() {return "MesyDAQ::IO::Timer";}

	virtual const char *GetDevType() {return "IO::Timer";}

	virtual void deviceUpdate(void) throw (::TACO::Exception);

	virtual void deviceQueryResource(void) throw (::TACO::Exception);

	// TACODEVEL CODEGEN METHOD DECLARATIONS END

private:
	// TACODEVEL CODEGEN TACO METHOD DECLARATIONS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

	static void tacoSetPreselection(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception);

	static void tacoPreselection(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception);

	static void tacoSetMode(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception);

	static void tacoMode(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception);

	static void tacoEnableMaster(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception);

	static void tacoIsMaster(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception);

	static void tacoRead(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception);
	// TACODEVEL CODEGEN TACO METHOD DECLARATIONS END
};

#endif // MESY_D_A_Q_I_O_TIMER_H

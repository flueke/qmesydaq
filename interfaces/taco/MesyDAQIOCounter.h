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

#ifndef MESY_D_A_Q_I_O_COUNTER_H
#define MESY_D_A_Q_I_O_COUNTER_H

// TACODEVEL CODEGEN INCLUDES BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <IOCommon.h>
#include <TACOServer.h>

// TACODEVEL CODEGEN INCLUDES END

#if SIZEOF_UNSIGNED_LONG == 4
typedef unsigned long DevULong;
#else
typedef unsigned int DevULong;
#endif

class QMesyDAQDetectorInterface;

namespace MesyDAQ {
	namespace IO {
		class Counter;
	}
}

	// TACODEVEL CODEGEN BASE CLASSES BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

class MesyDAQ::IO::Counter
	: public ::TACO::Server

	// TACODEVEL CODEGEN BASE CLASSES END

	/* , MyFirstBaseClass, MySecondBaseClass, ... */
	// TACODEVEL CODEGEN METHOD DECLARATIONS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.
{
public:
	Counter(const std::string& name, DevLong& error) throw (::TACO::Exception);

	~Counter() throw ();

protected:
	virtual DevVoid start() throw (::TACO::Exception);

	virtual DevVoid stop() throw (::TACO::Exception);

	virtual DevVoid resume() throw (::TACO::Exception);

	virtual DevVoid clear() throw (::TACO::Exception);

	virtual DevVoid setMode(const DevLong input) throw (::TACO::Exception);

	virtual DevLong mode() throw (::TACO::Exception);

	virtual DevDouble timeBase() throw (::TACO::Exception);

	virtual DevVoid setTimeBase(const DevDouble input) throw (::TACO::Exception);

	virtual DevVoid enableMaster(const bool input) throw (::TACO::Exception);

	virtual bool isMaster() throw (::TACO::Exception);

	virtual DevULong read() throw (::TACO::Exception);

	virtual DevULong preselection() throw (::TACO::Exception);

	virtual DevVoid setPreselection(const DevULong input) throw (::TACO::Exception);

	virtual void deviceInit(void) throw (::TACO::Exception);

	virtual void deviceReset(void) throw (::TACO::Exception);

	virtual DevShort deviceState(void) throw (::TACO::Exception);

	virtual const char *GetClassName() {return "MesyDAQ::IO::Counter";}

	virtual const char *GetDevType() {return "IO::Counter";}

	virtual void deviceUpdate(void) throw (::TACO::Exception);

	virtual void deviceQueryResource(void) throw (::TACO::Exception);

	// TACODEVEL CODEGEN METHOD DECLARATIONS END

private:
	// TACODEVEL CODEGEN TACO METHOD DECLARATIONS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

	static void tacoStart(::TACO::Server *server, DevArgument, DevArgument) throw (::TACO::Exception);

	static void tacoStop(::TACO::Server *server, DevArgument, DevArgument) throw (::TACO::Exception);

	static void tacoResume(::TACO::Server *server, DevArgument, DevArgument) throw (::TACO::Exception);

	static void tacoClear(::TACO::Server *server, DevArgument, DevArgument) throw (::TACO::Exception);

	static void tacoSetMode(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception);

	static void tacoMode(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception);

	static void tacoTimeBase(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception);

	static void tacoSetTimeBase(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception);

	static void tacoEnableMaster(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception);

	static void tacoIsMaster(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception);

	static void tacoRead(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception);

	static void tacoPreselection(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception);

	static void tacoSetPreselection(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception);
	// TACODEVEL CODEGEN TACO METHOD DECLARATIONS END

	std::string incNumber(const std::string &);

private:
	QMesyDAQDetectorInterface	*m_interface;

	DevULong			m_channel;

	std::string			m_listFilename;
};

#endif // MESY_D_A_Q_I_O_COUNTER_H

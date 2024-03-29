// Interface to the QMesyDAQ software
// Copyright (C) 2012-2014 Jens Kr�ger

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

// TACODEVEL CODEGEN STARTUP INCLUDES BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4taco.h>

#if HAVE_RPC_RPC_H
#	include <rpc/rpc.h>
#endif

#if HAVE_RPC_PMAP_CLNT_H
#	include <rpc/pmap_clnt.h>
#endif
#include <string>
#include <vector>
#include <iostream>

#include <TACOExtensions.h>

#include <Admin.h>
#include <private/ApiP.h>

#include <TACOAdmin.h>

#include "MesyDAQServer.h"

// TACODEVEL CODEGEN STARTUP INCLUDES END

// TACODEVEL CODEGEN STARTUP BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

extern log4cpp::Category	*logStream;

/**
 * This function will initialise the log4cpp logging service.
 * The service will be configured via the ${LOGCONFIG} environment variable.
 * If this is not found it will use the default logging mechanism.
 * The instance will be the "taco.server" + serverName
 *
 * @param serverName the name of the server
 */
static void init_logstream(const std::string &serverName)
{
	const char *logpath = getenv("LOGCONFIG");
	std::string tmp = serverName;
	std::string::size_type pos = tmp.find('/');
	tmp[pos] = '.';
	try
	{
		if (!logpath)
			throw 0;
		log4cpp::PropertyConfigurator::configure(logpath);
	}
	catch (const log4cpp::ConfigureFailure &e)
	{
		std::cerr << e.what() << std::endl;
		logpath = "no";
		log4cpp::BasicConfigurator::configure();
	}
	catch (...)
	{
		logpath = "no";
		log4cpp::BasicConfigurator::configure();
	}
	logStream = &log4cpp::Category::getInstance("taco.server." + tmp);
	NOTICE_STREAM << "using " << logpath << " configuration file" << ENDLOG;
}
// list of all exported devices of the server
static std::vector< ::TACO::Server *> devices;

/**
 * The startup procedure is the first procedure called from main() when the device server starts up.
 * All toplevel devices to be created for the device server should be done in startup(). The startup
 * should make use of the database to determine which devices it should create. Initialisation of
 * devices is normally done from startup().
 *
 * This routine overwrites the startup routine provided by the TACO libraries by linking this file
 * statically to the server. Due to the mechanism of dynamically loading unresolved symbols at runtime,
 * this routine will called if the startup routine is linked statically, otherwise the device server
 * will use the startup() provided the TACO libraries.
 *
 * @param serverName	the name of the server
 * @param e		the error code in case of error
 *
 * @return DS_NOTOK in case of error, DS_OK else
 */
long startup(char *serverName, DevLong *e)
{
	init_logstream(serverName);

	NOTICE_STREAM << "startup: starting device server: " << serverName << ENDLOG;

	devices.clear();
	TACO::Server::setServerName(serverName);

// Query the list of device names for the corresponding device server from the database
	std::vector<std::string> deviceList;
	try
	{
		if (config_flags->device_no)
		{
			for (int i = 0; i < config_flags->device_no; ++i)
				deviceList.push_back(config_flags->device_list[i]);
		}
		else
			deviceList = TACO::queryDeviceList(serverName);
	}
	catch (const ::TACO::Exception& tmp)
	{
		*e = tmp;
		FATAL_STREAM << "startup: error: getting device list failed: " << tmp.what() << ENDLOG;
		return DS_NOTOK;
	}

// Allocate memory for the devices
	if (0 < MesyDAQ::DEVICE_MAX && MesyDAQ::DEVICE_MAX < deviceList.size())
	{
		FATAL_STREAM << "startup: error: too many devices" << ENDLOG;
		return DS_NOTOK;
	}

	for (unsigned int i = 0; i < deviceList.size(); ++i)
		INFO_STREAM << "startup: device: " << deviceList[i] << ENDLOG;

	unsigned int counter = 0;
// Create and export the devices
	for (unsigned int i = 0; i < deviceList.size(); ++i)
	{
		// Determine which device should be created
		std::string type;
		try
		{
			type = TACO::queryResource<std::string>(deviceList [i], "type");
		}
		catch (const ::TACO::Exception& tmp)
		{
			*e = tmp;
			INFO_STREAM << "startup: error: cannot get type for: " << deviceList[i]
				<< " : " << tmp.what() << ENDLOG;
			continue;
		}

// Create the device
		::TACO::Server *device(NULL);
		try
		{
			if (type == IO::COUNTER_ID)
				device = new MesyDAQ::IO::Counter(deviceList [i], *e);
			else if (type == IO::TIMER_ID)
				device = new MesyDAQ::IO::Timer(deviceList [i], *e);
			else if (type == Detector::DETECTOR_ID)
				device = new MesyDAQ::Detector::Detector(deviceList [i], *e);
			else
			{
				ERROR_STREAM << "startup: error: unsupported type: " << deviceList[i]
					<< " : " << type << ENDLOG;
				continue;
			}
			INFO_STREAM << "startup: created device: " << deviceList[i] << ENDLOG;
		}
		catch (const ::TACO::Exception& tmp)
		{
			*e = tmp;
			ERROR_STREAM << "startup: error: cannot create device: " << deviceList[i]
				<< " : " << tmp.what() << ENDLOG;
			continue;
		}

// Export the device
		if (dev_export (const_cast<char*>(deviceList [i].c_str()), device, e) != DS_OK)
		{
			delete device;
			ERROR_STREAM << "startup: error: cannot export device: " << deviceList[i]
				<< " : " << ::TACO::errorString(*e) << ENDLOG;
		}
		else
		{
			++counter;
			INFO_STREAM << "startup: exported device: " << deviceList[i] << ENDLOG;
			devices.push_back(device);
		}
	}

	if (counter == deviceList.size())
		NOTICE_STREAM << "startup: success" << ENDLOG;
	else if (counter != 0)
		ERROR_STREAM << "startup: some errors occurred. Not all devices exported." << ENDLOG;
	else
	{
		FATAL_STREAM << "startup: failed" << ENDLOG;
		return DS_NOTOK;
	}

	return DS_OK;
}


/**
 * Unregisters the device server from the static database and the portmapper and
 * closes open handles to database and messages services.
 */
void unregister_server (void)
{
	for (std::vector< ::TACO::Server *>::iterator it = devices.begin(); it != devices.end(); ++it)
	{
		if ((*it) == NULL)
			continue;
		NOTICE_STREAM << "delete device " << (*it)->deviceName() << ENDLOG;
		delete (*it);
		(*it) = NULL;
	}
	devices.clear();
	DevLong error = 0;
	LOCK(async_mutex);
//
// if this is a bona fida device server and it is using the database
// then unregister server from database device table
//
	if (multi_nethost[0].config_flags.device_server == True)
	{
		if (!multi_nethost[0].config_flags.no_database && (db_svc_unreg (multi_nethost[0].config_flags.server_name, &error) != DS_OK))
			ERROR_STREAM << "db_svc_unreg failed" <<  error << ENDLOG;
//
// destroy open client handles to message and database servers
//		clnt_destroy(db_info.conf->clnt);
//		clnt_destroy(msg_info.conf->clnt);
//
	}
//
// unregister synchronous version (4) of server from portmapper
//
	pmap_unset(multi_nethost[0].config_flags.prog_number, API_VERSION);
//
// unregister the asynchronous version (5) of the server from portmapper
//
	pmap_unset(multi_nethost[0].config_flags.prog_number, ASYNCH_API_VERSION);
//
//  finally unregister version (1) used by gettransient_ut()
//
	pmap_unset(multi_nethost[0].config_flags.prog_number, DEVSERVER_VERS);
//
// the server has been unregistred, so set flag to false!
// otherwise, there may be more than one attempt to unregister the server
// in multithreaded apps.
//
	multi_nethost[0].config_flags.device_server = False;
	UNLOCK(async_mutex);
//
// returning here and calling exit() later from main_signal_handler() will
// permit us to call unregister_server() from a different signal handler
// and continue to do something useful afterwards
//
	return;
}
// TACODEVEL CODEGEN STARTUP END

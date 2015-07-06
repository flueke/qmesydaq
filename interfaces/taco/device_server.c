/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File:		svc_api.c
 *
 * Project:	Device Servers with SUN-RPC
 *
 * Description:	Server side of the API.
 *
 * Author(s);	Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:	Feb 1994
 *
 * Version:	$Revision: 1.38 $
 *
 * Date:		$Date: 2009-09-25 14:35:15 $
 *
 ********************************************************************-*/
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
/*
 * Include files and Static Routine definitions
 */

/*
 * C++ version
 */
#ifndef WIN32
#ifdef HAVE_SYS_TYPES_H
#	include <sys/types.h>
#else
#	include <types.h>
#endif
#endif /* !WIN32 */

#include <API.h>
#include <private/ApiP.h>
#include <DevCmds.h>

#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <Admin.h>
#include <DevErrors.h>
#if defined WIN32
#include <rpc/Pmap_pro.h>
#include <rpc/pmap_cln.h>
#include <process.h>
/* #define NODATETIMEPICK to avoid compiler error. (I hate MicroSoft!)*/
#define NODATETIMEPICK
#include <commctrl.h>
#undef NODATETIMEPICK
#else
#include <rpc/pmap_clnt.h>
#endif  /* WIN32 */

#include "taco_utils.h"


/****************************************
 *          Globals                     *
 ****************************************/

/*
 *  Type for global state flags for the current
 *  server or client status is defined in API.h
 */

/*
 *  Configuration flags
 */

#ifdef __cplusplus
extern "C" char	*dev_error_stack;
#else
extern char	*dev_error_stack;
#endif

/*
 *  Debug flag
 */

extern long	debug_flag;



/*
 * Declare a pointer for the globale device array.
 * All exported devices are stored here.
 * Memory will be dynamically allocated during the
 * device_export().
 */
DevServerDevices	*devices = NULL;
long			max_no_of_devices = 0;

static int      udp_socket;
static int      tcp_socket;

extern long synch_svc_udp_sock;
extern long synch_svc_tcp_sock;

/*
 * debug flag
 */

extern long debug_flag;

static SVCXPRT *transp;
static SVCXPRT *transp_tcp;

extern long minimal_access;

#ifdef DARWIN
	typedef void (* MyRpcFuncPtr)();
#else
	typedef void (* MyRpcFuncPtr)(struct svc_req *, SVCXPRT *);
#endif

static MyRpcFuncPtr devserver_prog = (MyRpcFuncPtr)devserver_prog_4;

long svc_check  PT_( (DevLong *error) );
long db_check	PT_( (DevLong *error) );

#ifdef WIN32  /* WIN32 */

#include "resource.h"

#define TITLE_STR	"TACO-Device Server: "
#define	MB_ERR		(MB_OK | MB_ICONEXCLAMATION)
#define	MB_INFO		(MB_OK | MB_ICONINFORMATION)
#endif /* WIN32 */


static int error_msg(const char *msg)
{
#ifdef WIN32
	MessageBox((HWND)NULL, msg, TITLE_STR, MB_INFO);
	return (FALSE);
#else
	fprintf (stderr, msg);
	return (DS_NOTOK);
#endif
}



/**
 * @ingroup dsAPIintern
 * VXWORKS and NOMAIN routine to create a device server - device_server()
 *
 * @param server_name device server name registered in the database
 * @param pers_name   personal device server name registered in the database
 * @param m_opt       if it is TRUE use the message server for sending messages
 * @param s_opt             if TRUE call the 'startup' function of the server
 * @param nodb              if TRUE do not use the database server for the resources
 * @param pn        if different from 0 use this RPC program number
 * @param n_device    number of devices to export
 * @param device_list list of devices
 *
 * @retval DS_OK
 * @retval DS_NOTOK
 */
int device_server (char *server_name, char *pers_name, int m_opt, int s_opt, int nodb, int pn, int n_device, char** device_list)
{
	char			host_name [HOST_NAME_LENGTH],
				dsn_name [37],
				res_name[80];
	DevVarStringArray	default_access;
	db_resource		res_tab;

	long			prog_number=0,
				status;
	DevLong			error = 0;
	// int			pid = 0;
	short			i;

	if (strlen(server_name) >= DS_NAME_LENGTH)
	{
		char msg[80];
		snprintf(msg, sizeof(msg),"Filename to long : server_name <= %d char's\n", DS_NAME_LENGTH - 1);
		return error_msg(msg);
	}

	if (strlen(pers_name) >= DSPERS_NAME_LENGTH)
	{
		char msg[80];
		snprintf(msg, sizeof(msg), "Personal DS_name to long : personal_dsname <= %d char's\n", DSPERS_NAME_LENGTH - 1);
		return error_msg(msg);
	}
/*
 * unregister this program number from the portmapper - this is potentially
 * dangerous because it can unregister another running server. we assume
 * the -pn option is used by those who know what they are doing ...
 */
	if (prog_number != 0)
	{
		pmap_unset (prog_number, API_VERSION);
		pmap_unset (prog_number, ASYNCH_API_VERSION);
	}

	snprintf(dsn_name, sizeof(dsn_name), "%s/%s", server_name, pers_name);
/*
 * option nodb means run device server without database
 */
	if (nodb > 0)
	{
		config_flags->no_database = True;
		xdr_load_kernel(&error);
	}
/*
 * option -device means remaining command line arguments are device names
 */
	config_flags->device_no = n_device;
	config_flags->device_list = device_list;

#ifdef vxworks
/*
 * call rpcTaskInit() to initialise task-specific data structures
 * required by RPC (cf. VxWorks Reference manual pg. 1-203).
 * Failure to do so will result in the task crashing the first
 * time a call to is made to an RPC function
 */
	rpcTaskInit();
#endif /* vxworks */
/*
 *  get process ID, host_name
 *  and create device server network name
 */

#if 0
#if defined (WIN32)
	pid = _getpid ();
#elif !defined (vxworks)
	pid = getpid ();
#else  /* !vxworks */
	pid = taskIdSelf ();
#endif /* !vxworks */
#endif
/*
 * M. Diehl, 22.7.1999
 * We have to take care here, since hostname might be set to the FQDN thus
 * gethostname() returns "host.and.complete.domain" which may easily exceed
 * the 19 characters reserved for it!
 * There are 3 possible solutions:
 * 1) General extension of sizeof(host_name) reasonably beyond 19 characters
 * 2) Extracting the hostname from the FQDN
 * 3) Switching to an IP-String
 *
 * Here are some ideas on that:
 *
 * 1) Is obviously the best way, especially as there are several limitations
 *    to SHORT_NAME_SIZE=32 in quite a number of files. However this implies
 *    some major issues with respect to DBM and platform independence.
 * 2) Means switching to the intended behaviour and is rather easy to
 *    achieve. However this will fail, if the host (on which the device
 *    server is running e.g.) is not in our search domain - which is
 *    a disadvantage of the current implementation anyway, I believe.
 * 3) Replacing the hostname by it's IP-String-Quad (if it won't fit into
 *    19 characters) is possible without any changes at DBM code and
 *    should work in all situations. The disadvantage however is,
 *    that db_tools show up with xxx.xxx.xxx.xxx values instead of
 *    well-known hostnames, so one might have to use nslookup.
 *
 * What follows realizes suggestion 3) using some code from sec_api.c
 * with respect to different IP-retrieving for VxWorks and the rest of the
 * world. However, one has to keep in mind, that a bunch of other stuff
 * will fail, if FQDN exceeds SHORT_NAME_SIZE=32 - which is not too hard!
 */
	if( taco_gethostname(host_name, sizeof(host_name)) != 0 )
		return (DS_NOTOK);

	str_tolower(dsn_name);
	str_tolower(host_name);
	strncpy(config_flags->server_name, dsn_name, sizeof(config_flags->server_name));
	strncpy(config_flags->server_host, host_name, sizeof(config_flags->server_host));

/*
 * install signal handling for HPUX, SUN, OS9
 */
	(void) signal(SIGINT,  main_signal_handler);
	(void) signal(SIGTERM, main_signal_handler);
        (void) signal(SIGABRT, main_signal_handler);

#if defined (unix)
	(void) signal(SIGQUIT, main_signal_handler);
/*
 * SIGHUP and SIGPIPE are now caught in the main signal handler
 * which will simply return. This is needed for asynchronous
 * clients and servers to detect a server/client going down.
 *
 * andy 8may97
 */
	(void) signal(SIGHUP,  main_signal_handler);
	(void) signal(SIGPIPE, main_signal_handler);
#endif /* unix */

#if defined (WIN32)
	(void) signal(SIGBREAK,main_signal_handler);
#endif /* WIN32 */

#if ( OSK || _OSK )
	(void) signal(SIGQUIT, main_signal_handler);
#endif /* OSK || _OSK */

	if (nodb == False)
	{
/*
 *  if database required then import database server
 */
		if ( db_check (&error) < 0 )
		{
			dev_printerror_no (SEND,"db_import failed",error);
			return (DS_NOTOK);
		}
/*
 *  check wether an old server with the same name
 *  is mapped to portmap or still running
 */
		if ( svc_check(&error) < 0 )
		{
			dev_printerror_no (SEND,"svc_check()",error);
			return (DS_NOTOK);
		}
/*
 * If the security system is switched on, read the minimal
 * access right for version 3 clients from the
 * security database.
 */
		if ( config_flags->security == True )
		{
			char	res_path [80];
			default_access.length   = 0;
			default_access.sequence = NULL;

			strncpy (res_name, "default", sizeof(res_name));
			res_tab.resource_name = res_name;
			res_tab.resource_type = D_VAR_STRINGARR;
			res_tab.resource_adr  = &default_access;

			strncpy(res_path, "SEC/MINIMAL/ACC_RIGHT", sizeof(res_path));

			if (db_getresource (res_path, &res_tab, 1, &error) == DS_NOTOK)
			{
				dev_printerror_no (SEND, "db_getresource() get default security access right\n",error);
				return  (DS_NOTOK);
			}
/*
 * Transform the string array into an access right value.
 */
			if ( default_access.length > 0 )
			{
				for (i=0; i<SEC_LIST_LENGTH; i++)
				{
					if (strcmp (default_access.sequence[0], DevSec_List[i].access_name) == 0)
					{
						minimal_access = DevSec_List[i].access_right;
						break;
					}
				}
				if ( i == SEC_LIST_LENGTH )
					minimal_access = NO_ACCESS;
			}
			else
				minimal_access = NO_ACCESS;

			free_var_str_array (&default_access);
		}
	}
	else
	{
		prog_number = pn;
	}
/*
 * let portmapper choose port numbers for services
 */
	udp_socket = RPC_ANYSOCK;
	tcp_socket = RPC_ANYSOCK;

/*
 *  create server handle and register to portmap
 *
 *  register udp port
 */
	transp = svcudp_create (udp_socket);
	if (transp == NULL)
	{
		char msg[]="Cannot create udp service, exiting...\n";
		return error_msg(msg);
	}
/*
 *  make 3 tries to get transient progam number
 */
	synch_svc_udp_sock = -1;
	for (i = 0; i < 3; i++)
	{
		if (prog_number == 0)
			prog_number = gettransient(dsn_name);
		if( prog_number == 0 )
		{
			dev_printerror_no(SEND,"gettransient: no free programm nnumber\n",error);
			return (DS_NOTOK);
		}
/*
 * Write the device server identification to the global
 * configuration structure.
 */
		config_flags->prog_number = prog_number;
		config_flags->vers_number = API_VERSION;


                if (!svc_register(transp, prog_number, API_VERSION, devserver_prog, IPPROTO_UDP))
		{
			char msg[]="Unable to register server (UDP,4), retry...\n";
			return error_msg(msg);
		}
		else
		{
/*
 * keep the socket, we need it later for dev_synch();
 */
			udp_socket = transp->xp_sock;
			synch_svc_udp_sock = transp->xp_sock;
			break;
		}
	}
	if (synch_svc_udp_sock == -1)
	{
		char msg[]="Unable to register server (UDP,4), exiting...\n";
		return error_msg(msg);
	}

/*
 *  register tcp port
 */
	transp_tcp = svctcp_create(tcp_socket,0,0);
	if (transp_tcp == NULL)
	{
		char msg[]= "Cannot create tcp service, exiting...\n";
		return error_msg(msg);
	}

        if (!svc_register(transp_tcp, prog_number, API_VERSION, devserver_prog, IPPROTO_TCP))
	{
		char msg[]= "Unable to register server (TCP,4), exiting...\n";
		return error_msg(msg);
	}

/*
 * keep the socket, we need it later
 */
	tcp_socket = transp_tcp->xp_sock;
	synch_svc_tcp_sock = transp_tcp->xp_sock;
/*
 * if the process has got this far then it is a bona-fida device server set the appropiate flag
 */
	config_flags->device_server = True;
/*
 * startup message service
 */
	if (m_opt ==True)
	{
		char *display=getenv("DISPLAY");
		if(msg_import(dsn_name,host_name,prog_number,display,&error)!=DS_OK)
/*
 * we dont care
 */
			fprintf(stderr, "can not import message service\n");
	}
/*
 * Register the asynchronous rpc service so that the device server can receive asynchronous calls
 * from clients. The asynchronous calls are sent as batched tcp requests without wait. The server
 * will return the results to the client asynchronously using batched TCP.
 */
	status = asynch_rpc_register(&error);
	if (status != DS_OK)
	{
		dev_printerror_no (SEND,"failed to register asynchronus rpc",error);
	}
/*
 * DO NOT abort server, continue (without asynchronous server) ...
 * startup device server
 */
	if (s_opt == True)
	{
/*
 * Set the startup configuration flag to SERVER_STARTUP
 * during the startup phase.
 */
		config_flags->startup = SERVER_STARTUP;
		status = startup(config_flags->server_name, &error);
		if ( status != DS_OK )
		{
			dev_printerror_no (SEND,"startup failed",error);
			return (DS_NOTOK);
		}
/*
 *  if ds__svcrun() is used, the server can return from
 *  the startup function with status=1 to avoid svc_run()
 *  and to do a proper exit.
 */
		if ( status == 1 )
			return (DS_NOTOK);
		multi_nethost[0].config_flags.startup = True;
	}

#ifndef WIN32
/*
 *  set server into wait status
 */
	svc_run();

	fprintf (stderr, "svc_run returned\n");
	return DS_NOTOK;
#else   /* WIN32 */
/*
 * show up the main dialog
 */
	return TRUE;
#endif
}

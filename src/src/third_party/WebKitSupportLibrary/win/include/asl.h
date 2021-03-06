/*
 * Copyright (c) 2004 - 2007 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * "Portions Copyright (c) 2004 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.0 (the 'License').  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License."
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#ifndef __ASL_H__
#define __ASL_H__

#include <stdarg.h>
#include <stdint.h>

#include <TargetConditionals.h>

#if TARGET_OS_WIN32
#	if __cplusplus
#		define __BEGIN_DECLS extern "C" {
#		define __END_DECLS }
#	else
#		define __BEGIN_DECLS
#		define __END_DECLS
#	endif
#	if defined (ASL_STATIC)
#		define ASL_ENTRY_POINT
#	elif defined (ASL_BUILDING)
#		define ASL_ENTRY_POINT __declspec(dllexport)
#	else
#		define ASL_ENTRY_POINT __declspec(dllimport)
#	endif
#else
#	include <sys/cdefs.h>
#	define ASL_ENTRY_POINT 
#endif

typedef struct __aslclient *aslclient;
typedef struct __aslmsg *aslmsg;
typedef struct __aslresponse *aslresponse;

/*! @header
 * These routines provide an interface to the Apple System Log facility.
 * The API allows client applications to create flexible, structured messages
 * and send them to the syslogd server.  Messages received by the server are
 * saved in a data store, subject to input filtering constraints.
 * This API also permits clients to create queries and search the message
 * data store for matching messages.
 */

/*
 * NOTE FOR HeaderDoc
 *
 * These are added to allow headerdoc2html to process 
 * the prototypes of asl_log and asl_vlog correctly.
 * The "-p" option to headerdoc2html is required.
 */
#ifndef __DARWIN_LDBL_COMPAT2
/*! @parseOnly */
#define __DARWIN_LDBL_COMPAT2(a)
#endif
#ifndef __printflike
/*! @parseOnly */
#define __printflike(a,b)
#endif

/*! @defineblock Log Message Priority Levels
 * Log levels of the message.
 */
#define ASL_LEVEL_EMERG   0
#define ASL_LEVEL_ALERT   1
#define ASL_LEVEL_CRIT    2
#define ASL_LEVEL_ERR     3
#define ASL_LEVEL_WARNING 4
#define ASL_LEVEL_NOTICE  5
#define ASL_LEVEL_INFO    6
#define ASL_LEVEL_DEBUG   7
/*! @/defineblock */

/*! @defineblock Log Message Priority Level Strings
 * Strings corresponding to log levels.
 */
#define ASL_STRING_EMERG	"Emergency"
#define ASL_STRING_ALERT	"Alert"
#define ASL_STRING_CRIT		"Critical"
#define ASL_STRING_ERR		"Error"
#define ASL_STRING_WARNING  "Warning"
#define ASL_STRING_NOTICE   "Notice"
#define ASL_STRING_INFO		"Info"
#define ASL_STRING_DEBUG	"Debug"
/*! @/defineblock */

/*! @defineblock Attribute Matching
 * Attribute value comparison operations.
 */
#define ASL_QUERY_OP_CASEFOLD      0x0010
#define ASL_QUERY_OP_PREFIX		   0x0020
#define ASL_QUERY_OP_SUFFIX		   0x0040
#define ASL_QUERY_OP_SUBSTRING     0x0060
#define ASL_QUERY_OP_NUMERIC       0x0080
#define ASL_QUERY_OP_REGEX         0x0100

#define ASL_QUERY_OP_EQUAL         0x0001
#define ASL_QUERY_OP_GREATER       0x0002
#define ASL_QUERY_OP_GREATER_EQUAL 0x0003
#define ASL_QUERY_OP_LESS          0x0004
#define ASL_QUERY_OP_LESS_EQUAL    0x0005
#define ASL_QUERY_OP_NOT_EQUAL     0x0006
#define ASL_QUERY_OP_TRUE          0x0007
/*! @/defineblock */

/*! @defineblock Message Attributes
 *
 * These attributes are known by ASL, and are generally
 * associated with all log messages.
 * Additional attributes may be added as desired.
 */
#define ASL_KEY_TIME        "Time"          /* Timestamp.  Set automatically */
#define ASL_KEY_TIME_NSEC   "TimeNanoSec"   /* Nanosecond time. */
#define ASL_KEY_HOST        "Host"          /* Sender's address (set by the server). */
#define ASL_KEY_SENDER      "Sender"        /* Sender's identification string.  Default is process name. */
#define ASL_KEY_FACILITY    "Facility"      /* Sender's facility.  Default is "user". */
#define ASL_KEY_PID         "PID"           /* Sending process ID encoded as a string.  Set automatically. */
#define ASL_KEY_UID         "UID"           /* UID that sent the log message (set by the server). */
#define ASL_KEY_GID         "GID"           /* GID that sent the log message (set by the server). */
#define ASL_KEY_LEVEL       "Level"         /* Log level number encoded as a string.  See levels above. */
#define ASL_KEY_MSG         "Message"       /* Message text. */
#define ASL_KEY_READ_UID    "ReadUID"       /* User read access (-1 is any group). */
#define ASL_KEY_READ_GID    "ReadGID"       /* Group read access (-1 is any group). */
#define ASL_KEY_EXPIRE_TIME "ASLExpireTime" /* Expiration time for messages with long TTL. */
#define ASL_KEY_MSG_ID      "ASLMessageID"  /* 64-bit message ID number (set by the server). */
#define ASL_KEY_SESSION     "Session"       /* Session (set by the launchd). */
#define ASL_KEY_REF_PID     "RefPID"        /* Reference PID for messages proxied by launchd */
#define ASL_KEY_REF_PROC    "RefProc"       /* Reference process for messages proxied by launchd */
/*! @/defineblock */

/*! @defineblock aslmsg Types
 * Message type argument passed to asl_new().
 */
#define ASL_TYPE_MSG    0
#define ASL_TYPE_QUERY  1
/*! @/defineblock */

/*! @defineblock Filter Masks
 * Used in client-side filtering, which determines which
 * messages are sent by the client to the syslogd server.
 */
#define ASL_FILTER_MASK_EMERG   0x01
#define ASL_FILTER_MASK_ALERT   0x02
#define ASL_FILTER_MASK_CRIT    0x04
#define ASL_FILTER_MASK_ERR     0x08
#define ASL_FILTER_MASK_WARNING 0x10
#define ASL_FILTER_MASK_NOTICE  0x20
#define ASL_FILTER_MASK_INFO    0x40
#define ASL_FILTER_MASK_DEBUG   0x80
/*! @/defineblock */

/*! @defineblock Filter Mask Macros
 * Macros to create bitmasks for filter settings - see asl_set_filter().
 */
#define	ASL_FILTER_MASK(level) (1 << (level))
#define	ASL_FILTER_MASK_UPTO(level) ((1 << ((level) + 1)) - 1)
/*! @/defineblock */

/*! @defineblock Client Creation Options
 * Options for asl_open().
 */
#define ASL_OPT_STDERR		0x00000001
#define ASL_OPT_NO_DELAY    0x00000002
#define ASL_OPT_NO_REMOTE   0x00000004
/*! @/defineblock */

__BEGIN_DECLS

/*!
 * Initialize a connection to the ASL server.
 *
 * This call is optional in most cases.  The library will perform any
 * necessary initializations on the fly.  A call to asl_open() is required
 * if optional settings must be made before messages are sent to the server.
 * These include setting the client filter and managing additional output
 * file descriptors.  Note that the default setting of the client filter is
 * ASL_FILTER_MASK_UPTO(ASL_LEVEL_NOTICE), so ASL_LEVEL_DEBUG and ASL_LEVEL_INFO
 * messages are not sent to the server by default.
 * 
 * Options (defined above) may be set using the opts parameter. They are:
 *
 *   ASL_OPT_STDERR    - adds stderr as an output file descriptor
 *
 *   ASL_OPT_NO_DELAY  - connects to the server immediately
 *
 *   ASL_OPT_NO_REMOTE - disables the remote-control mechanism for adjusting
 *                       filter levers for processes using e.g. syslog -c ...
 *
 * @param ident
 *    (input) Sender name
 * @param facility
 *    (input) Facility name
 * @param opts
 *    (input) Options (see asl_open Options)
 * @result Returns an ASL client handle
 */
ASL_ENTRY_POINT aslclient asl_open(const char *ident, const char *facility, uint32_t opts);

/*!
 * Shuts down a connection to the server.
 *
 * @param asl
 *    (input) An ASL client handle
 */
ASL_ENTRY_POINT void asl_close(aslclient asl);

/*!
 * Write log messages to the given file descriptor.
 *
 * Log messages will be written to this file as well as to the server.
 *
 * @param asl
 *    (input) An ASL client handle
 * @param fd
 *    (input) A file descriptor
 * @result Returns 0 on success, non-zero on failure
*/
ASL_ENTRY_POINT int asl_add_log_file(aslclient asl, int fd);

/*!
 * Stop writing log messages to the given file descriptor.
 * The file descripter is not closed by this routine.
 *
 * @param asl
 *    (input) An ASL client handle
 * @param fd
 *    (input) A file descriptor
 * @result Returns 0 on success, non-zero on failure
 */
ASL_ENTRY_POINT int asl_remove_log_file(aslclient asl, int fd);

/*!
 * Set a filter for messages being sent to the server.
 * The filter is a bitmask representing priorities.  The ASL_FILTER_MASK
 * macro may be used to convert a priority level into a bitmask for that
 * level.  The ASL_FILTER_MASK_UPTO macro creates a bitmask for all
 * priorities up to and including a given priority.
 * Messages with priority levels that do not have a corresponding bit 
 * set in the filter are not sent to the server, although they will be
 * sent to any file descripters added with asl_add_log_file().
 * The default setting is ASL_FILTER_MASK_UPTO(ASL_LEVEL_NOTICE).
 * Returns the previous filter value.
 *
 * @param asl
 *    (input) An ASL client handle
 * @param f
 *    (input) A filter value
 * @result Returns the previous filter value
 */
ASL_ENTRY_POINT int asl_set_filter(aslclient asl, int f);

/*
 * Examine attribute keys.
 *
 * @param msg
 *    (input) An ASL message
 * @param n
 *    (input) An index value
 * @result Returns the key of the nth attribute in a message (beginning at zero),
 * or NULL if n is greater than the largest message index.
 */
ASL_ENTRY_POINT const char *asl_key(aslmsg msg, uint32_t n);

/*!
 * Create a new log message or query message.
 *
 * @param type
 *    (input) Message type (see aslmsg Types)
 * @result Returns a newly allocated asmsg of the specified type
 */
ASL_ENTRY_POINT aslmsg asl_new(uint32_t type);

/*!
 * Set or re-set a message attribute.
 *
 * @param msg
 *    (input) An aslmsg
 * @param key
 *    (input) Attribute key 
 * @param value
 *    (input) Attribute value
 * @result returns 0 for success, non-zero for failure
 */
ASL_ENTRY_POINT int asl_set(aslmsg msg, const char *key, const char *value);

/*!
 * Remove a message attribute.
 *
 * @param msg
 *    (input) An aslmsg
 * @param key
 *    (input) Attribute key 
 * returns 0 for success, non-zero for failure
 */
ASL_ENTRY_POINT int asl_unset(aslmsg msg, const char *key);

/*!
 * Get the value of a message attribute.
 *
 * @param msg
 *    (input) An aslmsg
 * @param key
 *    (input) Attribute key 
 * @result Returns the attribute value, or NULL if the message does not contain the key
 */
ASL_ENTRY_POINT const char *asl_get(aslmsg msg, const char *key);

/*!
 * Log a message with a particular log level.
 *
 * @param asl
 *    (input) An ASL client handle
 * @param msg
 *    (input) An aslmsg (default attributes will be supplied if msg is NULL)
 * @param level
 *    (input) Log level (ASL_LEVEL_DEBUG to ASL_LEVEL_EMERG)
 * @param format
 *    (input) A printf() - style format string followed by a list of arguments
 * @result Returns 0 for success, non-zero for failure
 */
#ifdef __DARWIN_LDBL_COMPAT2
ASL_ENTRY_POINT int asl_log(aslclient asl, aslmsg msg, int level, const char *format, ...) __DARWIN_LDBL_COMPAT2(asl_log) __printflike(4, 5);
#else
ASL_ENTRY_POINT int asl_log(aslclient asl, aslmsg msg, int level, const char *format, ...) __printflike(4, 5);
#endif

/*!
 * Log a message with a particular log level.
 * Similar to asl_log, but takes a va_list argument.
 *
 * @param asl
 *    (input) An ASL client handle
 * @param msg
 *    (input) An aslmsg (default attributes will be supplied if msg is NULL)
 * @param level
 *    (input) Log level (ASL_LEVEL_DEBUG to ASL_LEVEL_EMERG)
 * @param format
 *    (input) A printf() - style format string followed by a list of arguments
 * @param ap
 *    (input) A va_list containing the values for the format string
 * @result Returns 0 for success, non-zero for failure
 */
#ifdef __DARWIN_LDBL_COMPAT2
ASL_ENTRY_POINT int asl_vlog(aslclient asl, aslmsg msg, int level, const char *format, va_list ap) __DARWIN_LDBL_COMPAT2(asl_vlog) __printflike(4, 0);
#else
ASL_ENTRY_POINT int asl_vlog(aslclient asl, aslmsg msg, int level, const char *format, va_list ap) __printflike(4, 0);
#endif

/*!
 * Log a message.
 *
 * This routine may be used instead of asl_log() or asl_vlog() if asl_set() 
 * has been used to set all of a message's attributes.
 *
 * @param asl
 *    (input) An ASL client handle
 * @param msg
 *    (input) An aslmsg
 * @result Returns 0 for success, non-zero for failure
 */
ASL_ENTRY_POINT int asl_send(aslclient asl, aslmsg msg);

/*!
 * Free a message.  Frees all the attribute keys and values.
 *
 * @param msg
 *    (input) An aslmsg to free
 */
ASL_ENTRY_POINT void asl_free(aslmsg msg);

/*!
 * Set arbitrary parameters of a query.
 * This is similar to asl_set, but allows richer query operations.
 * See ASL_QUERY_OP_* above.
 *
 * @param msg
 *    (input) An aslmsg
 * @param key
 *    (input) Attribute key 
 * @param value
 *    (input) Attribute value
 * @param op
 *    (input) An operation (ASL_QUERY_OP_*)
 * @result Returns 0 for success, non-zero for failure
 */
ASL_ENTRY_POINT int asl_set_query(aslmsg msg, const char *key, const char *value, uint32_t op);

/*!
 * Search for messages matching the criteria described by the aslmsg.
 * The caller should set the attributes to match using asl_set_query() or asl_set().
 * The operatoin ASL_QUERY_OP_EQUAL is used for attributes set with asl_set().
 *
 * @param msg
 *    (input) An aslmsg to match
 * @result Returns a set of messages accessable using aslresponse_next(),
 */
ASL_ENTRY_POINT aslresponse asl_search(aslclient asl, aslmsg msg);

/*!
 * Iterate over responses returned from asl_search().
 *
 * @param r
 *    (input) An aslresponse returned by asl_search()
 * @result Returns the next message (an aslmsg) in the response, or NULL when there are no more messages
 */
ASL_ENTRY_POINT aslmsg aslresponse_next(aslresponse r);

/*!
 * Free a response returned from asl_search().
 * @param r
 *    (input) An aslresponse returned by asl_search()
 */
ASL_ENTRY_POINT void aslresponse_free(aslresponse r);

__END_DECLS

#endif /* __ASL_H__ */

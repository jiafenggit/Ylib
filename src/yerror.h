/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	yerror.h
 * @abstract	Definitions for error handling.
 * @discussion	The type yerr_t is a convenient way to transmit error values.
 *		When a yerr_t value is positive, there is no error. If it is
 *		negative or equal to zero, there is an error. When a function
 *		have to return an error code, it must return YENOERR in case
 *		of success, or a code that depend of the occuring error.
 * @see		The file $INCLUDE/asm/error.h
 * @version	1.0.0 Aug 12 2002
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YERROR_H__
#define __YERROR_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif // __cplusplus || c_plusplus

#include <errno.h>

/**
 * @enum	yerr_e
 *		Error constants.
 * @constant	YENOERR No error.
 * @constant	YEUNDEF Error not defined. 
 * @constant	YEPERM Operation not permitted. 
 * @constant	YENOENT No such file or directory. 
 * @constant	YESRCH No such process. 
 * @constant	YEINTR Interrupted system call. 
 * @constant	YEIO I/O error. 
 * @constant	YENXIO No such device or address. 
 * @constant	YE2BIG Arg list too long. 
 * @constant	YENOEXEC Exec format error. 
 * @constant	YEBADF Bad file number. 
 * @constant	YECHILD No child processes. 
 * @constant	YEAGAIN Try again. 
 * @constant	YENOMEM Out of memory. 
 * @constant	YEACCESS Permission denied. 
 * @constant	YEFAULT Bad address. 
 * @constant	YENOTBLK Block device required. 
 * @constant	YEBUSY Device or resource busy. 
 * @constant	YEEXIST File exists. 
 * @constant	YEXDEV Cross-device link. 
 * @constant	YENODEV No such device. 
 * @constant	YENOTDIR Not a directory. 
 * @constant	YEISDIR Is a directory. 
 * @constant	YEINVAL Invalid argument. 
 * @constant	YENFILE File table overflow. 
 * @constant	YEMFILE Too many open files. 
 * @constant	YENOTTY Not a typewriter. 
 * @constant	YETXTBSY Text file busy. 
 * @constant	YEFBIG File too large. 
 * @constant	YENOSPC No space left on device. 
 * @constant	YESPIPE Illegal seek. 
 * @constant	YEROFS Read-only file system. 
 * @constant	YEMLINK Too many links. 
 * @constant	YEPIPE Broken pipe. 
 * @constant	YEDOM Math argument out of domain of func. 
 * @constant	YERANGE Math result not representable. 
 * @constant	YEDEADLK Resource deadlock would occur. 
 * @constant	YENAMETOOLONG File name too long. 
 * @constant	YENOLCK No record locks available. 
 * @constant	YENOSYS Function not implemented. 
 * @constant	YENOTEMPTY Directory not empty. 
 * @constant	YELOOP Too many symbolic links encountered. 
 * @constant	YEWOULDBLOCK Operation would block. 
 * @constant	YENOMSG No message of desired type. 
 * @constant	YEIDRM Identifier removed. 
 * @constant	YECHRNG Channel number out of range. 
 * @constant	YEL2NSYNC Level 2 not synchronized. 
 * @constant	YEL3HLT Level 3 halted. 
 * @constant	YEL3RST Level 3 reset. 
 * @constant	YELNRNG Link number out of range. 
 * @constant	YEUNATCH Protocol driver not attached. 
 * @constant	YENOCSI No CSI structure available. 
 * @constant	YEL2HLT Level 2 halted. 
 * @constant	YEBADE Invalid exchange. 
 * @constant	YEBADR Invalid request descriptor. 
 * @constant	YEXFULL Exchange full. 
 * @constant	YENOANO No anode. 
 * @constant	YEBADRQC Invalid request code. 
 * @constant	YEBADSLT Invalid slot. 
 * @constant	YEDEADLOCK Dead lock. 
 * @constant	YEBFONT Bad font file format. 
 * @constant	YENOSTR Device not a stream. 
 * @constant	YENODATA No data available. 
 * @constant	YETIME Timer expired. 
 * @constant	YENOSR Out of streams resources. 
 * @constant	YENONET Machine is not on the network. 
 * @constant	YENOPKG Package not installed. 
 * @constant	YEREMOTE Object is remote. 
 * @constant	YENOLINK Link has been severed. 
 * @constant	YEADV Advertise error. 
 * @constant	YESRMNT Srmount error. 
 * @constant	YECOMM Communication error on send. 
 * @constant	YEPROTO Protocol error. 
 * @constant	YEMULTIHOP Multihop attempted. 
 * @constant	YEDOTDOT RFS specific error. 
 * @constant	YEBADMSG Not a data message. 
 * @constant	YEOVERFLOW Value too large for defined data type. 
 * @constant	YENOTUNIQ Name not unique on network. 
 * @constant	YEBADFD File descriptor in bad state. 
 * @constant	YEREMCHG Remote address changed. 
 * @constant	YELIBACC Can not access a needed shared library. 
 * @constant	YELIBBAD Accessing a corrupted shared library. 
 * @constant	YELIBSCN .lib section in a.out corrupted. 
 * @constant	YELIBMAX Attempting to link in too many shared libraries. 
 * @constant	YELIBEXEC Cannot exec a shared library directly. 
 * @constant	YEILSEQ Illegal byte sequence. 
 * @constant	YERESTART Interrupted system call should be restarted. 
 * @constant	YESTRPIPE Streams pipe error. 
 * @constant	YEUSERS Too many users. 
 * @constant	YENOTSOCK Socket operation on non-socket. 
 * @constant	YEDESTADDRREQ Destination address required. 
 * @constant	YEMSGSIZE Message too long. 
 * @constant	YEPROTOTYPE Protocol wrong type for socket. 
 * @constant	YENOPROTOOPT Protocol not available. 
 * @constant	YEPROTONOSUPPORT Protocol not supported. 
 * @constant	YESOCKTNOSUPPORT Socket type not supported. 
 * @constant	YEOPNOTSUPP Operation not supported on transport endpoint. 
 * @constant	YEPFNOSUPPORT Protocol family not supported. 
 * @constant	YEAFNOSUPPORT Address family not supported by protocol. 
 * @constant	YEADDRINUSE Address already in use. 
 * @constant	YEADDRNOTAVAIL Cannot assign requested address. 
 * @constant	YENETDOWN Network is down. 
 * @constant	YENETUNREACH Network is unreachable. 
 * @constant	YENETRESET Network dropped connection because of reset. 
 * @constant	YECONNABORTED Software caused connection abort. 
 * @constant	YECONNRESET Connection reset by peer. 
 * @constant	YENOBUFS No buffer space available. 
 * @constant	YEISCONN Transport endpoint is already connected. 
 * @constant	YENOTCONN Transport endpoint is not connected. 
 * @constant	YESHUTDOWN Cannot send after transport endpoint shutdown. 
 * @constant	YETOOMANYREFS Too many references: cannot splice. 
 * @constant	YETIMEDOUT Connection timed out. 
 * @constant	YECONNREFUSED Connection refused. 
 * @constant	YEHOSTDOWN Host is down. 
 * @constant	YEHOSTUNREACH No route to host. 
 * @constant	YEALREADY Operation already in progress. 
 * @constant	YEINPROGRESS Operation now in progress. 
 * @constant	YESTALE Stale NFS file handle. 
 * @constant	YEUCLEAN Structure needs cleaning. 
 * @constant	YENOTNAM Not a XENIX named type file. 
 * @constant	YENAVAIL No XENIX semaphores available. 
 * @constant	YEISNAM Is a named type file. 
 * @constant	YEREMOTEIO Remote I/O error. 
 * @constant	YEDQUOT Quota exceeded. 
 * @constant	YENOMEDIUM No medium found. 
 * @constant	YEMEDIUMTYPE Wrong medium type. 
 * @constant	YESYNTAX Syntax error. 
 */
enum yerr_e {
	YENOERR			= 1,
	YEUNDEF			= 0,
	YEPERM			= -1,
	YENOENT			= -2,
	YESRCH			= -3,
	YEINTR			= -4,
	YEIO			= -5,
	YENXIO			= -6,
	YE2BIG			= -7,
	YENOEXEC		= -8,
	YEBADF			= -9,
	YECHILD			= -10,
	YEAGAIN			= -11,
	YENOMEM			= -12,
	YEACCESS		= -13,
	YEFAULT			= -14,
	YENOTBLK		= -15,
	YEBUSY			= -16,
	YEEXIST			= -17,
	YEXDEV			= -18,
	YENODEV			= -19,
	YENOTDIR		= -20,
	YEISDIR			= -21,
	YEINVAL			= -22,
	YENFILE			= -23,
	YEMFILE			= -24,
	YENOTTY			= -25,
	YETXTBSY		= -26,
	YEFBIG			= -27,
	YENOSPC			= -28,
	YESPIPE			= -29,
	YEROFS			= -30,
	YEMLINK			= -31,
	YEPIPE			= -32,
	YEDOM			= -33,
	YERANGE			= -34,
	YEDEADLK		= -35,
	YENAMETOOLONG		= -36,
	YENOLCK			= -37,
	YENOSYS			= -38,
	YENOTEMPTY		= -39,
	YELOOP			= -40,
	YEWOULDBLOCK		= -EAGAIN,
	YENOMSG			= -42,
	YEIDRM			= -43,
	YECHRNG			= -44,
	YEL2NSYNC		= -45,
	YEL3HLT			= -46,
	YEL3RST			= -47,
	YELNRNG			= -48,
	YEUNATCH		= -49,
	YENOCSI			= -50,
	YEL2HLT			= -51,
	YEBADE			= -52,
	YEBADR			= -53,
	YEXFULL			= -54,
	YENOANO			= -55,
	YEBADRQC		= -56,
	YEBADSLT		= -57,
	YEDEADLOCK		= -EDEADLK,
	YEBFONT			= -59,
	YENOSTR			= -60,
	YENODATA		= -61,
	YETIME			= -62,
	YENOSR			= -63,
	YENONET			= -64,
	YENOPKG			= -65,
	YEREMOTE		= -66,
	YENOLINK		= -67,
	YEADV			= -68,
	YESRMNT			= -69,
	YECOMM			= -70,
	YEPROTO			= -71,
	YEMULTIHOP		= -72,
	YEDOTDOT		= -73,
	YEBADMSG		= -74,
	YEOVERFLOW		= -75,
	YENOTUNIQ		= -76,
	YEBADFD			= -77,
	YEREMCHG		= -78,
	YELIBACC		= -79,
	YELIBBAD		= -80,
	YELIBSCN		= -81,
	YELIBMAX		= -82,
	YELIBEXEC		= -83,
	YEILSEQ			= -84,
	YERESTART		= -85,
	YESTRPIPE		= -86,
	YEUSERS			= -87,
	YENOTSOCK		= -88,
	YEDESTADDRREQ		= -89,
	YEMSGSIZE		= -90,
	YEPROTOTYPE		= -91,
	YENOPROTOOPT		= -92,
	YEPROTONOSUPPORT	= -93,
	YESOCKTNOSUPPORT	= -94,
	YEOPNOTSUPP		= -95,
	YEPFNOSUPPORT		= -96,
	YEAFNOSUPPORT		= -97,
	YEADDRINUSE		= -98,
	YEADDRNOTAVAIL		= -99,
	YENETDOWN		= -100,
	YENETUNREACH		= -101,
	YENETRESET		= -102,
	YECONNABORTED		= -103,
	YECONNRESET		= -104,
	YENOBUFS		= -105,
	YEISCONN		= -106,
	YENOTCONN		= -107,
	YESHUTDOWN		= -108,
	YETOOMANYREFS		= -109,
	YETIMEDOUT		= -110,
	YECONNREFUSED		= -111,
	YEHOSTDOWN		= -112,
	YEHOSTUNREACH		= -113,
	YEALREADY		= -114,
	YEINPROGRESS		= -115,
	YESTALE			= -116,
	YEUCLEAN		= -117,
	YENOTNAM		= -118,
	YENAVAIL		= -119,
	YEISNAM			= -120,
	YEREMOTEIO		= -121,
	YEDQUOT			= -122,
	YENOMEDIUM		= -123,
	YEMEDIUMTYPE		= -124,
	YESYNTAX		= -125
};

/** @typedef yerr_t See enum yerr_e. */
typedef enum yerr_e yerr_t;

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif // __cplusplus || c_plusplus 

#endif // __YERROR_H__ 

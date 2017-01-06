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
 * @link	http://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/Errors/unix_system_errors.html
 * @link	http://www.virtsync.com/c-error-codes-include-errno
 * @version	1.0.0 Aug 12 2002
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#pragma once

#include <errno.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif // __cplusplus || c_plusplus

#include <errno.h>

/**
 * @enum	yerr_e
 *		Error constants.
 * @constant	YEOK			No error.
 * @constant	YENOERR			No error.
 * @constant	YEUNDEF			Error not defined. 
 * @constant	YEPERM			Operation not permitted. 
 * @constant	YENOENT			No such file or directory. 
 * @constant	YESRCH			No such process. 
 * @constant	YEINTR			Interrupted system call. 
 * @constant	YEIO			I/O error. 
 * @constant	YENXIO			No such device or address. 
 * @constant	YE2BIG			Arg list too long. 
 * @constant	YENOEXEC		Exec format error. 
 * @constant	YEBADF			Bad file number. 
 * @constant	YECHILD			No child processes. 
 * @constant	YEAGAIN			Try again. 
 * @constant	YENOMEM			Out of memory. 
 * @constant	YEACCESS		Permission denied. 
 * @constant	YEFAULT			Bad address. 
 * @constant	YENOTBLK		Block device required. 
 * @constant	YEBUSY			Device or resource busy. 
 * @constant	YEEXIST			File exists. 
 * @constant	YEXDEV			Cross-device link. 
 * @constant	YENODEV			No such device. 
 * @constant	YENOTDIR		Not a directory. 
 * @constant	YEISDIR			Is a directory. 
 * @constant	YEINVAL			Invalid argument. 
 * @constant	YENFILE			File table overflow. 
 * @constant	YEMFILE			Too many open files. 
 * @constant	YENOTTY			Not a typewriter. 
 * @constant	YETXTBSY		Text file busy. 
 * @constant	YEFBIG			File too large. 
 * @constant	YENOSPC			No space left on device. 
 * @constant	YESPIPE			Illegal seek. 
 * @constant	YEROFS			Read-only file system. 
 * @constant	YEMLINK			Too many links. 
 * @constant	YEPIPE			Broken pipe. 
 * @constant	YEDOM			Math argument out of domain of func. 
 * @constant	YERANGE			Math result not representable.
 * @constant	YEDEADLK		Resource deadlock would occur. 
 * @constant	YENAMETOOLONG		File name too long. 
 * @constant	YENOLCK			No record locks available. 
 * @constant	YENOSYS			Function not implemented. 
 * @constant	YENOTEMPTY		Directory not empty. 
 * @constant	YELOOP			Too many symbolic links encountered. 
 * @constant	YEWOULDBLOCK		Operation would block. 
 * @constant	YENOMSG			No message of desired type. 
 * @constant	YEIDRM			Identifier removed. 
 * @constant	YECHRNG			Channel number out of range. 
 * @constant	YEL2NSYNC		Level 2 not synchronized. 
 * @constant	YEL3HLT			Level 3 halted. 
 * @constant	YEL3RST			Level 3 reset. 
 * @constant	YELNRNG			Link number out of range. 
 * @constant	YEUNATCH		Protocol driver not attached. 
 * @constant	YENOCSI			No CSI structure available. 
 * @constant	YEL2HLT			Level 2 halted. 
 * @constant	YEBADE			Invalid exchange. 
 * @constant	YEBADR			Invalid request descriptor. 
 * @constant	YEXFULL			Exchange full. 
 * @constant	YENOANO			No anode. 
 * @constant	YEBADRQC		Invalid request code. 
 * @constant	YEBADSLT		Invalid slot. 
 * @constant	YEDEADLOCK		Dead lock. 
 * @constant	YEBFONT			Bad font file format. 
 * @constant	YENOSTR			Device not a stream. 
 * @constant	YENODATA		No data available. 
 * @constant	YETIME			Timer expired. 
 * @constant	YENOSR			Out of streams resources. 
 * @constant	YENONET			Machine is not on the network. 
 * @constant	YENOPKG			Package not installed. 
 * @constant	YEREMOTE		Object is remote. 
 * @constant	YENOLINK		Link has been severed. 
 * @constant	YEADV			Advertise error. 
 * @constant	YESRMNT			Srmount error. 
 * @constant	YECOMM			Communication error on send. 
 * @constant	YEPROTO			Protocol error. 
 * @constant	YEMULTIHOP		Multihop attempted. 
 * @constant	YEDOTDOT		RFS specific error. 
 * @constant	YEBADMSG		Not a data message. 
 * @constant	YEOVERFLOW		Value too large for defined data type. 
 * @constant	YENOTUNIQ		Name not unique on network. 
 * @constant	YEBADFD			File descriptor in bad state. 
 * @constant	YEREMCHG		Remote address changed. 
 * @constant	YELIBACC		Can not access a needed shared library. 
 * @constant	YELIBBAD		Accessing a corrupted shared library. 
 * @constant	YELIBSCN		.lib section in a.out corrupted. 
 * @constant	YELIBMAX		Attempting to link in too many shared libraries. 
 * @constant	YELIBEXEC		Cannot exec a shared library directly. 
 * @constant	YEILSEQ			Illegal byte sequence. 
 * @constant	YERESTART		Interrupted system call should be restarted. 
 * @constant	YESTRPIPE		Streams pipe error. 
 * @constant	YEUSERS			Too many users. 
 * @constant	YENOTSOCK		Socket operation on non-socket. 
 * @constant	YEDESTADDRREQ		Destination address required. 
 * @constant	YEMSGSIZE		Message too long. 
 * @constant	YEPROTOTYPE		Protocol wrong type for socket. 
 * @constant	YENOPROTOOPT		Protocol not available. 
 * @constant	YEPROTONOSUPPORT	Protocol not supported. 
 * @constant	YESOCKTNOSUPPORT	Socket type not supported. 
 * @constant	YEOPNOTSUPP		Operation not supported on transport endpoint. 
 * @constant	YEPFNOSUPPORT		Protocol family not supported. 
 * @constant	YEAFNOSUPPORT		Address family not supported by protocol. 
 * @constant	YEADDRINUSE		Address already in use. 
 * @constant	YEADDRNOTAVAIL		Cannot assign requested address. 
 * @constant	YENETDOWN		Network is down. 
 * @constant	YENETUNREACH		Network is unreachable. 
 * @constant	YENETRESET		Network dropped connection because of reset. 
 * @constant	YECONNABORTED		Software caused connection abort. 
 * @constant	YECONNRESET		Connection reset by peer. 
 * @constant	YENOBUFS		No buffer space available. 
 * @constant	YEISCONN		Transport endpoint is already connected. 
 * @constant	YENOTCONN		Transport endpoint is not connected. 
 * @constant	YESHUTDOWN		Cannot send after transport endpoint shutdown. 
 * @constant	YETOOMANYREFS		Too many references: cannot splice. 
 * @constant	YETIMEDOUT		Connection timed out. 
 * @constant	YECONNREFUSED		Connection refused. 
 * @constant	YEHOSTDOWN		Host is down. 
 * @constant	YEHOSTUNREACH		No route to host. 
 * @constant	YEALREADY		Operation already in progress. 
 * @constant	YEINPROGRESS		Operation now in progress. 
 * @constant	YESTALE			Stale NFS file handle. 
 * @constant	YEUCLEAN		Structure needs cleaning. 
 * @constant	YENOTNAM		Not a XENIX named type file. 
 * @constant	YENAVAIL		No XENIX semaphores available. 
 * @constant	YEISNAM			Is a named type file. 
 * @constant	YEREMOTEIO		Remote I/O error. 
 * @constant	YEDQUOT			Quota exceeded. 
 * @constant	YENOMEDIUM		No medium found. 
 * @constant	YEMEDIUMTYPE		Wrong medium type.
 * @constant	YECANCELED		Operation Canceled.
 * @constant	YENOKEY			Required key not available.
 * @constant	YEKEYEXPIRED		Key has expired.
 * @contstant	YEKEYREVOKED		Key has been revoked.
 * @constant	YEKEYREJECTED		Key was rejected by service.
 * @constant	YEOWNERDEAD		Owner died.
 * @constant	YENOTRECOVERABLE	State not recoverable.
 * @constant	YESYNTAX		Syntax error. 
 */
enum yerr_e {
	YEOK			= 1,
	YENOERR			= 1,
	YEUNDEF			= 0,
	YEPERM			= -EPERM,
	YENOENT			= -ENOENT,
	YESRCH			= -ESRCH,
	YEINTR			= -EINTR,
	YEIO			= -EIO,
	YENXIO			= -ENXIO,
	YE2BIG			= -E2BIG,
	YENOEXEC		= -ENOEXEC,
	YEBADF			= -EBADF,
	YECHILD			= -ECHILD,
	YEAGAIN			= -EAGAIN,
	YENOMEM			= -ENOMEM,
	YEACCES			= -EACCES,
	YEFAULT			= -EFAULT,
	YENOTBLK		= -ENOTBLK,
	YEBUSY			= -EBUSY,
	YEEXIST			= -EEXIST,
	YEXDEV			= -EXDEV,
	YENODEV			= -ENODEV,
	YENOTDIR		= -ENOTDIR,
	YEISDIR			= -EISDIR,
	YEINVAL			= -EINVAL,
	YENFILE			= -ENFILE,
	YEMFILE			= -EMFILE,
	YENOTTY			= -ENOTTY,
	YETXTBSY		= -ETXTBSY,
	YEFBIG			= -EFBIG,
	YENOSPC			= -ENOSPC,
	YESPIPE			= -ESPIPE,
	YEROFS			= -EROFS,
	YEMLINK			= -EMLINK,
	YEPIPE			= -EPIPE,
	YEDOM			= -EDOM,
	YERANGE			= -ERANGE,
	YEDEADLK		= -EDEADLK,
	YENAMETOOLONG		= -ENAMETOOLONG,
	YENOLCK			= -ENOLCK,
	YENOSYS			= -ENOSYS,
	YENOTEMPTY		= -ENOTEMPTY,
	YELOOP			= -ELOOP,
	YEWOULDBLOCK		= -EAGAIN,
	YENOMSG			= -ENOMSG,
	YEIDRM			= -EIDRM,
	YECHRNG			= -ECHRNG,
	YEL2NSYNC		= -EL2NSYNC,
	YEL3HLT			= -EL3HLT,
	YEL3RST			= -EL3RST,
	YELNRNG			= -ELNRNG,
	YEUNATCH		= -EUNATCH,
	YENOCSI			= -ENOCSI,
	YEL2HLT			= -EL2HLT,
	YEBADE			= -EBADE,
	YEBADR			= -EBADR,
	YEXFULL			= -EXFULL,
	YENOANO			= -ENOANO,
	YEBADRQC		= -EBADRQC,
	YEBADSLT		= -EBADSLT,
	YEDEADLOCK		= -EDEADLK,
	YEBFONT			= -EBFONT,
	YENOSTR			= -ENOSTR,
	YENODATA		= -ENODATA,
	YETIME			= -ETIME,
	YENOSR			= -ENOSR,
	YENONET			= -ENONET,
	YENOPKG			= -ENOPKG,
	YEREMOTE		= -EREMOTE,
	YENOLINK		= -ENOLINK,
	YEADV			= -EADV,
	YESRMNT			= -ESRMNT,
	YECOMM			= -ECOMM,
	YEPROTO			= -EPROTO,
	YEMULTIHOP		= -EMULTIHOP,
	YEDOTDOT		= -EDOTDOT,
	YEBADMSG		= -EBADMSG,
	YEOVERFLOW		= -EOVERFLOW,
	YENOTUNIQ		= -ENOTUNIQ,
	YEBADFD			= -EBADFD,
	YEREMCHG		= -EREMCHG,
	YELIBACC		= -ELIBACC,
	YELIBBAD		= -ELIBBAD,
	YELIBSCN		= -ELIBSCN,
	YELIBMAX		= -ELIBMAX,
	YELIBEXEC		= -ELIBEXEC,
	YEILSEQ			= -EILSEQ,
	YERESTART		= -ERESTART,
	YESTRPIPE		= -ESTRPIPE,
	YEUSERS			= -EUSERS,
	YENOTSOCK		= -ENOTSOCK,
	YEDESTADDRREQ		= -EDESTADDRREQ,
	YEMSGSIZE		= -EMSGSIZE,
	YEPROTOTYPE		= -EPROTOTYPE,
	YENOPROTOOPT		= -ENOPROTOOPT,
	YEPROTONOSUPPORT	= -EPROTONOSUPPORT,
	YESOCKTNOSUPPORT	= -ESOCKTNOSUPPORT,
	YEOPNOTSUPP		= -EOPNOTSUPP,
	YEPFNOSUPPORT		= -EPFNOSUPPORT,
	YEAFNOSUPPORT		= -EAFNOSUPPORT,
	YEADDRINUSE		= -EADDRINUSE,
	YEADDRNOTAVAIL		= -EADDRNOTAVAIL,
	YENETDOWN		= -ENETDOWN,
	YENETUNREACH		= -ENETUNREACH,
	YENETRESET		= -ENETRESET,
	YECONNABORTED		= -ECONNABORTED,
	YECONNRESET		= -ECONNRESET,
	YENOBUFS		= -ENOBUFS,
	YEISCONN		= -EISCONN,
	YENOTCONN		= -ENOTCONN,
	YESHUTDOWN		= -ESHUTDOWN,
	YETOOMANYREFS		= -ETOOMANYREFS,
	YETIMEDOUT		= -ETIMEDOUT,
	YECONNREFUSED		= -ECONNREFUSED,
	YEHOSTDOWN		= -EHOSTDOWN,
	YEHOSTUNREACH		= -EHOSTUNREACH,
	YEALREADY		= -EALREADY,
	YEINPROGRESS		= -EINPROGRESS,
	YESTALE			= -ESTALE,
	YEUCLEAN		= -EUCLEAN,
	YENOTNAM		= -ENOTNAM,
	YENAVAIL		= -ENAVAIL,
	YEISNAM			= -EISNAM,
	YEREMOTEIO		= -EREMOTEIO,
	YEDQUOT			= -EDQUOT,
	YENOMEDIUM		= -ENOMEDIUM,
	YEMEDIUMTYPE		= -EMEDIUMTYPE,
	YECANCELED		= -ECANCELED,
	YENOKEY			= -ENOKEY,
	YEKEYEXPIRED		= -EKEYEXPIRED,
	YEKEYREVOKED		= -EKEYREVOKED,
	YEKEYREJECTED		= -EKEYREJECTED,
	YEOWNERDEAD		= -EOWNERDEAD,
	YENOTRECOVERABLE	= -ENOTRECOVERABLE,
	YESYNTAX		= -0xffff
};

/** @typedef yerr_t See enum yerr_e. */
typedef enum yerr_e yerr_t;

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif // __cplusplus || c_plusplus 

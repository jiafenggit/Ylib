
struct _ylib_namespace y = {
	.sys = {
		.malloc = _ymalloc,
		.calloc = _ycalloc,
		.free = _yfree
	},
	.str = {
		.create = _ys_new,
		.free = _ys_free,
		.trunc = _ys_trunc,
		.setsz = _ys_setsz,
		.len = _ys_len,
		.append = _ys_append,
		.prepend = _ys_prepend,
		.nappend = _ys_nappend,
		.nprepend = _ys_nprepend,
		.dup = _ys_dup,
		.tostring = _ys_string,
		.concat = _ys_concat,
		.ltrim = _ys_ltrim,
		.rtrim = _ys_rtrim,
		.trim = _ys_trim,
		.lshift = _ys_lshift,
		.rshift = _ys_rshift,
		.putc = _ys_putc,
		.addc = _ys_addc,
		.upcase = _ys_upcase,
		.lowcase = _ys_lowcase,
		.printf = _ys_printf,
		.vprintf = _ys_vprintf,
		.str2hexa = _ys_str2hexa,
		.subs = _ys_subs,
		.casesubs = _ys_casesubs,
		.str2xmlentity = _str2xmlentity,
		.xmlentity2str = _xmlentity2str
	}
};


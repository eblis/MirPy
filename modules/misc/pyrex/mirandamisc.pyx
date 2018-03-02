import MirPy


def VersionInfoData(disableFormatting = 0):
	"""VersionInfoData(disableFormatting = 0)
	Returns the report provided by VersionInfo plugin.
	"""
	cdef char *data
	data = NULL
	res = MirPy.CallService("Versioninfo/GetInfo", <long> disableFormatting, <long> &data)
	buffer = data
	if (data != <char *> 0):
		MirPy.MirandaFree(<long> data)
	return buffer
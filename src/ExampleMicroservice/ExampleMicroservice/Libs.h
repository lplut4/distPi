#pragma once

#ifdef _WIN64

	#pragma comment(lib, "Ws2_32.lib" )
	#pragma comment(lib, "wsock32.lib" ) 
	#pragma comment(lib, "hiredis.lib" )
	#pragma comment(lib, "redis++_static.lib" ) 
	#pragma comment(lib, "kernel32.lib" ) 
	#pragma comment(lib, "user32.lib" )
	#pragma comment(lib, "gdi32.lib" )
	#pragma comment(lib, "winspool.lib" )
	#pragma comment(lib, "comdlg32.lib" )
	#pragma comment(lib, "advapi32.lib" )
	#pragma comment(lib, "shell32.lib" )
	#pragma comment(lib, "ole32.lib" )
	#pragma comment(lib, "oleaut32.lib" )
	#pragma comment(lib, "uuid.lib" )
	#pragma comment(lib, "odbc32.lib" )
	#pragma comment(lib, "odbccp32.lib" )

#else

#endif
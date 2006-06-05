/*
 *  Copyright 2005-2006 The Apache Software Foundation or its licensors, as applicable.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 * @author Nikolay A. Sidelnikov
 * @version $Revision: 1.8.12.1.4.3 $
 */

#ifndef _PLATFORMDEPENDANT_H_
#define _PLATFORMDEPENDANT_H_


#ifndef _MSC_VER
    #define strcmpi strcasecmp
    #define stricmp strcasecmp
    #define strnicmp strncasecmp
#else
    #pragma warning( push, 4 )
    #pragma warning( disable : 4100 4127 4201 4511 4512 )
	#pragma conform( forScope, on )
#endif //_MSC_VER

#undef stdcall__
#undef cdecl_		
#ifdef PLATFORM_POSIX

#ifndef  __stdcall
   #define __stdcall
#endif

#ifndef  _cdecl
    #define _cdecl
#endif
    
    #define stdcall__    __attribute__ ((__stdcall__))
    #define cdecl_       __attribute__ ((__cdecl__))
#else
    #define stdcall__
    #define cdecl_		
#endif

#endif // _PLATFORMDEPENDANT_H_

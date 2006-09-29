                            >  G     �  �   	  1    #   File:       noresnames.make
#   Target:     noresnames
#   Sources:    noresnames.c
#   Created:    Tuesday, January 25, 2000 08:29:29 PM


MAKEFILE     = noresnames.make
�MondoBuild� = {MAKEFILE}  # Make blank to avoid rebuilds when makefile is modified
Includes     =
Sym�PPC      = 
ObjDir�PPC   =
Sym�68K      = 
ObjDir�68K   =

PPCCOptions  = {Includes} {Sym�PPC} -opt size 

COptions     = {Includes} {Sym�68K} -mc68020 

Objects�PPC  = �
		"{ObjDir�PPC}noresnames.c.x"

Objects�68K  = �
		"{ObjDir�68K}noresnames.c.o"


noresnames �� {�MondoBuild�} {Objects�PPC}
	PPCLink �
		-o {Targ} {Sym�PPC} �
		{Objects�PPC} �
		-t 'MPST' �
		-c 'MPS ' �
		"{SharedLibraries}InterfaceLib" �
		"{SharedLibraries}StdCLib" �
		"{SharedLibraries}MathLib" �
		"{PPCLibraries}StdCRuntime.o" �
		"{PPCLibraries}PPCCRuntime.o" �
		"{PPCLibraries}PPCToolLibs.o"


noresnames �� {�MondoBuild�} {Objects�68K}
	Link �
		-o {Targ} -d {Sym�68K} �
		{Objects�68K} �
		-t 'MPST' �
		-c 'MPS ' �
		"{Libraries}Stubs.o" �
		#"{CLibraries}Complex.o" �
		"{CLibraries}StdCLib.o" �
		"{Libraries}MacRuntime.o" �
		"{Libraries}IntEnv.o" �
		"{Libraries}Interface.o"


"{ObjDir�PPC}noresnames.c.x" � {�MondoBuild�} noresnames.c
	{PPCC} noresnames.c -o {Targ} {PPCCOptions}


"{ObjDir�68K}noresnames.c.o" � {�MondoBuild�} noresnames.c
	{C} noresnames.c -o {Targ} {COptions}

     n   n   >ine TARGET_RT_BIG_ENDIAN		1
	#define TARGET_RT_MAC_CFM			0
	#define TARGET_RT_MAC_MACHO			0
	#if defined(mc68881)
		#define TARGET_RT_MAC_68881		1
	#else
		#define TARGET_RT_MAC_68881		0
	#endif
	#define PRAGMA_IMPORT				0
	#define PRAGMA_S   H 	Monaco t: ��Pnoresnames
#   Sou   < $m� < $m�����  ]  ]  V     < $m� < $m�  V               n   n   >���4    >  MPSR  
���        ���   L�$TEXTCWIE z           �     ^A
# Microsoft Developer Studio Project File - Name="TdxW" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TdxW - Win32 Debug_CF_LEVEL2
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TdxW.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TdxW.mak" CFG="TdxW - Win32 Debug_CF_LEVEL2"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TdxW - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Debug_OEM_STAR" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Release_OEM_STAR" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Release_BIG5" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Debug_BIG5" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Debug_OEM_STAR_HM" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Release_OEM_STAR_HM" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Debug_OEM_NEWJY" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Release_OEM_NEWJY" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Debug_LEVEL2" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Release_LEVEL2" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Release_Personal" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Debug_Personal" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Debug_CF_LEVEL2" (based on "Win32 (x86) Application")
!MESSAGE "TdxW - Win32 Release_CF_LEVEL2" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/TdxW5_New2", WPBAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TdxW - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Netapi32.lib appface.lib ssleay32.lib libeay32.lib /nologo /subsystem:windows /machine:I386 /out:"W:\code\Bi\env\100nian_bit_test.exe"
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Netapi32.lib appface.lib libeay32.lib ssleay32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"W:\code\Bi\env\100nian_bit.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_OEM_STAR"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TdxW___Win32_Debug_OEM_STAR"
# PROP BASE Intermediate_Dir "TdxW___Win32_Debug_OEM_STAR"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "TdxW___Win32_Debug_OEM_STAR"
# PROP Intermediate_Dir "TdxW___Win32_Debug_OEM_STAR"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "TCPIP_NODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "OEM_STAR" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_OEM_STAR"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TdxW___Win32_Release_OEM_STAR"
# PROP BASE Intermediate_Dir "TdxW___Win32_Release_OEM_STAR"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TdxW___Win32_Release_OEM_STAR"
# PROP Intermediate_Dir "TdxW___Win32_Release_OEM_STAR"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "TCPIP_NODE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "OEM_STAR" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_BIG5"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TdxW___Win32_Release_BIG5"
# PROP BASE Intermediate_Dir "TdxW___Win32_Release_BIG5"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TdxW___Win32_Release_BIG5"
# PROP Intermediate_Dir "TdxW___Win32_Release_BIG5"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "TCPIP_NODE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "BIG5" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_BIG5"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TdxW___Win32_Debug_BIG5"
# PROP BASE Intermediate_Dir "TdxW___Win32_Debug_BIG5"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "TdxW___Win32_Debug_BIG5"
# PROP Intermediate_Dir "TdxW___Win32_Debug_BIG5"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "TCPIP_NODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "BIG5" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_OEM_STAR_HM"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TdxW___Win32_Debug_OEM_STAR_HM"
# PROP BASE Intermediate_Dir "TdxW___Win32_Debug_OEM_STAR_HM"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "TdxW___Win32_Debug_OEM_STAR_HM"
# PROP Intermediate_Dir "TdxW___Win32_Debug_OEM_STAR_HM"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "OEM_STAR" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "OEM_STAR_HM" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_OEM_STAR_HM"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TdxW___Win32_Release_OEM_STAR_HM"
# PROP BASE Intermediate_Dir "TdxW___Win32_Release_OEM_STAR_HM"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TdxW___Win32_Release_OEM_STAR_HM"
# PROP Intermediate_Dir "TdxW___Win32_Release_OEM_STAR_HM"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "OEM_STAR" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "OEM_STAR_HM" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_OEM_NEWJY"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TdxW___Win32_Debug_OEM_NEWJY"
# PROP BASE Intermediate_Dir "TdxW___Win32_Debug_OEM_NEWJY"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "TdxW___Win32_Debug_OEM_NEWJY"
# PROP Intermediate_Dir "TdxW___Win32_Debug_OEM_NEWJY"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "OEM_NEWJY" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"c:\new_xczq\TdxW.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_OEM_NEWJY"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TdxW___Win32_Release_OEM_NEWJY"
# PROP BASE Intermediate_Dir "TdxW___Win32_Release_OEM_NEWJY"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TdxW___Win32_Release_OEM_NEWJY"
# PROP Intermediate_Dir "TdxW___Win32_Release_OEM_NEWJY"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "OEM_NEWJY" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386 /out:"C:\new_xczq\TdxW.exe"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_LEVEL2"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TdxW___Win32_Debug_LEVEL2"
# PROP BASE Intermediate_Dir "TdxW___Win32_Debug_LEVEL2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "TdxW___Win32_Debug_LEVEL2"
# PROP Intermediate_Dir "TdxW___Win32_Debug_LEVEL2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "LEVEL2" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"C:\563\TdxW.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_LEVEL2"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TdxW___Win32_Release_LEVEL2"
# PROP BASE Intermediate_Dir "TdxW___Win32_Release_LEVEL2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TdxW___Win32_Release_LEVEL2"
# PROP Intermediate_Dir "TdxW___Win32_Release_LEVEL2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "LEVEL2" /D "CITY10" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386 /out:"C:\563\KingNiu.exe"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_Personal"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TdxW___Win32_Release_Personal"
# PROP BASE Intermediate_Dir "TdxW___Win32_Release_Personal"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TdxW___Win32_Release_Personal"
# PROP Intermediate_Dir "TdxW___Win32_Release_Personal"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "PERSONAL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_Personal"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TdxW___Win32_Debug_Personal"
# PROP BASE Intermediate_Dir "TdxW___Win32_Debug_Personal"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "TdxW___Win32_Debug_Personal"
# PROP Intermediate_Dir "TdxW___Win32_Debug_Personal"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "PERSONAL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"C:\temp\new_zszq_level2_test\TdxW.exe" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"E:\onleading\li\²«ÀË¾­µä°æ1\TdxW.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_CF_LEVEL2"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TdxW___Win32_Debug_CF_LEVEL2"
# PROP BASE Intermediate_Dir "TdxW___Win32_Debug_CF_LEVEL2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "TdxW___Win32_Debug_CF_LEVEL2"
# PROP Intermediate_Dir "TdxW___Win32_Debug_CF_LEVEL2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "LEVEL2" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "LEVEL2" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"C:\tdx_level2_test\TdxW.exe" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_CF_LEVEL2"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TdxW___Win32_Release_CF_LEVEL2"
# PROP BASE Intermediate_Dir "TdxW___Win32_Release_CF_LEVEL2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TdxW___Win32_Release_CF_LEVEL2"
# PROP Intermediate_Dir "TdxW___Win32_Release_CF_LEVEL2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "LEVEL2" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "LEVEL2" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386 /out:"C:\tdx_level2_test\TdxW.exe"
# ADD LINK32 Netapi32.lib /nologo /subsystem:windows /machine:I386

!ENDIF 

# Begin Target

# Name "TdxW - Win32 Release"
# Name "TdxW - Win32 Debug"
# Name "TdxW - Win32 Debug_OEM_STAR"
# Name "TdxW - Win32 Release_OEM_STAR"
# Name "TdxW - Win32 Release_BIG5"
# Name "TdxW - Win32 Debug_BIG5"
# Name "TdxW - Win32 Debug_OEM_STAR_HM"
# Name "TdxW - Win32 Release_OEM_STAR_HM"
# Name "TdxW - Win32 Debug_OEM_NEWJY"
# Name "TdxW - Win32 Release_OEM_NEWJY"
# Name "TdxW - Win32 Debug_LEVEL2"
# Name "TdxW - Win32 Release_LEVEL2"
# Name "TdxW - Win32 Release_Personal"
# Name "TdxW - Win32 Debug_Personal"
# Name "TdxW - Win32 Debug_CF_LEVEL2"
# Name "TdxW - Win32 Release_CF_LEVEL2"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AbsWarnPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Ad.cpp
# End Source File
# Begin Source File

SOURCE=.\AdjustParamDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvHqview.cpp
# End Source File
# Begin Source File

SOURCE=.\AgileReaderDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\AgileReaderView.cpp
# End Source File
# Begin Source File

SOURCE=.\AllTestMapDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AllZSDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AreaCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AuthPassMng.cpp
# End Source File
# Begin Source File

SOURCE=.\autoupdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoupInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseWin.cpp
# End Source File
# Begin Source File

SOURCE=.\batchdlg.cpp
# ADD CPP /I "TdxW"
# End Source File
# Begin Source File

SOURCE=.\BigMenuView.cpp
# End Source File
# Begin Source File

SOURCE=.\BkData.cpp
# End Source File
# Begin Source File

SOURCE=.\BkDataDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\blockcalcdlg.cpp
# ADD CPP /I "TdxW"
# End Source File
# Begin Source File

SOURCE=.\BlockColorPage.cpp
# End Source File
# Begin Source File

SOURCE=.\BlockGpDel.cpp
# End Source File
# Begin Source File

SOURCE=.\BlockPage.cpp
# End Source File
# Begin Source File

SOURCE=.\BlockWin.cpp
# End Source File
# Begin Source File

SOURCE=.\blowfish.cpp
# End Source File
# Begin Source File

SOURCE=.\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\CalcIndexChgRsn.cpp
# End Source File
# Begin Source File

SOURCE=.\CaptionButton.cpp
# End Source File
# Begin Source File

SOURCE=.\cf.cpp
# End Source File
# Begin Source File

SOURCE=.\ChangeDomainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ChoiceBlockDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CleChoiceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\CMYButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CMYControlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CMYTabctrlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorButton.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ComDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ComView.cpp
# End Source File
# Begin Source File

SOURCE=.\comview2.cpp
# End Source File
# Begin Source File

SOURCE=.\ComView_sub.cpp
# End Source File
# Begin Source File

SOURCE=.\ConditionGPDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConditionLoadDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\constmng.cpp
# End Source File
# Begin Source File

SOURCE=.\Credits.cpp
# End Source File
# Begin Source File

SOURCE=.\CusPadSaveDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Cwcl.cpp
# End Source File
# Begin Source File

SOURCE=.\DataDigDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Dataio.cpp
# End Source File
# Begin Source File

SOURCE=.\DataSyncDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dbfile.cpp
# End Source File
# Begin Source File

SOURCE=.\DCancelOrder.cpp
# End Source File
# Begin Source File

SOURCE=.\DCjbsStat.cpp
# End Source File
# Begin Source File

SOURCE=.\DCjtjStat.cpp
# End Source File
# Begin Source File

SOURCE=.\dde.cpp
# End Source File
# Begin Source File

SOURCE=.\DDrawLineTool.cpp
# End Source File
# Begin Source File

SOURCE=.\DeepWarnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DescFormDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DescZBDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLogout.cpp
# End Source File
# Begin Source File

SOURCE=.\DownHttpFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DownloadDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DragListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DSConnectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DSZHSortDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DTotalOrder.cpp
# End Source File
# Begin Source File

SOURCE=.\DUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\EditBlockDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditConDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditEx.cpp
# End Source File
# Begin Source File

SOURCE=.\EditServerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditTxtDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExDayMng.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportExtDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\externpage.cpp
# ADD CPP /I "TdxW"
# End Source File
# Begin Source File

SOURCE=.\FastIni.cpp
# End Source File
# Begin Source File

SOURCE=.\FeedSource.cpp
# End Source File
# Begin Source File

SOURCE=.\FindExeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FindResultDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FlatButton.cpp
# End Source File
# Begin Source File

SOURCE=.\FluxCountDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FormulaPackDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Func.cpp
# End Source File
# Begin Source File

SOURCE=.\Func_comm.cpp
# End Source File
# Begin Source File

SOURCE=.\Func_ini.cpp
# End Source File
# Begin Source File

SOURCE=.\FundHoldStockDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Gdi.cpp
# End Source File
# Begin Source File

SOURCE=.\GetBlockDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Getdata.cpp
# End Source File
# Begin Source File

SOURCE=.\GetData2.cpp
# End Source File
# Begin Source File

SOURCE=.\GetData3.cpp
# End Source File
# Begin Source File

SOURCE=.\GetTxtDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalVar.cpp
# End Source File
# Begin Source File

SOURCE=.\gntree.cpp
# ADD CPP /I "TdxW"
# End Source File
# Begin Source File

SOURCE=.\GpGxdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\gptree.cpp
# End Source File
# Begin Source File

SOURCE=.\GradientProgressCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Grid.cpp
# End Source File
# Begin Source File

SOURCE=.\GridView.cpp
# End Source File
# Begin Source File

SOURCE=.\GrigHzParaDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GSDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GuideDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\hint2dlg.cpp
# ADD CPP /I "TdxW"
# End Source File
# Begin Source File

SOURCE=.\HintDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HisDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HomePageView.cpp
# End Source File
# Begin Source File

SOURCE=.\HQComm.cpp
# End Source File
# Begin Source File

SOURCE=.\hqcontrol.cpp
# End Source File
# Begin Source File

SOURCE=.\HQGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\HQGrid2.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\HToolTipEx.cpp
# End Source File
# Begin Source File

SOURCE=.\HyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\IConButton.cpp
# End Source File
# Begin Source File

SOURCE=.\IEBrowserView.cpp
# End Source File
# Begin Source File

SOURCE=.\inblockdlg.cpp
# ADD CPP /I "TdxW"
# End Source File
# Begin Source File

SOURCE=.\IndexInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\indexsortdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\info.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\InputName.cpp
# End Source File
# Begin Source File

SOURCE=.\InputName2.cpp
# End Source File
# Begin Source File

SOURCE=.\Jbfxview.cpp
# End Source File
# Begin Source File

SOURCE=.\JiangDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\JJGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\JJView.cpp
# End Source File
# Begin Source File

SOURCE=.\json_reader.cpp

!IF  "$(CFG)" == "TdxW - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_OEM_STAR"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_OEM_STAR"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_BIG5"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_BIG5"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_OEM_STAR_HM"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_OEM_STAR_HM"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_OEM_NEWJY"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_OEM_NEWJY"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_LEVEL2"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_LEVEL2"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_Personal"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_Personal"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_CF_LEVEL2"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_CF_LEVEL2"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\json_value.cpp

!IF  "$(CFG)" == "TdxW - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_OEM_STAR"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_OEM_STAR"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_BIG5"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_BIG5"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_OEM_STAR_HM"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_OEM_STAR_HM"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_OEM_NEWJY"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_OEM_NEWJY"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_LEVEL2"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_LEVEL2"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_Personal"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_Personal"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_CF_LEVEL2"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_CF_LEVEL2"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\json_writer.cpp

!IF  "$(CFG)" == "TdxW - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_OEM_STAR"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_OEM_STAR"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_BIG5"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_BIG5"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_OEM_STAR_HM"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_OEM_STAR_HM"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_OEM_NEWJY"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_OEM_NEWJY"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_LEVEL2"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_LEVEL2"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_Personal"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_Personal"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Debug_CF_LEVEL2"

!ELSEIF  "$(CFG)" == "TdxW - Win32 Release_CF_LEVEL2"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jy.cpp
# End Source File
# Begin Source File

SOURCE=.\KCLogoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyGuy.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyWordSeach.cpp
# End Source File
# Begin Source File

SOURCE=.\L2FaceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Label.cpp
# End Source File
# Begin Source File

SOURCE=.\leadup.cpp
# End Source File
# Begin Source File

SOURCE=.\levin.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkStkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ListBoxEx.cpp
# End Source File
# Begin Source File

SOURCE=.\ListCtrlSortClass.cpp
# End Source File
# Begin Source File

SOURCE=.\loaddll.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalExternMngDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalExternSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalIndexMng.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalIndexMngDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalIndexSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalPtStkMngDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalPtStkSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LogoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MarkMgn.cpp
# End Source File
# Begin Source File

SOURCE=.\MathParser.cpp
# End Source File
# Begin Source File

SOURCE=.\MathParser.h
# End Source File
# Begin Source File

SOURCE=.\MenuWin.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyAbsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MovingInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MyBitmapBt.cpp
# End Source File
# Begin Source File

SOURCE=.\MyCaptionBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MyDetailDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MyDockBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MyDockBarBase.cpp
# End Source File
# Begin Source File

SOURCE=.\MyFileFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\MyInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\MyRichEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\MyScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MySelGpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MyStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MyStrategy.cpp
# End Source File
# Begin Source File

SOURCE=.\MySwitchBar.cpp
# End Source File
# Begin Source File

SOURCE=.\myswitchbar2.cpp
# End Source File
# Begin Source File

SOURCE=.\MyToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MyTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\NewsView.cpp
# End Source File
# Begin Source File

SOURCE=.\NewUrgentDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NTHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\Other2Page.cpp
# End Source File
# Begin Source File

SOURCE=.\other3page.cpp
# End Source File
# Begin Source File

SOURCE=.\Other4Page.cpp
# End Source File
# Begin Source File

SOURCE=.\OtherPage.cpp
# End Source File
# Begin Source File

SOURCE=.\otherwarn2page.cpp
# End Source File
# Begin Source File

SOURCE=.\OtherWarnPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OTHERWTDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\PadListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamTestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PasswordDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PieChartCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Plugin_gn.cpp
# End Source File
# Begin Source File

SOURCE=.\PluginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PopPreViewTool.cpp
# End Source File
# Begin Source File

SOURCE=.\PreviewUnitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ProxyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\QJReport.cpp
# End Source File
# Begin Source File

SOURCE=.\QjStatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\QRReport.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickFlushTjDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ReceiveThread.cpp
# End Source File
# Begin Source File

SOURCE=.\recentsearchdlg.cpp
# ADD CPP /I "TdxW"
# End Source File
# Begin Source File

SOURCE=.\Registry.cpp
# End Source File
# Begin Source File

SOURCE=.\Remote.cpp
# End Source File
# Begin Source File

SOURCE=.\ReqThread.cpp
# End Source File
# Begin Source File

SOURCE=.\RMReport.cpp
# End Source File
# Begin Source File

SOURCE=.\RssView.cpp
# End Source File
# Begin Source File

SOURCE=.\SafeComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\SafeEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\SafeEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\SeatQueryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectDateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectGPDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectOneDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelfLineShowDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetCBDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetDefPadDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetQRParamDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetTqTimeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ShortCutPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowRiskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowURL.cpp
# End Source File
# Begin Source File

SOURCE=.\SimChoiceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SimOptDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SortListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeedTestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SwitcherButton.cpp
# End Source File
# Begin Source File

SOURCE=.\SwitcherWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\SwitchEx.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSetPage.cpp
# End Source File
# Begin Source File

SOURCE=.\TdxW.cpp
# End Source File
# Begin Source File

SOURCE=.\TdxW.rc
# End Source File
# Begin Source File

SOURCE=.\testdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TestYieldWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\TestZBDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TipDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TipMarkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TjWarnConfDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolTipMark.cpp
# End Source File
# Begin Source File

SOURCE=.\TQCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\treeid.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeViewEx.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\TrueColorToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\TvView.cpp
# End Source File
# Begin Source File

SOURCE=.\TxtFindDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TxtView.cpp
# End Source File
# Begin Source File

SOURCE=.\u_changepwd.cpp
# ADD CPP /I "TdxW"
# End Source File
# Begin Source File

SOURCE=.\U_RegDetial.cpp
# End Source File
# Begin Source File

SOURCE=.\u_selreg.cpp
# ADD CPP /I "TdxW"
# End Source File
# Begin Source File

SOURCE=.\u_seluser.cpp
# ADD CPP /I "TdxW"
# End Source File
# Begin Source File

SOURCE=.\u_testzjzh.cpp
# End Source File
# Begin Source File

SOURCE=.\UBase.cpp
# End Source File
# Begin Source File

SOURCE=.\UBaseCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\UBaseInner.cpp
# End Source File
# Begin Source File

SOURCE=.\UBigVol.cpp
# End Source File
# Begin Source File

SOURCE=.\UCb.cpp
# End Source File
# Begin Source File

SOURCE=.\UCdp.cpp
# End Source File
# Begin Source File

SOURCE=.\UF10.cpp
# End Source File
# Begin Source File

SOURCE=.\UF10Ctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\UF10CtrlWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\UFjb.cpp
# End Source File
# Begin Source File

SOURCE=.\UFsb.cpp
# End Source File
# Begin Source File

SOURCE=.\Ufxdline.cpp
# End Source File
# Begin Source File

SOURCE=.\Ufxdraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Ufxsub.cpp
# End Source File
# Begin Source File

SOURCE=.\UFxt.cpp
# End Source File
# Begin Source File

SOURCE=.\UGlhq.cpp
# End Source File
# Begin Source File

SOURCE=.\UGPWeb.cpp
# End Source File
# Begin Source File

SOURCE=.\UHisk.cpp
# End Source File
# Begin Source File

SOURCE=.\UHq.cpp
# End Source File
# Begin Source File

SOURCE=.\UHq10.cpp
# End Source File
# Begin Source File

SOURCE=.\UHycj.cpp
# End Source File
# Begin Source File

SOURCE=.\UKList.cpp
# End Source File
# Begin Source File

SOURCE=.\UMmp.cpp
# End Source File
# Begin Source File

SOURCE=.\UnitView.cpp
# End Source File
# Begin Source File

SOURCE=.\UnzipFile.cpp
# End Source File
# Begin Source File

SOURCE=.\UQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\URadar.cpp
# End Source File
# Begin Source File

SOURCE=.\UrgentDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UrgentListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\URLEncode.cpp
# End Source File
# Begin Source File

SOURCE=.\URmReport.cpp
# End Source File
# Begin Source File

SOURCE=.\URmReportCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\URmReportCtrlWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\USCancel.cpp
# End Source File
# Begin Source File

SOURCE=.\USeat.cpp
# End Source File
# Begin Source File

SOURCE=.\UShortGuy.cpp
# End Source File
# Begin Source File

SOURCE=.\UShowMulInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\UShowMulInfoCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\UShowMulInfoCtrlWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\USortGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\UTCancel.cpp
# End Source File
# Begin Source File

SOURCE=.\UTick.cpp
# End Source File
# Begin Source File

SOURCE=.\UTick2.cpp
# End Source File
# Begin Source File

SOURCE=.\UTickStat.cpp
# End Source File
# Begin Source File

SOURCE=.\UTickZst.cpp
# End Source File
# Begin Source File

SOURCE=.\UTVGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\UWeb.cpp
# End Source File
# Begin Source File

SOURCE=.\UWeb2.cpp
# End Source File
# Begin Source File

SOURCE=.\UWtfb.cpp
# End Source File
# Begin Source File

SOURCE=.\UWTRank.cpp
# End Source File
# Begin Source File

SOURCE=.\UZhSort.cpp
# End Source File
# Begin Source File

SOURCE=.\Uzsline.cpp
# End Source File
# Begin Source File

SOURCE=.\Uzssub.cpp
# End Source File
# Begin Source File

SOURCE=.\UZst.cpp
# End Source File
# Begin Source File

SOURCE=.\verify.cpp
# End Source File
# Begin Source File

SOURCE=.\vesselwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewManager.cpp
# End Source File
# Begin Source File

SOURCE=.\waitdlg.cpp
# ADD CPP /I "TdxW"
# End Source File
# Begin Source File

SOURCE=.\WarnGPDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\warninfopage.cpp
# End Source File
# Begin Source File

SOURCE=.\WarnManager.cpp
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.cpp
# End Source File
# Begin Source File

SOURCE=.\WebPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\webtree.cpp
# End Source File
# Begin Source File

SOURCE=.\widedatadlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteUrgent.cpp
# End Source File
# Begin Source File

SOURCE=.\WTPathSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ZBReport.cpp
# End Source File
# Begin Source File

SOURCE=.\zbtree.cpp
# End Source File
# Begin Source File

SOURCE=.\ZHSortDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ZhSortOptDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Znxg.cpp
# End Source File
# Begin Source File

SOURCE=.\ZstPlayDlg.cpp
# ADD CPP /I "TdxW"
# End Source File
# Begin Source File

SOURCE=.\ZstTickDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ZUBaseFile.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AbsWarnPage.h
# End Source File
# Begin Source File

SOURCE=.\Ad.h
# End Source File
# Begin Source File

SOURCE=.\AdjustParamDlg.h
# End Source File
# Begin Source File

SOURCE=.\AdvDlg.h
# End Source File
# Begin Source File

SOURCE=.\AdvHqview.h
# End Source File
# Begin Source File

SOURCE=.\AgileReaderDoc.h
# End Source File
# Begin Source File

SOURCE=.\AgileReaderView.h
# End Source File
# Begin Source File

SOURCE=.\AllTestMapDlg.h
# End Source File
# Begin Source File

SOURCE=.\AllZSDlg.h
# End Source File
# Begin Source File

SOURCE=.\AppFace.h
# End Source File
# Begin Source File

SOURCE=.\AreaCfgDlg.h
# End Source File
# Begin Source File

SOURCE=.\AuthPassMng.h
# End Source File
# Begin Source File

SOURCE=.\autoupdlg.h
# End Source File
# Begin Source File

SOURCE=.\AutoupInfo.h
# End Source File
# Begin Source File

SOURCE=.\BASETSD.H
# End Source File
# Begin Source File

SOURCE=.\BaseWin.h
# End Source File
# Begin Source File

SOURCE=.\batchdlg.h
# End Source File
# Begin Source File

SOURCE=.\BigMenuView.h
# End Source File
# Begin Source File

SOURCE=.\BkData.h
# End Source File
# Begin Source File

SOURCE=.\BkDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\blockcalcdlg.h
# End Source File
# Begin Source File

SOURCE=.\BlockColorPage.h
# End Source File
# Begin Source File

SOURCE=.\BlockGpDel.h
# End Source File
# Begin Source File

SOURCE=.\BlockPage.h
# End Source File
# Begin Source File

SOURCE=.\BlockWin.h
# End Source File
# Begin Source File

SOURCE=.\blowfish.h
# End Source File
# Begin Source File

SOURCE=.\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\CalcIndexChgRsn.h
# End Source File
# Begin Source File

SOURCE=.\CaptionButton.h
# End Source File
# Begin Source File

SOURCE=.\Bi\CBase64Coding.h
# End Source File
# Begin Source File

SOURCE=.\cf.h
# End Source File
# Begin Source File

SOURCE=.\ChangeDomainDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ChoiceBlockDlg.h
# End Source File
# Begin Source File

SOURCE=.\CleChoiceDlg.h
# End Source File
# Begin Source File

SOURCE=.\ClientSocket.h
# End Source File
# Begin Source File

SOURCE=.\CMLDPlugInInterface.h
# End Source File
# Begin Source File

SOURCE=.\CMYButton.h
# End Source File
# Begin Source File

SOURCE=.\CMYControlBar.h
# End Source File
# Begin Source File

SOURCE=.\CMYTabCtrlBar.h
# End Source File
# Begin Source File

SOURCE=.\ColorButton.h
# End Source File
# Begin Source File

SOURCE=.\ColorListBox.h
# End Source File
# Begin Source File

SOURCE=.\ColorPage.h
# End Source File
# Begin Source File

SOURCE=.\ComDoc.h
# End Source File
# Begin Source File

SOURCE=.\ComView.h
# End Source File
# Begin Source File

SOURCE=.\comview2.h
# End Source File
# Begin Source File

SOURCE=.\ComView_sub.h
# End Source File
# Begin Source File

SOURCE=.\ConditionGPDlg.h
# End Source File
# Begin Source File

SOURCE=.\ConditionLoadDlg.h
# End Source File
# Begin Source File

SOURCE=.\ConnectCfgDlg.h
# End Source File
# Begin Source File

SOURCE=.\constmng.h
# End Source File
# Begin Source File

SOURCE=.\Credits.h
# End Source File
# Begin Source File

SOURCE=.\CusPadSaveDlg.h
# End Source File
# Begin Source File

SOURCE=.\custompad.h
# End Source File
# Begin Source File

SOURCE=.\Cwcl.h
# End Source File
# Begin Source File

SOURCE=.\DataDigDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dataio.h
# End Source File
# Begin Source File

SOURCE=.\DataSyncDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dbf.h
# End Source File
# Begin Source File

SOURCE=.\dbfile.h
# End Source File
# Begin Source File

SOURCE=.\DCancelOrder.h
# End Source File
# Begin Source File

SOURCE=.\DCjbsStat.h
# End Source File
# Begin Source File

SOURCE=.\DCjtjStat.h
# End Source File
# Begin Source File

SOURCE=.\dde.h
# End Source File
# Begin Source File

SOURCE=.\DDraw.h
# End Source File
# Begin Source File

SOURCE=.\DDrawLineStruct.h
# End Source File
# Begin Source File

SOURCE=.\DDrawLineTool.h
# End Source File
# Begin Source File

SOURCE=.\DeepWarnDlg.h
# End Source File
# Begin Source File

SOURCE=.\DescFormDlg.h
# End Source File
# Begin Source File

SOURCE=.\DescZBDlg.h
# End Source File
# Begin Source File

SOURCE=.\DlgLogout.h
# End Source File
# Begin Source File

SOURCE=.\DownHttpFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\DownloadDlg.h
# End Source File
# Begin Source File

SOURCE=.\DragListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\DSConnectDlg.h
# End Source File
# Begin Source File

SOURCE=.\DSZHSortDlg.h
# End Source File
# Begin Source File

SOURCE=.\DTotalOrder.h
# End Source File
# Begin Source File

SOURCE=.\DUnit.h
# End Source File
# Begin Source File

SOURCE=.\EditBlockDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditConDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditEx.h
# End Source File
# Begin Source File

SOURCE=.\EditServerDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditTxtDlg.h
# End Source File
# Begin Source File

SOURCE=.\EnMarco.h
# End Source File
# Begin Source File

SOURCE=.\ExDayMng.h
# End Source File
# Begin Source File

SOURCE=.\ExportDlg.h
# End Source File
# Begin Source File

SOURCE=.\ExportExtDlg.h
# End Source File
# Begin Source File

SOURCE=.\externpage.h
# End Source File
# Begin Source File

SOURCE=.\FastIni.h
# End Source File
# Begin Source File

SOURCE=.\FeedSource.h
# End Source File
# Begin Source File

SOURCE=.\FindExeDlg.h
# End Source File
# Begin Source File

SOURCE=.\FindResultDlg.h
# End Source File
# Begin Source File

SOURCE=.\FlatButton.h
# End Source File
# Begin Source File

SOURCE=.\FluxCountDlg.h
# End Source File
# Begin Source File

SOURCE=.\FormulaPackDlg.h
# End Source File
# Begin Source File

SOURCE=.\Func.h
# End Source File
# Begin Source File

SOURCE=.\Func_comm.h
# End Source File
# Begin Source File

SOURCE=.\Func_ini.h
# End Source File
# Begin Source File

SOURCE=.\FundHoldStockDlg.h
# End Source File
# Begin Source File

SOURCE=.\Gdi.h
# End Source File
# Begin Source File

SOURCE=.\GetBlockDlg.h
# End Source File
# Begin Source File

SOURCE=.\Getdata.h
# End Source File
# Begin Source File

SOURCE=.\GetTxtDlg.h
# End Source File
# Begin Source File

SOURCE=.\GlobalVar.h
# End Source File
# Begin Source File

SOURCE=.\gntree.h
# End Source File
# Begin Source File

SOURCE=.\GpGxdDlg.h
# End Source File
# Begin Source File

SOURCE=.\gptree.h
# End Source File
# Begin Source File

SOURCE=.\GradientProgressCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Grid.h
# End Source File
# Begin Source File

SOURCE=.\GridView.h
# End Source File
# Begin Source File

SOURCE=.\GrigHzParaDlg.h
# End Source File
# Begin Source File

SOURCE=.\GSDlg.h
# End Source File
# Begin Source File

SOURCE=.\GuideDlg.h
# End Source File
# Begin Source File

SOURCE=.\hint2dlg.h
# End Source File
# Begin Source File

SOURCE=.\HintDlg.h
# End Source File
# Begin Source File

SOURCE=.\HisDlg.h
# End Source File
# Begin Source File

SOURCE=.\HomePageView.h
# End Source File
# Begin Source File

SOURCE=.\HQComm.h
# End Source File
# Begin Source File

SOURCE=.\hqcontrol.h
# End Source File
# Begin Source File

SOURCE=.\HQGrid.h
# End Source File
# Begin Source File

SOURCE=.\HtmlCtrl.h
# End Source File
# Begin Source File

SOURCE=.\HToolTipEx.h
# End Source File
# Begin Source File

SOURCE=.\HyperLink.h
# End Source File
# Begin Source File

SOURCE=.\IConButton.h
# End Source File
# Begin Source File

SOURCE=.\IEBrowserView.h
# End Source File
# Begin Source File

SOURCE=.\inblockdlg.h
# End Source File
# Begin Source File

SOURCE=.\IndexInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\indexsortdlg.h
# End Source File
# Begin Source File

SOURCE=.\info.h
# End Source File
# Begin Source File

SOURCE=.\InfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\InputName.h
# End Source File
# Begin Source File

SOURCE=.\InputName2.h
# End Source File
# Begin Source File

SOURCE=.\Interface.h
# End Source File
# Begin Source File

SOURCE=.\Jbfxview.h
# End Source File
# Begin Source File

SOURCE=.\JiangDlg.h
# End Source File
# Begin Source File

SOURCE=.\JJGrid.h
# End Source File
# Begin Source File

SOURCE=.\JJView.h
# End Source File
# Begin Source File

SOURCE=.\json_batchallocator.h
# End Source File
# Begin Source File

SOURCE=.\jy.h
# End Source File
# Begin Source File

SOURCE=.\KCLogoDlg.h
# End Source File
# Begin Source File

SOURCE=.\KeyGuy.h
# End Source File
# Begin Source File

SOURCE=.\KeyWordSeach.h
# End Source File
# Begin Source File

SOURCE=.\L2FaceDlg.h
# End Source File
# Begin Source File

SOURCE=.\Label.h
# End Source File
# Begin Source File

SOURCE=.\leadup.h
# End Source File
# Begin Source File

SOURCE=.\level2.h
# End Source File
# Begin Source File

SOURCE=.\levin.h
# End Source File
# Begin Source File

SOURCE=.\LinkStkDlg.h
# End Source File
# Begin Source File

SOURCE=.\ListBoxEx.h
# End Source File
# Begin Source File

SOURCE=.\ListCtrlSortClass.h
# End Source File
# Begin Source File

SOURCE=.\loaddll.h
# End Source File
# Begin Source File

SOURCE=.\LocalExternMngDlg.h
# End Source File
# Begin Source File

SOURCE=.\LocalExternSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\LocalIndexMng.h
# End Source File
# Begin Source File

SOURCE=.\LocalIndexMngDlg.h
# End Source File
# Begin Source File

SOURCE=.\LocalIndexSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\LocalPtStkMngDlg.h
# End Source File
# Begin Source File

SOURCE=.\LocalPtStkSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\LogoDlg.h
# End Source File
# Begin Source File

SOURCE=.\Macro.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MarkMgn.h
# End Source File
# Begin Source File

SOURCE=.\Memdc.h
# End Source File
# Begin Source File

SOURCE=.\MenuWin.h
# End Source File
# Begin Source File

SOURCE=.\ModifyAbsDlg.h
# End Source File
# Begin Source File

SOURCE=.\MovingInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\MsgDlg.h
# End Source File
# Begin Source File

SOURCE=.\MyBitmapBt.h
# End Source File
# Begin Source File

SOURCE=.\MyCaptionBar.h
# End Source File
# Begin Source File

SOURCE=.\MyDetailDlg.h
# End Source File
# Begin Source File

SOURCE=.\MyDockBar.h
# End Source File
# Begin Source File

SOURCE=.\mydockbarbase.h
# End Source File
# Begin Source File

SOURCE=.\MyFileFunc.h
# End Source File
# Begin Source File

SOURCE=.\MyInterface.h
# End Source File
# Begin Source File

SOURCE=.\MyRichEdit.h
# End Source File
# Begin Source File

SOURCE=.\MyScrollBar.h
# End Source File
# Begin Source File

SOURCE=.\MySelGpDlg.h
# End Source File
# Begin Source File

SOURCE=.\MyStatusBar.h
# End Source File
# Begin Source File

SOURCE=.\MyStrategy.h
# End Source File
# Begin Source File

SOURCE=.\MySwitchBar.h
# End Source File
# Begin Source File

SOURCE=.\myswitchbar2.h
# End Source File
# Begin Source File

SOURCE=.\MyToolBar.h
# End Source File
# Begin Source File

SOURCE=.\MyTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\NewsView.h
# End Source File
# Begin Source File

SOURCE=.\NewUrgentDlg.h
# End Source File
# Begin Source File

SOURCE=.\NTHelper.h
# End Source File
# Begin Source File

SOURCE=.\Other2Page.h
# End Source File
# Begin Source File

SOURCE=.\other3page.h
# End Source File
# Begin Source File

SOURCE=.\Other4Page.h
# End Source File
# Begin Source File

SOURCE=.\OtherPage.h
# End Source File
# Begin Source File

SOURCE=.\otherwarn2page.h
# End Source File
# Begin Source File

SOURCE=.\OtherWarnPage.h
# End Source File
# Begin Source File

SOURCE=.\OTHERWTDLG.h
# End Source File
# Begin Source File

SOURCE=.\OutStruct.h
# End Source File
# Begin Source File

SOURCE=.\PadListDlg.h
# End Source File
# Begin Source File

SOURCE=.\ParamTestDlg.h
# End Source File
# Begin Source File

SOURCE=.\PasswordDlg.h
# End Source File
# Begin Source File

SOURCE=.\PieChartCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Plugin.h
# End Source File
# Begin Source File

SOURCE=.\Plugin_gn.h
# End Source File
# Begin Source File

SOURCE=.\PluginDlg.h
# End Source File
# Begin Source File

SOURCE=.\PopPreViewTool.h
# End Source File
# Begin Source File

SOURCE=.\PreviewUnitDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProgressWnd.h
# End Source File
# Begin Source File

SOURCE=.\Protocol.h
# End Source File
# Begin Source File

SOURCE=.\protocol3.h
# End Source File
# Begin Source File

SOURCE=.\ProxyDlg.h
# End Source File
# Begin Source File

SOURCE=.\QJReport.h
# End Source File
# Begin Source File

SOURCE=.\QjStatDlg.h
# End Source File
# Begin Source File

SOURCE=.\QRReport.h
# End Source File
# Begin Source File

SOURCE=.\QuickFlushTjDlg.h
# End Source File
# Begin Source File

SOURCE=.\ReceiveThread.h
# End Source File
# Begin Source File

SOURCE=.\recentsearchdlg.h
# End Source File
# Begin Source File

SOURCE=.\Registry.h
# End Source File
# Begin Source File

SOURCE=.\Remote.h
# End Source File
# Begin Source File

SOURCE=.\ReqThread.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RMReport.h
# End Source File
# Begin Source File

SOURCE=.\RssView.h
# End Source File
# Begin Source File

SOURCE=.\SafeComboBox.h
# End Source File
# Begin Source File

SOURCE=.\SafeEdit.h
# End Source File
# Begin Source File

SOURCE=.\SafeEngine.h
# End Source File
# Begin Source File

SOURCE=.\SeatQueryDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelectDateDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelectGPDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelectOneDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelfLineShowDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetCBDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetDefPadDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetQRParamDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetTqTimeDlg.h
# End Source File
# Begin Source File

SOURCE=.\ShortCutPage.h
# End Source File
# Begin Source File

SOURCE=.\ShowInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\ShowRiskDlg.h
# End Source File
# Begin Source File

SOURCE=.\ShowURL.h
# End Source File
# Begin Source File

SOURCE=.\SimChoiceDlg.h
# End Source File
# Begin Source File

SOURCE=.\SimOptDlg.h
# End Source File
# Begin Source File

SOURCE=.\SortListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SpeedTestDlg.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\StatusWnd.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Struct.h
# End Source File
# Begin Source File

SOURCE=.\SwitcherButton.h
# End Source File
# Begin Source File

SOURCE=.\SwitcherWnd.h
# End Source File
# Begin Source File

SOURCE=.\SwitchEx.h
# End Source File
# Begin Source File

SOURCE=.\TabSetPage.h
# End Source File
# Begin Source File

SOURCE=.\TdxW.h
# End Source File
# Begin Source File

SOURCE=.\testdlg.h
# End Source File
# Begin Source File

SOURCE=.\TestYieldWnd.h
# End Source File
# Begin Source File

SOURCE=.\TestZBDlg.h
# End Source File
# Begin Source File

SOURCE=.\TipDlg.h
# End Source File
# Begin Source File

SOURCE=.\TipMarkDlg.h
# End Source File
# Begin Source File

SOURCE=.\TjWarnConfDlg.h
# End Source File
# Begin Source File

SOURCE=.\ToolTipMark.h
# End Source File
# Begin Source File

SOURCE=.\TQCfgDlg.h
# End Source File
# Begin Source File

SOURCE=.\treeid.h
# End Source File
# Begin Source File

SOURCE=.\TreeViewEx.h
# End Source File
# Begin Source File

SOURCE=.\TreeWnd.h
# End Source File
# Begin Source File

SOURCE=.\TrueColorToolBar.h
# End Source File
# Begin Source File

SOURCE=.\TvView.h
# End Source File
# Begin Source File

SOURCE=.\TxtFindDlg.h
# End Source File
# Begin Source File

SOURCE=.\TxtView.h
# End Source File
# Begin Source File

SOURCE=.\u_changepwd.h
# End Source File
# Begin Source File

SOURCE=.\U_RegDetial.h
# End Source File
# Begin Source File

SOURCE=.\u_selreg.h
# End Source File
# Begin Source File

SOURCE=.\u_seluser.h
# End Source File
# Begin Source File

SOURCE=.\u_testzjzh.h
# End Source File
# Begin Source File

SOURCE=.\UBase.h
# End Source File
# Begin Source File

SOURCE=.\UBaseCtrl.h
# End Source File
# Begin Source File

SOURCE=.\UBaseInner.h
# End Source File
# Begin Source File

SOURCE=.\UBigVol.h
# End Source File
# Begin Source File

SOURCE=.\UCb.h
# End Source File
# Begin Source File

SOURCE=.\UCdp.h
# End Source File
# Begin Source File

SOURCE=.\UF10.h
# End Source File
# Begin Source File

SOURCE=.\UF10Ctrl.h
# End Source File
# Begin Source File

SOURCE=.\UF10CtrlWnd.h
# End Source File
# Begin Source File

SOURCE=.\UFjb.h
# End Source File
# Begin Source File

SOURCE=.\UFsb.h
# End Source File
# Begin Source File

SOURCE=.\UFxt.h
# End Source File
# Begin Source File

SOURCE=.\UGlhq.h
# End Source File
# Begin Source File

SOURCE=.\UGPWeb.h
# End Source File
# Begin Source File

SOURCE=.\UHisk.h
# End Source File
# Begin Source File

SOURCE=.\UHq.h
# End Source File
# Begin Source File

SOURCE=.\UHq10.h
# End Source File
# Begin Source File

SOURCE=.\UHycj.h
# End Source File
# Begin Source File

SOURCE=.\UKList.h
# End Source File
# Begin Source File

SOURCE=.\UMmp.h
# End Source File
# Begin Source File

SOURCE=.\UnitView.h
# End Source File
# Begin Source File

SOURCE=.\UnzipFile.h
# End Source File
# Begin Source File

SOURCE=.\UQueue.h
# End Source File
# Begin Source File

SOURCE=.\URadar.h
# End Source File
# Begin Source File

SOURCE=.\UrgentDlg.h
# End Source File
# Begin Source File

SOURCE=.\UrgentListDlg.h
# End Source File
# Begin Source File

SOURCE=.\URLEncode.h
# End Source File
# Begin Source File

SOURCE=.\URmReport.h
# End Source File
# Begin Source File

SOURCE=.\URmReportCtrl.h
# End Source File
# Begin Source File

SOURCE=.\URmReportCtrlWnd.h
# End Source File
# Begin Source File

SOURCE=.\USCancel.h
# End Source File
# Begin Source File

SOURCE=.\USeat.h
# End Source File
# Begin Source File

SOURCE=.\UShortGuy.h
# End Source File
# Begin Source File

SOURCE=.\UShowMulInfo.h
# End Source File
# Begin Source File

SOURCE=.\UShowMulInfoCtrl.h
# End Source File
# Begin Source File

SOURCE=.\UShowMulInfoCtrlWnd.h
# End Source File
# Begin Source File

SOURCE=.\USortGrid.h
# End Source File
# Begin Source File

SOURCE=.\UTCancel.h
# End Source File
# Begin Source File

SOURCE=.\UTick.h
# End Source File
# Begin Source File

SOURCE=.\UTick2.h
# End Source File
# Begin Source File

SOURCE=.\UTickStat.h
# End Source File
# Begin Source File

SOURCE=.\UTickZst.h
# End Source File
# Begin Source File

SOURCE=.\UTVGraph.h
# End Source File
# Begin Source File

SOURCE=.\UWeb.h
# End Source File
# Begin Source File

SOURCE=.\UWeb2.h
# End Source File
# Begin Source File

SOURCE=.\UWtfb.h
# End Source File
# Begin Source File

SOURCE=.\UWTRank.h
# End Source File
# Begin Source File

SOURCE=.\UZhSort.h
# End Source File
# Begin Source File

SOURCE=.\UZst.h
# End Source File
# Begin Source File

SOURCE=.\verify.h
# End Source File
# Begin Source File

SOURCE=.\vesselwnd.h
# End Source File
# Begin Source File

SOURCE=.\VFClient.h
# End Source File
# Begin Source File

SOURCE=.\VGear.h
# End Source File
# Begin Source File

SOURCE=.\ViewManager.h
# End Source File
# Begin Source File

SOURCE=.\waitdlg.h
# End Source File
# Begin Source File

SOURCE=.\WarnGPDlg.h
# End Source File
# Begin Source File

SOURCE=.\warninfopage.h
# End Source File
# Begin Source File

SOURCE=.\WarnManager.h
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.h
# End Source File
# Begin Source File

SOURCE=.\WebPageDlg.h
# End Source File
# Begin Source File

SOURCE=.\webtree.h
# End Source File
# Begin Source File

SOURCE=.\WideDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\WriteUrgent.h
# End Source File
# Begin Source File

SOURCE=.\WTPathSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\xgsys.h
# End Source File
# Begin Source File

SOURCE=.\ZBReport.h
# End Source File
# Begin Source File

SOURCE=.\zbtree.h
# End Source File
# Begin Source File

SOURCE=.\zconf.h
# End Source File
# Begin Source File

SOURCE=.\ZHSortDlg.h
# End Source File
# Begin Source File

SOURCE=.\ZhSortOptDlg.h
# End Source File
# Begin Source File

SOURCE=.\ZipException.h
# End Source File
# Begin Source File

SOURCE=.\zlib.h
# End Source File
# Begin Source File

SOURCE=.\Znxg.h
# End Source File
# Begin Source File

SOURCE=.\ZstPlayDlg.h
# End Source File
# Begin Source File

SOURCE=.\ZstTickDlg.h
# End Source File
# Begin Source File

SOURCE=.\ZUBaseFile.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\2001.cur
# End Source File
# Begin Source File

SOURCE=.\res\2003.cur
# End Source File
# Begin Source File

SOURCE=.\res\32645.cur
# End Source File
# Begin Source File

SOURCE=.\res\32766.cur
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\apple.ico
# End Source File
# Begin Source File

SOURCE=.\res\barrier.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\bat1.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\bat2.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\bat3.ico
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\bk.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\bk_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\bk_d_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\bk_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\bk_n_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\bk_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\bk_o_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\block.ico
# End Source File
# Begin Source File

SOURCE=.\res\block2.ico
# End Source File
# Begin Source File

SOURCE=.\res\block3.ico
# End Source File
# Begin Source File

SOURCE=.\res\block_se.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\BLUE_.ICO
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Book01a.ico
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\bt_h_left.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\bt_h_right.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\bt_v_down.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\bt_v_up.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Calendar.ico
# End Source File
# Begin Source File

SOURCE=.\res\candrop_.cur
# End Source File
# Begin Source File

SOURCE=.\res\captionbar_gdzq.bmp
# End Source File
# Begin Source File

SOURCE=.\res\captionbar_zxzq.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CaptionButton_ZH.bmp
# End Source File
# Begin Source File

SOURCE=.\res\controlbar_closebt_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\controlbar_closebt_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\controlbar_closebt_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\cup.ico
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00005.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00006.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur164.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur186.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur187.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor3.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor4.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor5.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor6.cur
# End Source File
# Begin Source File

SOURCE=.\res\deepwarn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\deepwarn.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\direct.ico
# End Source File
# Begin Source File

SOURCE=.\res\down_arr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down_red.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\downhand.ico
# End Source File
# Begin Source File

SOURCE=.\res\download.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\drawicon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\drawicon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\drawicon3.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\drawicon4.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\drawicon5.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\drawicon6.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\drawicon7.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\drawicon8.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\drawicon9.ico
# End Source File
# Begin Source File

SOURCE=.\res\ds.ico
# End Source File
# Begin Source File

SOURCE=.\res\dynazoom.cur
# End Source File
# Begin Source File

SOURCE=.\res\exit_foc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\exit_nor.bmp
# End Source File
# Begin Source File

SOURCE=.\res\exp_down.ico
# End Source File
# Begin Source File

SOURCE=.\res\exp_up.ico
# End Source File
# Begin Source File

SOURCE=.\res\exp_up_s.ico
# End Source File
# Begin Source File

SOURCE=.\res\find.ico
# End Source File
# Begin Source File

SOURCE=.\res\finded.ico
# End Source File
# Begin Source File

SOURCE=.\res\flush.ico
# End Source File
# Begin Source File

SOURCE=.\res\gg.ico
# End Source File
# Begin Source File

SOURCE=.\res\ggbw.ico
# End Source File
# Begin Source File

SOURCE=.\res\gninfo.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\go.ico
# End Source File
# Begin Source File

SOURCE=.\res\gp.ico
# End Source File
# Begin Source File

SOURCE=.\res\grayinfo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\green.ICO
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\GREEN_.ICO
# End Source File
# Begin Source File

SOURCE=.\res\greendow.ico
# End Source File
# Begin Source File

SOURCE=.\res\greenup.ico
# End Source File
# Begin Source File

SOURCE=.\res\gtjaxp.ico
# End Source File
# Begin Source File

SOURCE=.\res\hand.ico
# End Source File
# Begin Source File

SOURCE=.\res\header.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\home.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\home2.ico
# End Source File
# Begin Source File

SOURCE=.\res\horsplit.cur
# End Source File
# Begin Source File

SOURCE=.\res\hudie.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00007.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00009.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00010.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00011.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00012.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00013.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00014.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00015.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00016.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00017.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00019.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00020.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00021.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00022.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00024.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico130.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon01.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon02.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon10.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon11.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon12.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon6.ico
# End Source File
# Begin Source File

SOURCE=.\res\ie.bmp
# End Source File
# Begin Source File

SOURCE=.\res\imagelist_grid.bmp
# End Source File
# Begin Source File

SOURCE=.\res\imagelist_hq.bmp
# End Source File
# Begin Source File

SOURCE=.\res\imagelist_levin.bmp
# End Source File
# Begin Source File

SOURCE=.\res\imagelist_qjtj.bmp
# End Source File
# Begin Source File

SOURCE=.\res\imagelist_tree.bmp
# End Source File
# Begin Source File

SOURCE=.\res\imagelist_tv.bmp
# End Source File
# Begin Source File

SOURCE=.\res\index2.ico
# End Source File
# Begin Source File

SOURCE=.\res\info.ico
# End Source File
# Begin Source File

SOURCE=.\res\info2.ico
# End Source File
# Begin Source File

SOURCE=.\res\info_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\info_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\info_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\info_pop.ico
# End Source File
# Begin Source File

SOURCE=.\res\infotitle.bmp
# End Source File
# Begin Source File

SOURCE=.\res\infourl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\jb1.ico
# End Source File
# Begin Source File

SOURCE=.\res\jb2.ico
# End Source File
# Begin Source File

SOURCE=.\res\jj.ico
# End Source File
# Begin Source File

SOURCE=.\res\key.ico
# End Source File
# Begin Source File

SOURCE=.\res\keyguy.bmp
# End Source File
# Begin Source File

SOURCE=.\res\klinesho.ico
# End Source File
# Begin Source File

SOURCE=.\res\l2face.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\l_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\l_d_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\l_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\l_n_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\l_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\l_o_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\level_gr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\level_re.bmp
# End Source File
# Begin Source File

SOURCE=.\res\levin.ico
# End Source File
# Begin Source File

SOURCE=.\res\levinbuy.ico
# End Source File
# Begin Source File

SOURCE=.\res\levinsell.ico
# End Source File
# Begin Source File

SOURCE=.\res\Lighton.ico
# End Source File
# Begin Source File

SOURCE=.\res\linetool.bmp
# End Source File
# Begin Source File

SOURCE=.\res\linetool_gann.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\lion1.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\lion2.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\lion3.ico
# End Source File
# Begin Source File

SOURCE=.\res\logo1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LOGO10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\main.ico
# End Source File
# Begin Source File

SOURCE=.\res\max.ico
# End Source File
# Begin Source File

SOURCE=.\res\max_focu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\max_norm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mima.ico
# End Source File
# Begin Source File

SOURCE=.\res\min_focu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\min_norm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\mobile.ico
# End Source File
# Begin Source File

SOURCE=.\res\mthunder.ico
# End Source File
# Begin Source File

SOURCE=.\res\myicon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\myicon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\mythunde.ico
# End Source File
# Begin Source File

SOURCE=.\res\newd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\newn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\oem.bmp
# End Source File
# Begin Source File

SOURCE=.\res\oem1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\oem2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\oem3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\oem4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\orange.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\ORANGE_.ICO
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\pack.ico
# End Source File
# Begin Source File

SOURCE=.\res\pack.ico
# End Source File
# Begin Source File

SOURCE=.\res\pad.ico
# End Source File
# Begin Source File

SOURCE=.\res\pad2.ico
# End Source File
# Begin Source File

SOURCE=.\res\pbuy0.ico
# End Source File
# Begin Source File

SOURCE=.\res\pbuy1.ico
# End Source File
# Begin Source File

SOURCE=.\res\pbuy2.ico
# End Source File
# Begin Source File

SOURCE=.\res\pbuy3.ico
# End Source File
# Begin Source File

SOURCE=.\res\pdown.ico
# End Source File
# Begin Source File

SOURCE=.\res\Pencil.cur
# End Source File
# Begin Source File

SOURCE=.\res\person.ico
# End Source File
# Begin Source File

SOURCE=.\res\pertool.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pertoold.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\PINK_.ICO
# End Source File
# Begin Source File

SOURCE=.\res\plugin.ico
# End Source File
# Begin Source File

SOURCE=.\res\PointHand.ico
# End Source File
# Begin Source File

SOURCE=.\res\pressb.cur
# End Source File
# Begin Source File

SOURCE=.\res\presss.cur
# End Source File
# Begin Source File

SOURCE=.\res\psell1.ico
# End Source File
# Begin Source File

SOURCE=.\res\psell2.ico
# End Source File
# Begin Source File

SOURCE=.\res\pu1.ico
# End Source File
# Begin Source File

SOURCE=.\res\pu2.ico
# End Source File
# Begin Source File

SOURCE=.\res\pu3.ico
# End Source File
# Begin Source File

SOURCE=.\res\pu6.ico
# End Source File
# Begin Source File

SOURCE=.\res\pu7.ico
# End Source File
# Begin Source File

SOURCE=.\res\Quest.ico
# End Source File
# Begin Source File

SOURCE=.\res\Question.ico
# End Source File
# Begin Source File

SOURCE=.\res\quit.ico
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\r_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\r_d_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\r_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\r_n_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\r_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_myswitchbar\r_o_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\radar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\radar2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ReaderImage.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\red.ico
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\redcross.ICO
# End Source File
# Begin Source File

SOURCE=.\res\redup.ico
# End Source File
# Begin Source File

SOURCE=.\res\redup1.ico
# End Source File
# Begin Source File

SOURCE=.\res\res_focu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\res_norm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\restore.ico
# End Source File
# Begin Source File

SOURCE=.\res\shzq.ico
# End Source File
# Begin Source File

SOURCE=.\res\sjsmainf.bmp
# End Source File
# Begin Source File

SOURCE=.\res\splash_i.BMP
# End Source File
# Begin Source File

SOURCE=.\res\splash_ii.bmp
# End Source File
# Begin Source File

SOURCE=.\res\starhm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\starmain.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sysbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sysbar_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sysbar_hot.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sysbt_close_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sysbt_close_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sysbt_close_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TdxW.rc2
# End Source File
# Begin Source File

SOURCE=.\res\toold.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toold_1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toold_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toold_3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toold_4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tooln.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tooln_1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tooln_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tooln_3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tooln_4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Tools.ico
# End Source File
# Begin Source File

SOURCE=.\res\tosmall_.cur
# End Source File
# Begin Source File

SOURCE=.\res\type_advhq.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_bigmenu.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_com.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_extern.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_grid.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_his.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_homepage.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_jbfx.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_jj.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_news.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_tv.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_txt.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_web.ico
# End Source File
# Begin Source File

SOURCE=.\res\up.ico
# End Source File
# Begin Source File

SOURCE=.\res\up_arrow.bmp
# End Source File
# Begin Source File

SOURCE=.\res\up_green.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drawicons\uphand.ico
# End Source File
# Begin Source File

SOURCE=.\res\versplit.cur
# End Source File
# Begin Source File

SOURCE=.\res\vsatgs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\W95mbx01.ico
# End Source File
# Begin Source File

SOURCE=.\res\W95MBX02.ICO
# End Source File
# Begin Source File

SOURCE=.\res\W95MBX04.ICO
# End Source File
# Begin Source File

SOURCE=.\res\warn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\warn.ico
# End Source File
# Begin Source File

SOURCE=.\res\warn2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\warntjco.ico
# End Source File
# Begin Source File

SOURCE=.\res\web_leaf.ico
# End Source File
# Begin Source File

SOURCE=.\res\xhmainf.bmp
# End Source File
# Begin Source File

SOURCE=.\res\yhzq.ico
# End Source File
# Begin Source File

SOURCE=.\res\zjcf.bmp
# End Source File
# Begin Source File

SOURCE=.\res\zjcfd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\zszqflush.ico
# End Source File
# Begin Source File

SOURCE=.\res\zx1.ico
# End Source File
# Begin Source File

SOURCE=.\res\zx2.ico
# End Source File
# Begin Source File

SOURCE=.\res\zx3.ico
# End Source File
# Begin Source File

SOURCE=.\res\zx_dayngg.ico
# End Source File
# Begin Source File

SOURCE=.\res\zx_gg.ico
# End Source File
# Begin Source File

SOURCE=.\res\zx_user1.ico
# End Source File
# Begin Source File

SOURCE=.\res\zx_user2.ico
# End Source File
# Begin Source File

SOURCE=.\res\zx_user3.ico
# End Source File
# Begin Source File

SOURCE=.\res\zx_user4.ico
# End Source File
# Begin Source File

SOURCE=.\res\zx_web.ico
# End Source File
# Begin Source File

SOURCE=.\res\zxfolder.ico
# End Source File
# Begin Source File

SOURCE=.\res\zxfolder2.ico
# End Source File
# Begin Source File

SOURCE=.\res\zxjt.ico
# End Source File
# End Group
# Begin Group "Bi Source"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\Bi\algo_hmac.cpp
# End Source File
# Begin Source File

SOURCE=.\Bi\CBase64Coding.cpp
# End Source File
# Begin Source File

SOURCE=.\Bi\CoopCache.cpp
# End Source File
# Begin Source File

SOURCE=.\Bi\HideNetWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Bi\HideNetWndClass.cpp
# End Source File
# Begin Source File

SOURCE=.\Bi\HostAHq.cpp
# End Source File
# Begin Source File

SOURCE=.\Bi\HqSource.cpp
# End Source File
# Begin Source File

SOURCE=.\Bi\HqSrcMgn.cpp
# End Source File
# Begin Source File

SOURCE=.\Bi\OneCalc.cpp
# End Source File
# Begin Source File

SOURCE=.\Bi\PoloTrade.cpp
# End Source File
# End Group
# Begin Group "Bi Header"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\Bi\algo_hmac.h
# End Source File
# Begin Source File

SOURCE=.\Bi\CoopCache.h
# End Source File
# Begin Source File

SOURCE=.\Bi\HideNetWnd.h
# End Source File
# Begin Source File

SOURCE=.\Bi\HideNetWndClass.h
# End Source File
# Begin Source File

SOURCE=.\Bi\HostAHq.h
# End Source File
# Begin Source File

SOURCE=.\Bi\HqSource.h
# End Source File
# Begin Source File

SOURCE=.\Bi\HqSrcMgn.h
# End Source File
# Begin Source File

SOURCE=.\Bi\IDefine.h
# End Source File
# Begin Source File

SOURCE=.\Bi\OneCalc.h
# End Source File
# Begin Source File

SOURCE=.\Bi\PoloTrade.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\newskin.urf
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# Begin Source File

SOURCE=.\Dbf.lib
# End Source File
# Begin Source File

SOURCE=.\zlib.lib
# End Source File
# Begin Source File

SOURCE=.\TopView.lib
# End Source File
# Begin Source File

SOURCE=.\DGear.lib
# End Source File
# End Target
# End Project
# Section TdxW : {D30C1661-CDAF-11D0-8A3E-00C04FC9E26E}
# 	2:5:Class:CWebBrowser2
# 	2:10:HeaderFile:webbrowser2.h
# 	2:8:ImplFile:webbrowser2.cpp
# End Section
# Section TdxW : {72ADFD54-2C39-11D0-9903-00A0C91BC942}
# 	1:17:CG_IDS_DIDYOUKNOW:184
# 	1:22:CG_IDS_TIPOFTHEDAYMENU:183
# 	1:18:CG_IDS_TIPOFTHEDAY:182
# 	1:22:CG_IDS_TIPOFTHEDAYHELP:187
# 	1:19:CG_IDP_FILE_CORRUPT:186
# 	1:7:IDD_TIP:177
# 	1:13:IDB_LIGHTBULB:176
# 	1:18:CG_IDS_FILE_ABSENT:185
# 	2:17:CG_IDS_DIDYOUKNOW:CG_IDS_DIDYOUKNOW
# 	2:7:CTipDlg:CTipDlg
# 	2:22:CG_IDS_TIPOFTHEDAYMENU:CG_IDS_TIPOFTHEDAYMENU
# 	2:18:CG_IDS_TIPOFTHEDAY:CG_IDS_TIPOFTHEDAY
# 	2:12:CTIP_Written:OK
# 	2:22:CG_IDS_TIPOFTHEDAYHELP:CG_IDS_TIPOFTHEDAYHELP
# 	2:2:BH:
# 	2:19:CG_IDP_FILE_CORRUPT:CG_IDP_FILE_CORRUPT
# 	2:7:IDD_TIP:IDD_TIP
# 	2:8:TipDlg.h:TipDlg.h
# 	2:13:IDB_LIGHTBULB:IDB_LIGHTBULB
# 	2:18:CG_IDS_FILE_ABSENT:CG_IDS_FILE_ABSENT
# 	2:10:TipDlg.cpp:TipDlg.cpp
# End Section
# Section TdxW : {8856F961-340A-11D0-A96B-00C04FD705A2}
# 	2:21:DefaultSinkHeaderFile:webbrowser2.h
# 	2:16:DefaultSinkClass:CWebBrowser2
# End Section

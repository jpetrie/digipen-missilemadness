# Microsoft Developer Studio Project File - Name="tank" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=tank - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tank.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tank.mak" CFG="tank - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tank - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "tank - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tank - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Gi- /GR- /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FAcs /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ws2_32.lib winmm.lib fmodvc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /map /debug /debugtype:cv /machine:I386

!ELSEIF  "$(CFG)" == "tank - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib fmodvc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "tank - Win32 Release"
# Name "tank - Win32 Debug"
# Begin Group "Main"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\tank.cpp
# End Source File
# Begin Source File

SOURCE=.\tank.h
# End Source File
# Begin Source File

SOURCE=.\tank.rc
# End Source File
# Begin Source File

SOURCE=.\utilities.cpp
# End Source File
# Begin Source File

SOURCE=.\utilities.h
# End Source File
# End Group
# Begin Group "Debugging"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\debugging.cpp
# End Source File
# Begin Source File

SOURCE=.\debugging.h
# End Source File
# End Group
# Begin Group "Modules"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\fmod.h
# End Source File
# Begin Source File

SOURCE=.\mod_ai.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_ai.h
# End Source File
# Begin Source File

SOURCE=.\mod_display.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_display.h
# End Source File
# Begin Source File

SOURCE=.\mod_econ.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_econ.h
# End Source File
# Begin Source File

SOURCE=.\mod_game.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_game.h
# End Source File
# Begin Source File

SOURCE=.\mod_items.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_items.h
# End Source File
# Begin Source File

SOURCE=.\mod_missiles.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_missiles.h
# End Source File
# Begin Source File

SOURCE=.\mod_network.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_network.h
# End Source File
# Begin Source File

SOURCE=.\mod_sound.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_sound.h
# End Source File
# Begin Source File

SOURCE=.\mod_tank.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_tank.h
# End Source File
# Begin Source File

SOURCE=.\mod_terrain.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_terrain.h
# End Source File
# Begin Source File

SOURCE=.\mod_ui.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_ui.h
# End Source File
# Begin Source File

SOURCE=.\mod_weapon_attributes.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_weapon_attributes.h
# End Source File
# Begin Source File

SOURCE=.\mod_weapons.cpp
# End Source File
# End Group
# Begin Group "Handlers"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\handler_network.cpp
# End Source File
# Begin Source File

SOURCE=.\handler_network.h
# End Source File
# Begin Source File

SOURCE=.\handler_redraw.cpp
# End Source File
# Begin Source File

SOURCE=.\handler_redraw.h
# End Source File
# Begin Source File

SOURCE=.\handler_ui.cpp
# End Source File
# Begin Source File

SOURCE=.\handler_ui.h
# End Source File
# Begin Source File

SOURCE=.\mod_weapons.h
# End Source File
# End Group
# Begin Group "Error Handling"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\error_handling.cpp
# End Source File
# Begin Source File

SOURCE=.\error_handling.h
# End Source File
# End Group
# End Target
# End Project

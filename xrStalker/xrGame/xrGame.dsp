# Microsoft Developer Studio Project File - Name="xrGame" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xrGame - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xrGame.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xrGame.mak" CFG="xrGame - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xrGame - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xrGame - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xrGame - Win32 Mixed" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/xr_3da/xrGame", LSDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xrGame - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "x:\game\"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XRGAME_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /Gz /MT /W3 /GR /GX /Zi /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XRGAME_EXPORTS" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib x:\game\xr_3da.lib /nologo /dll /profile /map:"x:\game\xrGame.map" /debug /debugtype:both /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "xrGame - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "x:\game\"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XRGAME_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gz /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XRGAME_EXPORTS" /D "DEBUG" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib x:\game\xr_3da.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "xrGame - Win32 Mixed"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xrGame___Win32_Mixed"
# PROP BASE Intermediate_Dir "xrGame___Win32_Mixed"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "x:\game\"
# PROP Intermediate_Dir "Mixed"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /Gr /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XRGAME_EXPORTS" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G6 /Gz /MT /W3 /GR /GX /Zi /O2 /Oy- /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XRGAME_EXPORTS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib x:\game\xr_3da.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib x:\game\xr_3da.lib /nologo /dll /profile /map /debug /machine:I386

!ENDIF 

# Begin Target

# Name "xrGame - Win32 Release"
# Name "xrGame - Win32 Debug"
# Name "xrGame - Win32 Mixed"
# Begin Group "Kernel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\clsid_game.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WayPointDef.h
# End Source File
# Begin Source File

SOURCE=.\xrGame.cpp
# End Source File
# End Group
# Begin Group "Objects"

# PROP Default_Filter ""
# Begin Group "Basic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Entity.cpp
# End Source File
# Begin Source File

SOURCE=.\Entity.h
# End Source File
# Begin Source File

SOURCE=.\GameObject.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject.h
# End Source File
# Begin Source File

SOURCE=.\MovementControl.cpp
# End Source File
# Begin Source File

SOURCE=.\MovementControl.h
# End Source File
# Begin Source File

SOURCE=.\SoundHelper.h
# End Source File
# End Group
# Begin Group "AI_Entity"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ai_commands.h
# End Source File
# Begin Source File

SOURCE=.\AI_Console.h
# End Source File
# Begin Source File

SOURCE=.\ai_goals.h
# End Source File
# Begin Source File

SOURCE=.\AI_PathNodes.cpp
# End Source File
# Begin Source File

SOURCE=.\AI_PathNodes.h
# End Source File
# Begin Source File

SOURCE=.\CustomMonster.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomMonster.h
# End Source File
# Begin Source File

SOURCE=.\CustomMonster_VCPU.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomMonster_VisTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomMonster_VisTrack.h
# End Source File
# Begin Source File

SOURCE=.\dbg_draw_frustum.cpp
# End Source File
# End Group
# Begin Group "Weapons"

# PROP Default_Filter ""
# Begin Group "Custom Weapon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Weapon.cpp
# End Source File
# Begin Source File

SOURCE=.\Weapon.h
# End Source File
# Begin Source File

SOURCE=.\WeaponAutoRifle.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponAutoRifle.h
# End Source File
# Begin Source File

SOURCE=.\WeaponHUD.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponHUD.h
# End Source File
# Begin Source File

SOURCE=.\WeaponMagazined.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponMagazined.h
# End Source File
# End Group
# Begin Group "Weapon M134"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WeaponM134.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponM134.h
# End Source File
# End Group
# Begin Group "Weapon Rail"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WeaponRail.cpp

!IF  "$(CFG)" == "xrGame - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xrGame - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xrGame - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WeaponRail.h

!IF  "$(CFG)" == "xrGame - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xrGame - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xrGame - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Weapon Protecta"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WeaponProtecta.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WeaponProtecta.h
# End Source File
# End Group
# Begin Group "Effectors"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EffectorNoise.cpp
# End Source File
# Begin Source File

SOURCE=.\EffectorNoise.h
# End Source File
# Begin Source File

SOURCE=.\EffectorRecoil.cpp
# End Source File
# Begin Source File

SOURCE=.\EffectorRecoil.h
# End Source File
# Begin Source File

SOURCE=.\EffectorShot.cpp
# End Source File
# Begin Source File

SOURCE=.\EffectorShot.h
# End Source File
# End Group
# Begin Group "Weapon AK74"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WeaponAK74.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponAK74.h
# End Source File
# End Group
# Begin Group "LR300"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WeaponLR300.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponLR300.h
# End Source File
# End Group
# Begin Group "FN2000"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WeaponFN2000.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponFN2000.h
# End Source File
# End Group
# Begin Group "HPSA"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WeaponHPSA.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponHPSA.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\xr_weapon_list.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_weapon_list.h
# End Source File
# End Group
# Begin Group "_door"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CustomDoor.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomDoor.h
# End Source File
# End Group
# Begin Group "_item"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CustomItem.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomItem.h
# End Source File
# End Group
# Begin Group "_lift"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CustomLift.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomLift.h
# End Source File
# End Group
# Begin Group "Events"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CustomEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomEvent.h
# End Source File
# End Group
# Begin Group "_actor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Actor.cpp
# End Source File
# Begin Source File

SOURCE=.\Actor.h
# End Source File
# Begin Source File

SOURCE=.\Actor_Flags.h
# End Source File
# Begin Source File

SOURCE=.\ActorAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\ActorAnimation.h
# End Source File
# Begin Source File

SOURCE=.\ActorCameras.cpp
# End Source File
# Begin Source File

SOURCE=.\ActorInput.cpp
# End Source File
# Begin Source File

SOURCE=.\EffectorBobbing.cpp
# End Source File
# Begin Source File

SOURCE=.\EffectorBobbing.h
# End Source File
# End Group
# Begin Group "_actor_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DemoActor.cpp
# End Source File
# Begin Source File

SOURCE=.\DemoActor.h
# End Source File
# Begin Source File

SOURCE=.\DemoActorCameras.cpp
# End Source File
# End Group
# Begin Group "_dummy"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DummyObject.cpp
# End Source File
# Begin Source File

SOURCE=.\DummyObject.h
# End Source File
# End Group
# Begin Group "_helicopter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Flyer.cpp
# End Source File
# Begin Source File

SOURCE=.\Flyer.h
# End Source File
# Begin Source File

SOURCE=.\FlyerCameras.cpp
# End Source File
# Begin Source File

SOURCE=.\FlyerInput.cpp
# End Source File
# End Group
# Begin Group "AI_Monsters"

# PROP Default_Filter ""
# Begin Group "ai_hen"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AI\Hen\ai_hen.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Hen\ai_hen.h
# End Source File
# Begin Source File

SOURCE=.\AI\Hen\ai_hen_selectors.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Hen\ai_hen_selectors.h
# End Source File
# End Group
# Begin Group "ai_rat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AI\Rat\ai_rat.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Rat\ai_rat.h
# End Source File
# Begin Source File

SOURCE=.\AI\Rat\ai_rat_selectors.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Rat\ai_rat_selectors.h
# End Source File
# End Group
# Begin Group "ai_soldier"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AI\Soldier\ai_soldier.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Soldier\ai_soldier.h
# End Source File
# Begin Source File

SOURCE=.\AI\Soldier\ai_soldier_animations.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Soldier\ai_soldier_fire.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Soldier\ai_soldier_fsm.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Soldier\ai_soldier_look.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Soldier\ai_soldier_selectors.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Soldier\ai_soldier_selectors.h
# End Source File
# Begin Source File

SOURCE=.\AI\Soldier\ai_soldier_templates.cpp
# End Source File
# End Group
# Begin Group "ai_crow"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AI\Crow\ai_crow.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Crow\ai_crow.h
# End Source File
# End Group
# Begin Group "ai_zombie"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AI\Zombie\ai_zombie.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Zombie\ai_zombie.h
# End Source File
# Begin Source File

SOURCE=.\AI\Zombie\ai_zombie_animations.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Zombie\ai_zombie_fire.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Zombie\ai_zombie_fsm.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Zombie\ai_zombie_look.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Zombie\ai_zombie_selectors.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\Zombie\ai_zombie_selectors.h
# End Source File
# Begin Source File

SOURCE=.\AI\Zombie\ai_zombie_templates.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\AI\ai_monsters.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\ai_monsters.h
# End Source File
# Begin Source File

SOURCE=.\AI\ai_monsters_misc.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\ai_monsters_misc.h
# End Source File
# End Group
# Begin Group "Test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TestPhysics.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\TestPhysics.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Group.cpp
# End Source File
# Begin Source File

SOURCE=.\Group.h
# End Source File
# Begin Source File

SOURCE=.\Level.cpp
# End Source File
# Begin Source File

SOURCE=.\Level.h
# End Source File
# End Group
# Begin Group "HUD"

# PROP Default_Filter ""
# Begin Group "UI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UI.cpp
# End Source File
# Begin Source File

SOURCE=.\UI.h
# End Source File
# Begin Source File

SOURCE=.\UICursor.cpp
# End Source File
# Begin Source File

SOURCE=.\UICursor.h
# End Source File
# Begin Source File

SOURCE=.\UICustomItem.cpp
# End Source File
# Begin Source File

SOURCE=.\UICustomItem.h
# End Source File
# Begin Source File

SOURCE=.\UIDynamicItem.cpp
# End Source File
# Begin Source File

SOURCE=.\UIDynamicItem.h
# End Source File
# Begin Source File

SOURCE=.\UIGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\UIGroup.h
# End Source File
# Begin Source File

SOURCE=.\UIHealth.cpp
# End Source File
# Begin Source File

SOURCE=.\UIHealth.h
# End Source File
# Begin Source File

SOURCE=.\UIStaticItem.cpp
# End Source File
# Begin Source File

SOURCE=.\UIStaticItem.h
# End Source File
# Begin Source File

SOURCE=.\UIWeapon.cpp
# End Source File
# Begin Source File

SOURCE=.\UIWeapon.h
# End Source File
# Begin Source File

SOURCE=.\UIZoneMap.cpp
# End Source File
# Begin Source File

SOURCE=.\UIZoneMap.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ContextMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\ContextMenu.h
# End Source File
# Begin Source File

SOURCE=.\HitMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\HitMarker.h
# End Source File
# Begin Source File

SOURCE=.\HUDCursor.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDCursor.h
# End Source File
# Begin Source File

SOURCE=.\HUDManager.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDManager.h
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\xrMessages.h
# End Source File
# Begin Source File

SOURCE=.\xrServer.cpp
# End Source File
# Begin Source File

SOURCE=.\xrServer.h
# End Source File
# Begin Source File

SOURCE=.\xrServer_Entities.cpp
# End Source File
# End Group
# Begin Group "Effects"

# PROP Default_Filter ""
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DynamicHeightMap.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\DynamicHeightMap.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Rain.cpp
# End Source File
# Begin Source File

SOURCE=.\Rain.h
# End Source File
# Begin Source File

SOURCE=.\Tracer.cpp
# End Source File
# Begin Source File

SOURCE=.\Tracer.h
# End Source File
# End Group
# Begin Group "AI Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ai_a_star.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_a_star.h
# End Source File
# Begin Source File

SOURCE=.\AI_qNode.cpp
# End Source File
# Begin Source File

SOURCE=.\AI_qRange.cpp
# End Source File
# Begin Source File

SOURCE=.\AI_Space.cpp
# End Source File
# Begin Source File

SOURCE=.\AI_Space.h
# End Source File
# Begin Source File

SOURCE=.\AI_Utilities.cpp
# End Source File
# End Group
# Begin Group "Spawn Process"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Spawn.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Spawn.h
# End Source File
# Begin Source File

SOURCE=.\Spawn_Event.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Spawn_Event.h
# End Source File
# End Group
# Begin Source File

SOURCE=X:\game\engine.log
# End Source File
# Begin Source File

SOURCE=.\notes.txt
# End Source File
# Begin Source File

SOURCE=.\Shedule.txt
# End Source File
# Begin Source File

SOURCE=.\todo.txt
# End Source File
# End Target
# End Project

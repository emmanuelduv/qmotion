[Setup]
AppName=qmotion
AppVerName=qmotion version 2.2
AppVersion=2.2
VersionInfoVersion=2.2
DefaultDirName={pf}\qmotion
DefaultGroupName=qmotion
AppPublisher=Stéphane List
AppPublisherURL=http://slist.lilotux.net/linux/qmotion/index_fr.html
AppCopyright=Copyright (C) 2010 Stéphane List (GPL)
LicenseFile=gpl-3.0.txt
OutputBaseFilename=qmotion-setup
OutputDir=release

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}
Name: sources; Description: {cm:SourceFiles,{app}}; GroupDescription: {cm:ExtraFiles}; Flags: unchecked

[Dirs]
Name: {app}
Name: {group}
Name: {app}\Sources; Tasks: sources

[Files]

; The following file is the redistributable Run-time for my version of Visual Studio;
; Please substitute your own
Source: ..\2008fra_VCRedist_x86.exe; DestDir: {tmp}; Flags: deleteafterinstall

Source: release\qmotion.exe; DestDir: {app}
Source: qmotion_fr.qm; DestDir: {app}; Languages: fra

; OpenCV
Source: c:\program files\_dev\opencv\bin\cxcore110.dll; DestDir: {app}
Source: c:\program files\_dev\opencv\bin\highgui110.dll; DestDir: {app}
Source: c:\program files\_dev\opencv\bin\cv110.dll; DestDir: {app}

; Qt4
Source: C:\Qt\2009.01\bin\QtCore4.dll; DestDir: {app}
Source: C:\Qt\2009.01\bin\QtGUI4.dll; DestDir: {app}
Source: C:\Qt\2009.01\bin\QtNetwork4.dll; DestDir: {app}
Source: C:\Qt\2009.01\bin\QtSvg4.dll; DestDir: {app}
Source: C:\Qt\2009.01\bin\imageformats\*.*; DestDir: {app}\imageformats
Source: C:\Qt\2009.01\bin\iconengines\*.*; DestDir: {app}\iconengines

Source: gpl-3.0.txt; DestDir: {app}; Tasks: not sources
Source: gpl-3.0.txt; DestDir: {app}\Sources; Tasks: sources

; Sources
Source: qmotion.iss; DestDir: {app}\Sources; Tasks: sources
Source: qmotion.pro; DestDir: {app}\Sources; Tasks: sources
Source: main.cpp; DestDir: {app}\Sources; Tasks: sources
Source: qmotion.cpp; DestDir: {app}\Sources; Tasks: sources
Source: qmotion.h; DestDir: {app}\Sources; Tasks: sources
Source: ftpsetupdlg.cpp; DestDir: {app}\Sources; Tasks: sources
Source: ftpsetupdlg.h; DestDir: {app}\Sources; Tasks: sources
Source: ftpsetupdlg_impl.cpp; DestDir: {app}\Sources; Tasks: sources
Source: ftpsetupdlg_impl.h; DestDir: {app}\Sources; Tasks: sources
Source: recordersetupdlg.cpp; DestDir: {app}\Sources; Tasks: sources
Source: recordersetupdlg.h; DestDir: {app}\Sources; Tasks: sources
Source: recordersetupdlg_impl.cpp; DestDir: {app}\Sources; Tasks: sources
Source: recordersetupdlg_impl.h; DestDir: {app}\Sources; Tasks: sources
Source: qmotion.ui; DestDir: {app}\Sources; Tasks: sources
Source: ftpsetupdlg.ui; DestDir: {app}\Sources; Tasks: sources
Source: recordersetupdlg.ui; DestDir: {app}\Sources; Tasks: sources
Source: qmotion_fr.ts; DestDir: {app}\Sources; Tasks: sources
Source: qmotion.qrc; DestDir: {app}\Sources; Tasks: sources
Source: ftp.png; DestDir: {app}\Sources; Tasks: sources
Source: qmotion.rc; DestDir: {app}\Sources; Tasks: sources
Source: hibou.ico; DestDir: {app}\Sources; Tasks: sources
Source: qmotion.svg; DestDir: {app}\Sources; Tasks: sources
Source: qmotion_hibou.svg; DestDir: {app}\Sources; Tasks: sources
Source: recorder_folder.svg; DestDir: {app}\Sources; Tasks: sources

[Run]
Filename: {tmp}\2008fra_VCRedist_x86.exe; Parameters: /Q; Flags: waituntilterminated; StatusMsg: {cm:VCRedist}
Filename: {app}\qmotion.exe; Description: {cm:LaunchProgram,qmotion}; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: dirifempty; Name: {app}
Type: dirifempty; Name: {group}

[Icons]
Name: {group}\qmotion; Filename: {app}\qmotion.exe; WorkingDir: {app}; Comment: {cm:LaunchProgram,qmotion}
Name: {group}\{cm:UninstallProgram,qmotion}; Filename: {uninstallexe}
Name: {userdesktop}\qmotion; Filename: {app}\qmotion.exe; WorkingDir: {app}; Comment: {cm:LaunchProgram,qmotion}

[Languages]
Name: eng; MessagesFile: compiler:Default.isl
Name: fra; MessagesFile: compiler:Languages\French.isl

[CustomMessages]
eng.ExtraFiles=Extra Files
eng.SourceFiles=Source Files in "%1\Sources" subdirectory
eng.VCRedist=Updating System Components, please wait...

fra.ExtraFiles=Fichiers supplémentaires
fra.SourceFiles=Fichiers sources dans le sous-répertoire "%1\Sources"
fra.VCRedist=Mise à jour des composants système, veuillez patienter...

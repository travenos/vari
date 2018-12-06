; webot inno setup install script

#define AppName      "${APP_NAME}"
#define AppVersion   "${PROJECT_VERSION_LONG}"
#define AppPublisher "${PROJECT_VENDOR_LONG}"
#define AppURL       "${ORG_WEBSITE}"
#define AppGUID      "34B25717-C3E5-455D-8FCF-01E60BAEB42A"

#define AppVARIName    "vari"
#define AppVARIExeName "vari.exe"

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName = {#AppName}
AppVersion = {#AppVersion}
AppId = {#AppGUID}
AppPublisher = {#AppPublisher}
AppCopyright = {#AppPublisher}
AppPublisherURL = {#AppURL}
VersionInfoCompany = {#AppPublisher}
DefaultDirName = {pf}\{#AppName}
DefaultGroupName = {#AppName}
UninstallDisplayIcon = {app}\{#AppVClientExeName}
Compression = lzma2
SolidCompression = yes
OutputBaseFilename = ${APP_NAME}-${PROJECT_VERSION_LONG}

[Languages]
Name: ru; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#AppVARIExeName}"; DestDir: "{app}"

Source: "$ENV{QTDIR}/bin/Qt5Core.dll";    DestDir: "{app}"
Source: "$ENV{QTDIR}/bin/Qt5Gui.dll";     DestDir: "{app}"
Source: "$ENV{QTDIR}/bin/Qt5Widgets.dll"; DestDir: "{app}"
Source: "$ENV{QTDIR}/bin/Qt5Sql.dll";     DestDir: "{app}"
Source: "$ENV{QTDIR}/bin/Qt5OpenGL.dll";  DestDir: "{app}"

Source: "$ENV{COINDIR}/bin/SoQt1.dll";      DestDir: "{app}"
Source: "$ENV{QTDIR}/bin/Coin4.dll";         DestDir: "{app}"

Source: "msvcr120.dll"; DestDir: "{app}"
Source: "msvcp120.dll"; DestDir: "{app}"

[Run]
Filename: "{sys}\netsh.exe"; Parameters: "firewall add allowedprogram ""{app}\vclient.exe"" ""Wicron vclient"" ENABLE ALL"; StatusMsg: "Status msg..."; Flags: runhidden; MinVersion: 0,5.01.2600sp2;

[Icons]
Name: "{group}\{#AppName}";           Filename: "{app}\{#AppVClientExeName}"; WorkingDir: "{app}"
Name: "{group}\Uninstall {#AppName}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#AppName}";   Filename: "{app}\{#AppVClientExeName}"; Tasks: desktopicon

[UninstallRun]
Filename: {sys}\netsh.exe; Parameters: "firewall delete allowedprogram program=""{app}\vclient.exe"""; Flags: runhidden; MinVersion: 0,5.01.2600sp2;

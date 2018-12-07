; vari inno setup install script

#define AppName      "${APP_NAME}"
#define AppVersion   "${PROJECT_VERSION_LONG}"
#define AppPublisher "${PROJECT_VENDOR_LONG}"
#define AppURL       "${ORG_WEBSITE}"
#define AppGUID      "931BBE66-77AA-471C-B785-7FAD395E634E"

#define AppVARIName    "{#AppName}"
#define AppVARIExeName "{#AppName}.exe"

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

Source: "sql/create.sql"; DestDir: "{app}/sql"
Source: "sql/remove.sql"; DestDir: "{app}/sql"

[Run]
Filename: "{sys}\psql.exe"; Parameters: "-h 127.0.0.1 -U postgres -f "{app}\sql\create.sql""; StatusMsg: "Initializing the database"; Flags: runhidden;

[Icons]
Name: "{group}\{#AppName}";           Filename: "{app}\{#AppVARIExeName}"; WorkingDir: "{app}"
Name: "{group}\Uninstall {#AppName}"; Filename: "{uninstall.exe}"
Name: "{commondesktop}\{#AppName}";   Filename: "{app}\{#AppVARIExeName}"; Tasks: desktopicon

[UninstallRun]
Filename: "{sys}\psql.exe"; Parameters: "-h 127.0.0.1 -U postgres -f "{app}\sql\remove.sql""; StatusMsg: "Dropping the database"; Flags: runhidden;

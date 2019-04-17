; vari inno setup install script

#define AppName      "${APP_NAME}"
#define AppVersion   "${PROJECT_VERSION_LONG}"
#define AppPublisher "${PROJECT_VENDOR_LONG}"
#define AppURL       "${ORG_WEBSITE}"
#define AppGUID      "931BBE66-77AA-471C-B785-7FAD395E634E"

#define AppVARIName    "${APP_NAME}"
#define AppVARIExeName "${APP_NAME}.exe"

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
UninstallDisplayIcon = {app}\{#AppVARIExeName}
Compression = lzma2
SolidCompression = yes
OutputBaseFilename = ${APP_NAME}-${PROJECT_VERSION_LONG}

[Languages]
Name: ru; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#AppVARIExeName}"; DestDir: "{app}"

Source: "${QT_DIR}/bin/Qt5Core.dll";    DestDir: "{app}"
Source: "${QT_DIR}/bin/Qt5Gui.dll";     DestDir: "{app}"
Source: "${QT_DIR}/bin/Qt5Widgets.dll"; DestDir: "{app}"
Source: "${QT_DIR}/bin/Qt5Sql.dll";     DestDir: "{app}"
Source: "${QT_DIR}/bin/Qt5OpenGL.dll";  DestDir: "{app}"
Source: "${QT_DIR}/bin/Qt5PrintSupport.dll";  DestDir: "{app}"
Source: "${QT_DIR}/bin/Qt5WinExtras.dll";  DestDir: "{app}"
Source: "${QT_DIR}/plugins/platforms/qwindows.dll";  DestDir: "{app}/platforms"
Source: "${QT_DIR}/plugins/sqldrivers/qsqlite.dll";  DestDir: "{app}/sqldrivers"

Source: "${COIN_DIR}/bin/SoQt1.dll";    DestDir: "{app}"
Source: "${COIN_DIR}/bin/Coin4.dll";    DestDir: "{app}"

Source: "msvcr120.dll"; DestDir: "{app}"
Source: "msvcp120.dll"; DestDir: "{app}"

Source: "sql/create.sql"; DestDir: "{app}/sql"
Source: "sql/remove.sql"; DestDir: "{app}/sql"

[Icons]
Name: "{group}\{#AppName}";           Filename: "{app}\{#AppVARIExeName}"; WorkingDir: "{app}"
Name: "{group}\Uninstall {#AppName}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#AppName}";   Filename: "{app}\{#AppVARIExeName}"; Tasks: desktopicon


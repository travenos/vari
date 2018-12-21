IDI_ICON1   ICON   DISCARDABLE   "${RESOURCES_DIRECTORY}/vari.ico"

#include <windows.h>

VS_VERSION_INFO VERSIONINFO
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040904E4"
        BEGIN
            VALUE "CompanyName",        "${PROJECT_VENDOR_LONG}"
            VALUE "FileDescription",    "${APP_NAME}"
            VALUE "FileVersion",        "${PROJECT_VERSION_LONG}\0"
            VALUE "InternalName",       "${APP_NAME}"
            VALUE "LegalCopyright",     "Copyright © ${CURRENT_YEAR} ${PROJECT_VENDOR_LONG}"
            VALUE "LegalTrademarks1",   "All Rights Reserved"
            VALUE "LegalTrademarks2",   "All Rights Reserved"
            VALUE "OriginalFilename",   "${APP_NAME}.exe"
            VALUE "ProductName",        "${APP_NAME}"
            VALUE "ProductVersion",     "${PROJECT_VERSION_LONG}\0"
        END
    END

    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x419, 1252
    END
END

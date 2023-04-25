/*
 *  version.h - Version of the library
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_version_h
#define sblib_version_h

/**
 * The version of the library. An integer value.
 * Can be displayed in ETS under deviceinfo->order number last two bytes.
 */
#define SBLIB_VERSION 0x0202

/**
 * Macro for the name and version of the application using the library.
 * @param appName       char[8]
 * @param majorVersion  char[1]
 * @param minorVersion  char[2]
 */
#define APP_VERSION(appName, majorVersion, minorVersion) \
        __attribute__((used)) volatile const char APP_VERSION_STRING[20] = "!AVP!@:" appName majorVersion "." minorVersion; \
        __attribute__((used)) volatile static const char * v = getAppVersion();


/**
 * @brief   Helper function to always include @ref APP_VERSION_STRING in the resulting binary
 * @details Disabling optimization seems to be the only way to ensure that this is not being removed by the linker
 *          to keep the variable, we need to declare a function that uses it.
 *          Alternatively the link script may be modified by adding KEEP to the section
 *
 * @return @ref APP_VERSION_STRING
 */
volatile const char * __attribute__((optimize("O0"))) getAppVersion();

#endif /*sblib_version_h*/

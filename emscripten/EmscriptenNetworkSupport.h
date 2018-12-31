//////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2018 Corona Labs Inc.
// Contact: support@coronalabs.com
//
// This file is part of the Corona game engine.
//
// Commercial License Usage
// Licensees holding valid commercial Corona licenses may use this file in
// accordance with the commercial license agreement between you and 
// Corona Labs Inc. For licensing terms and conditions please contact
// support@coronalabs.com or visit https://coronalabs.com/com-license
//
// GNU General Public License Usage
// Alternatively, this file may be used under the terms of the GNU General
// Public license version 3. The license is as published by the Free Software
// Foundation and appearing in the file LICENSE.GPL3 included in the packaging
// of this file. Please review the following information to ensure the GNU 
// General Public License requirements will
// be met: https://www.gnu.org/licenses/gpl-3.0.html
//
// For overview and more information on licensing please refer to README.md
//
//////////////////////////////////////////////////////////////////////////////

#include "Corona/CoronaLua.h"

#ifndef _EmscriptenNetworkSupport_H_
#define _EmscriptenNetworkSupport_H_

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <map>
#include <vector>
#include <string>

typedef std::map<std::string, std::string>	StringMap;
typedef std::vector<unsigned char>			ByteVector;
typedef std::string							UTF8String;


void paramValidationFailure(lua_State *luaState, char *message, ...);
bool isudatatype(lua_State *L, int idx, const char *name);
UTF8String pathForTemporaryFileWithPrefix(const char *prefix, UTF8String pathDir);

char * getContentType(const char *contentTypeHeader);
char * getContentTypeEncoding(const char *contentTypeHeader);

bool isContentTypeXML(const char *contentType);

bool isContentTypeHTML(const char *contentType);

bool isContentTypeText(const char *contentType);

char * getEncodingFromContent(const char *contentType, const char *content);

// ----------------------------------------------------------------------------

UTF8String utf8_encode(const wchar_t * wideString);
UTF8String utf8_encode(const wchar_t * wideString, int wideStringLen);
const wchar_t * getWCHARs(UTF8String string);

// ----------------------------------------------------------------------------

class WinHttpRequestOperation;

typedef enum {
	UNKNOWN = 0,
	Upload = 1,
	Download = 2,
	None = 3,
} ProgressDirection;

ProgressDirection getProgressDirectionFromString(const char *progressString);
const char* getProgressDirectionName(ProgressDirection progressDirection);


class CoronaFileSpec
{
public:

	CoronaFileSpec(const char *filename, void *baseDirectory, const char *fullPath, bool isResourceFile)
	{
		fFilename = filename;
		fBaseDirectory = baseDirectory;
		fFullPath = fullPath;
		fIsResourceFile = isResourceFile;
	}

	CoronaFileSpec(CoronaFileSpec *fileSpec)
	{
		fFilename = fileSpec->fFilename;
		fBaseDirectory = fileSpec->fBaseDirectory;
		fFullPath = fileSpec->fFullPath;
		fIsResourceFile = fileSpec->fIsResourceFile;
	}

	~CoronaFileSpec()
	{
	}

	UTF8String getFilename()
	{
		return fFilename;
	}

	void * getBaseDirectory()
	{
		return fBaseDirectory;
	}

	UTF8String getFullPath()
	{
		return fFullPath;
	}

	bool isResourceFile()
	{
		return fIsResourceFile;
	}

private:

	UTF8String	fFilename;
	void*		fBaseDirectory;
	UTF8String	fFullPath;
	bool		fIsResourceFile;

};

// ----------------------------------------------------------------------------

typedef enum
{
	TYPE_NONE,
	TYPE_STRING,
	TYPE_BYTES,
	TYPE_FILE,
} BodyType;

typedef struct
{
	BodyType bodyType;
	union
	{
		UTF8String* bodyString;
		ByteVector* bodyBytes;
		CoronaFileSpec* bodyFile;
	};
} Body;

// ----------------------------------------------------------------------------

class NetworkRequestState
{
public:

	NetworkRequestState();
	~NetworkRequestState();

	void setError(UTF8String *message = NULL);
	void setPhase(const char *phase);
	void setResponseHeaders(const char *headers);
	void setResponseType(const char *responseType);
	void setBytesEstimated(long long nBytesTransferred);
	void setBytesTransferred(long long nBytesTransferred);
	void incrementBytesTransferred(int newBytesTransferred);
	void setDebugValue(char *debugValue, char *debugKey);
	void setURL(const UTF8String& url) { fRequestURL = url; }
	void setStatus(int status) { fStatus = status; }

	bool isError();
	StringMap getResponseHeaders();
	UTF8String getResponseHeaderValue(const char *headerKey);
	Body* getResponseBody();
	const char* getPhase();

	int pushToLuaState(lua_State *L);

	Body			fResponseBody;

private:

	UTF8String		fRequestURL;
	int				fStatus;
	bool			fIsError;
	UTF8String		fPhase;
	StringMap		fResponseHeaders;
	UTF8String		fResponseType;

	long long		fBytesEstimated;
	long long		fBytesTransferred;
	StringMap		fDebugValues;

	int fRequestID;
};

// ----------------------------------------------------------------------------

class LuaCallback
{
public:

	LuaCallback(lua_State* luaState, CoronaLuaRef luaReference);
	~LuaCallback();

	bool callWithNetworkRequestState(NetworkRequestState *requestState);
	void unregister();

private:

	lua_State* fLuaState;
	CoronaLuaRef fLuaReference;

	std::string fLastNotificationPhase;
	uint32_t fMinNotificationIntervalMs;
	uint32_t fLastNotificationTime;

};

// ----------------------------------------------------------------------------

class NetworkRequestParameters
{
public:

	NetworkRequestParameters(lua_State *L);
	~NetworkRequestParameters();

	bool isValid();

	UTF8String getRequestUrl();
	UTF8String getRequestMethod();
	ProgressDirection getProgressDirection();
	UTF8String getRequestHeaderString();
	StringMap* getRequestHeaders();
	UTF8String* getRequestHeaderValue(const char *headerKey);
	Body* getRequestBody();
	long long getRequestBodySize();
	CoronaFileSpec* getResponseFile();
	LuaCallback* getLuaCallback();
	int getTimeout();
	bool isDebug();
	bool getHandleRedirects();

private:

	UTF8String		fRequestUrl;
	UTF8String		fMethod;
	ProgressDirection fProgressDirection;
	StringMap		fRequestHeaders;
	bool			fIsBodyTypeText;
	int				fTimeout;
	bool			fIsDebug;
	Body			fRequestBody;
	long long		fRequestBodySize;
	CoronaFileSpec*	fResponseFile;

	LuaCallback*	fLuaCallback;
	bool			fIsValid;
	bool			fHandleRedirects;
};

#endif

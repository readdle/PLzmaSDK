// DLLExports2.cpp

#include "StdAfx.h"

#include "../../Common/MyWindows.h"
#include "../../Common/MyInitGuid.h"

#if defined(Z7_LARGE_PAGES)
#include "../../../C/Alloc.h"
#endif

#include "../../Common/ComTry.h"

#include "../../Windows/NtCheck.h"
#include "../../Windows/PropVariant.h"

#include "../ICoder.h"
#include "../IPassword.h"

#include "../Common/CreateCoder.h"

#include "IArchive.h"

Z7_DEFINE_GUID(CLSID_CArchiveHandler,
    k_7zip_GUID_Data1,
    k_7zip_GUID_Data2,
    k_7zip_GUID_Data3_Common,
    0x10, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00);

STDAPI CreateCoder(const GUID *clsid, const GUID *iid, void **outObject);
STDAPI CreateHasher(const GUID *clsid, IHasher **hasher);
STDAPI CreateArchiver(const GUID *clsid, const GUID *iid, void **outObject);

STDAPI CreateObject(const GUID *clsid, const GUID *iid, void **outObject);
STDAPI CreateObject(const GUID *clsid, const GUID *iid, void **outObject)
{
  // COM_TRY_BEGIN
  *outObject = NULL;
  if (*iid == IID_ICompressCoder ||
      *iid == IID_ICompressCoder2 ||
      *iid == IID_ICompressFilter)
    return CreateCoder(clsid, iid, outObject);
  if (*iid == IID_IHasher)
    return CreateHasher(clsid, (IHasher **)outObject);
  return CreateArchiver(clsid, iid, outObject);
  // COM_TRY_END
}

STDAPI SetLargePageMode();
STDAPI SetLargePageMode()
{
  #if defined(Z7_LARGE_PAGES)
  #ifdef _WIN32
  SetLargePageSize();
  #endif
  #endif
  return S_OK;
}

extern bool g_CaseSensitive;

STDAPI SetCaseSensitive(Int32 caseSensitive);
STDAPI SetCaseSensitive(Int32 caseSensitive)
{
  g_CaseSensitive = (caseSensitive != 0);
  return S_OK;
}

/*
UInt32 g_ClientVersion;
STDAPI SetClientVersion(UInt32 version);
STDAPI SetClientVersion(UInt32 version)
{
  g_ClientVersion = version;
  return S_OK;
}
*/

/*
STDAPI SetProperty(Int32 id, const PROPVARIANT *value);
STDAPI SetProperty(Int32 id, const PROPVARIANT *value)
{
  return S_OK;
}
*/

#ifdef Z7_EXTERNAL_CODECS

CExternalCodecs g_ExternalCodecs;

STDAPI SetCodecs(ICompressCodecsInfo *compressCodecsInfo);
STDAPI SetCodecs(ICompressCodecsInfo *compressCodecsInfo)
{
  COM_TRY_BEGIN

  // OutputDebugStringA(compressCodecsInfo ? "SetCodecs" : "SetCodecs NULL");
  if (compressCodecsInfo)
  {
    g_ExternalCodecs.GetCodecs = compressCodecsInfo;
    return g_ExternalCodecs.Load();
  }
  g_ExternalCodecs.ClearAndRelease();
  return S_OK;

  COM_TRY_END
}

#else

STDAPI SetCodecs(ICompressCodecsInfo *);
STDAPI SetCodecs(ICompressCodecsInfo *)
{
  return S_OK;
}

#endif

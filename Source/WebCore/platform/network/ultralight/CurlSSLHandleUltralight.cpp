#include "config.h"
#include "CurlSSLHandle.h"
#include "ResourceFileLoader.h"
#include "StringUltralight.h"
#include <Ultralight/platform/Config.h>
#include <Ultralight/platform/Logger.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/private/util/Debug.h>
#include <wtf/FileSystem.h>
#include <wtf/MemoryProfiler.h>

namespace WebCore {

void CurlSSLHandle::platformInitialize()
{
    ProfiledMemoryZone(MemoryTag::Resource);

    // Restrict offered ciphers to modern AEAD suites; excludes RC4, 3DES, MD5,
    // and other ciphers flagged as insecure by howsmyssl.com.
    constexpr auto cipherList =
        "TLS_AES_128_GCM_SHA256:"
        "TLS_CHACHA20_POLY1305_SHA256:"
        "TLS_AES_256_GCM_SHA384:"
        "ECDHE-ECDSA-AES128-GCM-SHA256:"
        "ECDHE-RSA-AES128-GCM-SHA256:"
        "ECDHE-ECDSA-CHACHA20-POLY1305:"
        "ECDHE-RSA-CHACHA20-POLY1305:"
        "ECDHE-ECDSA-AES256-GCM-SHA384:"
        "ECDHE-RSA-AES256-GCM-SHA384:"
        "ECDHE-ECDSA-AES256-SHA:"
        "ECDHE-ECDSA-AES128-SHA:"
        "ECDHE-RSA-AES128-SHA:"
        "ECDHE-RSA-AES256-SHA:"
        "AES128-GCM-SHA256:"
        "AES256-GCM-SHA384:"
        "AES128-SHA:"
        "AES256-SHA";

    constexpr auto ecCurves =
        "X25519:"
        "P-256:"
        "P-384:"
        "P-521";

    setCipherList(cipherList);
    setECCurves(ecCurves);

    auto& platform = ultralight::Platform::instance();
    auto config = platform.config();
    ultralight::RefPtr<ultralight::Buffer> certData = ResourceFileLoader::openFile("cacert.pem"_s);
    size_t fileSize = 0;
    CertificateInfo::Certificate buffer;

    if (!certData)
        goto FAIL_LOAD;

    fileSize = certData->size();
    if (fileSize == 0)
        goto FAIL_LOAD;

    buffer.append(static_cast<char*>(certData->data()), fileSize);

    setCACertData(std::move(buffer));

    return;

FAIL_LOAD:
    UL_LOG_ERROR("Failed to load cacert.pem (SSL certificate chain), the library will be unable to make SSL/HTTPS requests.")
    return;
}

}

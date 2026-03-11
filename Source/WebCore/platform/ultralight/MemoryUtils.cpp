#include "config.h"
#include "MemoryUtils.h"
#include "ResourceUsageThread.h"
#include <wtf/MemoryPressureHandler.h>
#include <Ultralight/private/util/Debug.h>
#include <sstream>
#include <string>
#include "CommonVM.h"
#include <JavaScriptCore/VM.h>

namespace WebCore {

static std::string fmtBytes(uint64_t bytes) {
  const char* suffix[] = { "B", "KB", "MB", "GB", "TB" };
  char length = sizeof(suffix) / sizeof(suffix[0]);

  int i = 0;
  double dblBytes = (double)bytes;

  if (bytes > 1024) {
    for (i = 0; (bytes / 1024) > 0 && i < length - 1; i++, bytes /= 1024)
      dblBytes = bytes / 1024.0;
  }

  static char output[200];
  sprintf(output, "%.02lf %s", dblBytes, suffix[i]);
  return output;
}

MemoryUtils::MemoryUtils() {

}

MemoryUtils::~MemoryUtils() {

}

#define PRINT_STATS(str, obj) \
  stream << str << fmtBytes(obj) << std::endl;

void MemoryUtils::logMemoryStatistics() {
#if ENABLE(RESOURCE_USAGE)
  std::ostringstream stream;
  stream << "Memory Usage (WebCore): " << std::endl;

  JSC::VM* vm = &commonVM();
  size_t currentGCHeapCapacity = vm->heap.blockBytesAllocated();
  size_t currentGCOwnedExtra = vm->heap.extraMemorySize();
  PRINT_STATS("    JavaScript:          ", currentGCHeapCapacity + currentGCOwnedExtra);

  UL_LOG_INFO(stream.str().c_str());
#endif
}

void MemoryUtils::beginSimulatedMemoryPressure() {
  WTF::MemoryPressureHandler::singleton().beginSimulatedMemoryPressure();
}

void MemoryUtils::endSimulatedMemoryPressure() {
  WTF::MemoryPressureHandler::singleton().endSimulatedMemoryPressure();
}

}  // namespace WebCore

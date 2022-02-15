#include "xbeScanner.hpp"
#include <chrono>
#include <fstream>
#ifdef NXDK
#include <windows.h>
#include <winnt.h>
#endif

#include "infoLog.hpp"
#include "timing.hpp"

#define XBE_TYPE_MAGIC (0x48454258)
#define SECTORSIZE 0x1000

XBEScanner* XBEScanner::singleton = nullptr;

XBEScanner* XBEScanner::getInstance() {
  if (!XBEScanner::singleton) {
    XBEScanner::singleton = new XBEScanner();
  }
  return XBEScanner::singleton;
}

#ifdef SCANNER_THREADED
XBEScanner::XBEScanner() : running(true) {
  scannerThread = std::thread(threadMain, this);
}

XBEScanner::~XBEScanner() {
  running = false;
  queue.emplace("", nullptr);
  if (scannerThread.joinable()) {
    scannerThread.join();
  }
}
#endif

void XBEScanner::scanPath(const std::string& path, Callback&& callback) {
  XBEScanner::getInstance()->addJob(path, callback);
}

#ifndef SCANNER_THREADED
void XBEScanner::rebuildCaches() {
  XBEScanner::getInstance()->rescan();
}
#endif

void XBEScanner::addJob(std::string const& path, const Callback& callback) {
  paths.push_back(path);

#ifdef SCANNER_THREADED
  std::lock_guard<std::mutex> lock(queueMutex);
  queue.emplace(path, callback);
  jobPending.notify_one();
#else
  QueueItem item(path, callback);
  item.scan();
#endif
}

void XBEScanner::rescan() {
  for (const auto& path: paths) {
#ifdef SCANNER_THREADED
#error "IMPLEMENT ME"
#else
    auto null_callback = [](bool, std::list<XBEInfo> const&, long long) {
    };
    QueueItem item(path, null_callback);
    item.scan(false);
#endif
  }
}

#ifdef SCANNER_THREADED
void XBEScanner::threadMain(XBEScanner* scanner) {
  while (scanner->running) {
    QueueItem task;
    {
      std::unique_lock<std::mutex> lock(scanner->queueMutex);
      scanner->jobPending.wait(lock, [=] { return !scanner->queue.empty(); });
      if (!scanner->running) {
        break;
      }
      task = scanner->queue.front();
      scanner->queue.pop();
    }

    std::string const& path = task.first;
    std::vector<XBEInfo> xbes;
    auto scanStart = std::chrono::steady_clock::now();
    bool succeeded = scan(path, xbes);
    auto scanDuration = millisSince(scanStart);
    InfoLog::outputLine("Scanning %s %s in %lld ms (%d found)\n", path.c_str(),
                        succeeded ? "succeeded" : "failed", scanDuration, xbes.size());

    task.second(succeeded, xbes);
  }
}
#endif

XBEScanner::QueueItem::QueueItem(std::string p, XBEScanner::Callback c) :
    path(std::move(p)), callback(std::move(c)) {
  xbeData.resize(SECTORSIZE);
}

XBEScanner::QueueItem::~QueueItem() {
#ifdef NXDK
  if (dirHandle != INVALID_HANDLE_VALUE) {
    CloseHandle(dirHandle);
  }
#endif
}

#define MAX_FILE_PATH_SIZE 248
#ifdef NXDK
static void ensureFolderExists(const std::string& folder_path) {
  if (folder_path.length() > MAX_FILE_PATH_SIZE) {
    assert(!"Folder Path is too long.");
  }

  char buffer[MAX_FILE_PATH_SIZE + 1] = { 0 };
  const char* path_start = folder_path.c_str();
  const char* slash = strchr(path_start, '\\');
  slash = strchr(slash + 1, '\\');

  while (slash) {
    strncpy(buffer, path_start, slash - path_start);
    if (!CreateDirectory(buffer, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
      assert(!"Failed to create output directory.");
    }

    slash = strchr(slash + 1, '\\');
  }

  // Handle case where there was no trailing slash.
  if (!CreateDirectory(path_start, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
    assert(!"Failed to create output directory.");
  }
}

static void writeCacheFile(const std::string& cache_file,
                           const std::list<XBEScanner::XBEInfo>& results) {

  std::ofstream out(cache_file);

  for (const auto& result: results) {
    out << result.name << std::endl;
    out << result.path << std::endl;
  }
}

static bool readCacheFile(const std::string& cache_file,
                          std::list<XBEScanner::XBEInfo>& results) {
  std::ifstream infile(cache_file);

  uint32_t line = 1;
  std::string name;
  while (std::getline(infile, name)) {
    ++line;
    std::string path;
    if (!std::getline(infile, path)) {
      InfoLog::outputLine(InfoLog::INFO,
                          "Failed to read cache file '%s', missing path at line %d",
                          cache_file.c_str(), line);

      return false;
    }
    ++line;

    results.emplace_back(name, path);
  }

  InfoLog::outputLine(InfoLog::INFO, "Restored cache from %s. %d items", cache_file.c_str(),
                      results.size());
  return true;
}

static const char* cache_directory = "C:\\NeXCache";
#endif // #ifdef NXDK

void XBEScanner::QueueItem::scan(bool allowCache) {
#ifdef NXDK
  std::string cache_file_name = path;
  std::replace(cache_file_name.begin(), cache_file_name.end(), '\\', '_');
  std::replace(cache_file_name.begin(), cache_file_name.end(), ':', '_');
  std::string cache_file_path = cache_directory;
  cache_file_path += "\\";
  cache_file_path += cache_file_name + ".scancache";
  ensureFolderExists(cache_directory);

  bool has_cache = readCacheFile(cache_file_path, results);
  if (allowCache && has_cache) {
    callback(true, results, 0);
    return;
  }

  std::map<std::string, std::string> previous_results;
  for (auto& info: results) {
    previous_results[info.path] = info.name;
  }

  if (dirHandle == INVALID_HANDLE_VALUE) {
    InfoLog::outputLine(InfoLog::INFO, "Starting scan of %s", path.c_str());
    results.clear();
    scanStart = std::chrono::steady_clock::now();
    if (!openDir()) {
      callback(false, results, millisSince(scanStart));
      return;
    }
  }

  do {
    if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      continue;
    }

    processFile(path + findData.cFileName + "\\default.xbe");
  } while (FindNextFile(dirHandle, &findData));

  CloseHandle(dirHandle);
  dirHandle = INVALID_HANDLE_VALUE;

  // Preserve existing names, which may have been manually edited.
  for (auto& info: results) {
    auto previous = previous_results.find(info.path);
    if (previous != previous_results.end()) {
      info.name = previous->second;
    }
  }

  writeCacheFile(cache_file_path, results);

  callback(true, results, millisSince(scanStart));
#endif
}

bool XBEScanner::QueueItem::openDir() {
#ifdef NXDK
  std::string searchmask = path + "*";
  dirHandle = FindFirstFile(searchmask.c_str(), &findData);
  return (dirHandle != INVALID_HANDLE_VALUE);
#else
  return false;
#endif
}

void XBEScanner::QueueItem::processFile(const std::string& xbePath) {
#ifdef NXDK
  FILE* xbeFile = fopen(xbePath.c_str(), "rb");
  if (!xbeFile) {
    return;
  }

  size_t read_bytes = fread(xbeData.data(), 1, SECTORSIZE, xbeFile);
  auto xbe = (PXBE_FILE_HEADER)xbeData.data();
  if (xbe->SizeOfHeaders > read_bytes) {
    if (xbeData.size() < xbe->SizeOfHeaders) {
      xbeData.resize(xbe->SizeOfHeaders);
    }
    read_bytes += fread(&xbeData[read_bytes], 1, xbe->SizeOfHeaders - read_bytes, xbeFile);
  }
  if (xbe->Magic != XBE_TYPE_MAGIC || xbe->ImageBase != XBE_DEFAULT_BASE
      || xbe->ImageBase > (uint32_t)xbe->CertificateHeader
      || (uint32_t)xbe->CertificateHeader + 4 >= (xbe->ImageBase + xbe->SizeOfHeaders)
      || xbe->SizeOfHeaders > read_bytes) {
    return;
  }
  auto xbeCert =
      (PXBE_CERTIFICATE_HEADER)&xbeData[(uint32_t)xbe->CertificateHeader - xbe->ImageBase];

  for (int offset = 0; offset < XBE_NAME_SIZE; ++offset) {
    if (xbeCert->TitleName[offset] < 0x0100) {
      xbeName[offset] = (char)xbeCert->TitleName[offset];
    } else if (xbeCert->TitleName[offset]) {
      xbeName[offset] = '?';
    } else {
      xbeName[offset] = 0;
      break;
    }
  }

  // Some homebrew content may not have a name in the certification
  // header, so fallback to using the path as the name.
  if (!strlen(xbeName)) {
    strncpy(xbeName, findData.cFileName, sizeof(xbeName) - 1);
  }
  fclose(xbeFile);

  results.emplace_back(xbeName, xbePath);
#endif // #ifdef NXDK
}

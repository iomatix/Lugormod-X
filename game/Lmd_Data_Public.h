#pragma once

#include "q_shared.h"
#include <memory>
#include <functional>
#include <any>
#include <string>

enum class DataWriteResult_t {
    DWR_OK,
    DWR_SKIP,
    DWR_CONTINUE,
    DWR_NODATA,
};

enum class DataParseResult_t {
    DPAR_OK,
    DPAR_FAIL,
    DPAR_IGNORE
};

struct DataFieldArgs {
    void* parseState;
    void* writeState;
};

using ParseFunction = std::function<DataParseResult_t(const std::string&, const std::string&, DataFieldArgs*, void*)>;
using WriteFunction = std::function<DataWriteResult_t(void*, std::string&, std::string&, DataFieldArgs*, void*)>;

using FreeDataFunction = std::function<void(void*, void*)>;


struct DataField_t {
    // Key
    std::string key;
    bool isPrefix;

    // Parsing
    ParseFunction parse;
    DataFieldArgs parseArgs;

    // Writing
    WriteFunction write;
    DataFieldArgs writeArgs;

    // Memory
    FreeDataFunction freeData;
    std::shared_ptr<void> freeDataArgs;
};


#pragma once

#include <functional>
#include <string>

namespace slog {

class RecordBase;

typedef std::function<void(const RecordBase &record, std::string &buffer)>
    Formatter;

void RecordToJSON(const RecordBase &record, std::string &buffer);

void RecordToText(const RecordBase &record, std::string &buffer);

} // namespace slog

#include "FormattersImpl.hpp"

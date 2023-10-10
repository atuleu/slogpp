#pragma once

#include <functional>
#include <string>

namespace slog {

class Record;

typedef std::function<void(const Record &record, std::string &buffer)>
    Formatter;

void RecordToJSON(const Record &record, std::string &buffer);

void RecordToText(const Record &record, std::string &buffer);

} // namespace slog

#include "FormattersImpl.hpp"

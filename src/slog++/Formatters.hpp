#pragma once

#include <string>

namespace slog {

using Buffer = std::string;

class Record;

using Formatter = void (*)(const Record &record, Buffer &);

void RecordToJSON(const Record &record, Buffer &);

void RecordToText(const Record &record, Buffer &);

} // namespace slog

#include "FormattersImpl.hpp"

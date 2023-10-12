#pragma once

#include "Types.hpp"
#include <string>

namespace slog {

class Record;

using Formatter = void (*)(const Record &record, Buffer &);

void RecordToJSON(const Record &record, Buffer &);

void RecordToRawText(const Record &record, Buffer &);

void RecordToANSIText(const Record &record, Buffer &);

} // namespace slog

#include "FormattersImpl.hpp"

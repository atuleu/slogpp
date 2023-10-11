#pragma once

#include <string>

namespace slog {

using Buffer = std::string;

class RecordBase;

using Formatter = void (*)(const RecordBase &record, Buffer &);

void RecordToJSON(const RecordBase &record, Buffer &);

void RecordToText(const RecordBase &record, Buffer &);

} // namespace slog

#include "FormattersImpl.hpp"

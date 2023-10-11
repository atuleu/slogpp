#pragma once

#include <functional>
#include <iostream>

namespace slog {

class RecordBase;

using Formatter = void (*)(const RecordBase &record, std::string &);

void RecordToJSON(const RecordBase &record, std::string &);

void RecordToText(const RecordBase &record, std::string &);

} // namespace slog

#include "FormattersImpl.hpp"

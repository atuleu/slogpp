#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace slog {
struct Attribute;

using Value = std::variant<bool, int64_t, double, std::string,
                           std::unique_ptr<std::vector<Attribute>>>;

struct Attribute {
	std::string key;
	Value       value;
};

} // namespace slog

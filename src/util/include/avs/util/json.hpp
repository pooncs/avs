#pragma once
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <map>
#include <cstdint>
#include <optional>

// Minimal JSON DOM + parser/dumper (C++17). No external deps.
// Supports: null, bool, int64, double, string, array, object.
// Comments are not allowed. Keep code small and dependency-free.

namespace avs { namespace json {

struct Value;
using Array = std::vector<Value>;
using Object = std::map<std::string, Value>;

struct Value {
    using Storage = std::variant<std::nullptr_t, bool, int64_t, double, std::string, Array, Object>;
    Storage data{};

    Value() : data(nullptr) {}
    Value(std::nullptr_t) : data(nullptr) {}
    Value(bool v) : data(v) {}
    Value(int64_t v) : data(v) {}
    Value(double v) : data(v) {}
    Value(std::string v) : data(std::move(v)) {}
    Value(const char* s) : data(std::string(s)) {}
    Value(Array v) : data(std::move(v)) {}
    Value(Object v) : data(std::move(v)) {}

    bool is_null() const { return std::holds_alternative<std::nullptr_t>(data); }
    bool is_bool() const { return std::holds_alternative<bool>(data); }
    bool is_int() const { return std::holds_alternative<int64_t>(data); }
    bool is_double() const { return std::holds_alternative<double>(data); }
    bool is_string() const { return std::holds_alternative<std::string>(data); }
    bool is_array() const { return std::holds_alternative<Array>(data); }
    bool is_object() const { return std::holds_alternative<Object>(data); }

    const Array& as_array() const { return std::get<Array>(data); }
    Array& as_array() { return std::get<Array>(data); }
    const Object& as_object() const { return std::get<Object>(data); }
    Object& as_object() { return std::get<Object>(data); }
    const std::string& as_string() const { return std::get<std::string>(data); }
    std::string& as_string() { return std::get<std::string>(data); }
    int64_t as_int() const { return std::get<int64_t>(data); }
    double as_double() const { return std::holds_alternative<double>(data) ? std::get<double>(data) : static_cast<double>(std::get<int64_t>(data)); }
    bool as_bool() const { return std::get<bool>(data); }
};

// Parse JSON text into Value. Returns true on success. On failure, returns false and sets err.
bool parse(std::string_view text, Value& out, std::string* err = nullptr);

// Dump Value to JSON string. indent<0 -> no pretty print.
std::string dump(const Value& v, int indent = 2);

}} // namespace avs::json

#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <ios>
#include <unordered_map>

namespace json {

class Node;
// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
   /* Реализуйте Node, используя std::variant */
    using Value = std::variant<std::nullptr_t, std::string, int, double, bool, Array, Dict>;

    Node() = default;
    Node(std::nullptr_t): Node() {}

    Node(std::string value): value_(std::move(value)) {}

    Node(int value): value_(value) {}

    Node(double value): value_(value) {}

    Node(bool value): value_(value) {}

    Node(Array array): value_(std::move(array)) {}

    Node(Dict map): value_(std::move(map)) {}


    bool IsInt() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;

    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const;
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;

    const Value& GetValue() const;

    bool operator==(const Node& rhs) const;
    bool operator!=(const Node& rhs) const;

private:
    Value value_;
};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

    bool operator==(const Document& rhs) const;
    bool operator!=(const Document& rhs) const;
    
private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}  // namespace json
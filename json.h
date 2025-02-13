#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

class Node;
class Document;

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
    using Array = std::vector<Node>;
    using Dict = std::map<std::string, Node>;
    using Value = std::variant<std::nullptr_t, int , double, std::string, bool, Array, Dict>;

    Node();

    template <typename T>
    Node(T value) : value_(std::move(value)) {}

    Node(const Document& doc);

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
private:
    Value value_;
};

bool operator==(const Node& lhs, const Node& rhs);
bool operator!=(const Node& lhs, const Node& rhs);

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

private:
    Node root_;
};

Document Load(std::istream& input);

std::ostream& Print(const Document& doc, std::ostream& output);

}  // namespace json

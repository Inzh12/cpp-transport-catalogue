#include "json.h"

#include <cmath>
#include <span>

using namespace std;

namespace json {

namespace {

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
    Array result;

    bool closed = false;
    for (char c; input >> c;) {
        if (c == ']') {
            closed = true;
            break;
        }

        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    if(!closed) {
        throw ParsingError{"Array not closed"};
    }

    return Node(std::move(result));
}

int CharToInt(char c) {
    return c - '0';
}

double ReadExponent(istream& input) {
    if (input.peek() != 'e' && input.peek() != 'E') {
        return 1.0;
    } else {
        input.get();
    }

    int grade_sign_muliplier = 1;
    char sign = input.peek();
    if(sign == '-') {
        grade_sign_muliplier = -1;
        input.get();
    } else if (sign == '+'){
        input.get();
    }

    int grade = 0;
    while (isdigit(input.peek())) {
        grade *= 10;
        grade += CharToInt(input.get());
    }

    return pow(10, grade * grade_sign_muliplier);
}

Node LoadNumber(istream& input) {
    int result = 0;
    double double_result = 0;
    int sign_muliplier = 1;

    if(input.peek() == '-') {
        sign_muliplier = -1;
        input.get();
    }

    if (input.peek() != '0') {
        while (isdigit(input.peek())) {
            result *= 10;
            result += CharToInt(input.get());
        }
    } else {
        input.get();
    }

    if (input.peek() != '.') {
        if(input.peek() == 'e' || input.peek() == 'E') {
            double exponent = ReadExponent(input);
            return Node(result * sign_muliplier * exponent);
        } else {
            return Node(result * sign_muliplier);
        }
    } else {
        input.get();
    }

    double_result = result;
    double digit_multiplier = 0.1;
    while (isdigit(input.peek())) {
        double_result += (input.get() - '0') * digit_multiplier;
        digit_multiplier *= 0.1;
    }

    return Node(double_result * sign_muliplier * ReadExponent(input));
}

Node LoadString(istream& input) {
    std::string result;

    char symbol = 0;
    bool escape_mark = false;
    bool closed = false;

    while(input.get(symbol)) {
        if(escape_mark) {
            switch (symbol) {
            case '"':
                result.push_back('"');
                break;
            case 'n':
                result.push_back('\n');
                break;
            case 'r':
                result.push_back('\r');
                break;
            case 't':
                result.push_back('\t');
                break;
            case '\\':
                result.push_back('\\');
                break;
            default:
                result.push_back('\\');
                result.push_back(symbol);
                break;
            }

            escape_mark = false;
        } else {
            if (symbol == '"') {
                closed = true;
                break;
            } else if (symbol == '\\') {
                escape_mark = true;
            } else {
                result.push_back(symbol);
            }
        }
    }

    if(!closed) {
        throw ParsingError{"String not closed"};
    }

    return Node(result);
}

Node LoadDict(istream& input) {
    Dict result;

    bool closed = false;
    for (char c; input >> c;) {
        if(c == '}') {
            closed = true;
            break;
        }

        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input).AsString();
        input >> c;
        result.insert({std::move(key), LoadNode(input)});
    }

    if(!closed) {
        throw ParsingError{"Dict not closed"};
    }

    return Node(std::move(result));
}

Node LoadValue(istream& input) {
    std::string value;

    char symbol;
    while(input.get(symbol)) {
        if (symbol == ',' || symbol == ']' || symbol == '}') {
            input.putback(symbol);
            break;
        }

        if (symbol != ' ' && symbol != '\t' && symbol != '\r' && symbol != '\n' && symbol != '\t') {
            value += symbol;
        }
    }

    if (value == "true") {
        return Node{true};
    } else if (value == "false") {
        return Node{false};
    } else if (value == "null") {
        return Node{nullptr};
    }

    throw ParsingError{"Unknown token"};
}

Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == '[') {
        return LoadArray(input);
    } else if (c == '{') {
        return LoadDict(input);
    } else if (c == '"') {
        return LoadString(input);
    } else if (c == '+' || c == '-' || isdigit(c)) {
        input.putback(c);
        return LoadNumber(input);
    } else {
        input.putback(c);
        return LoadValue(input);
    }
}

}  // namespace

Node::Node() : value_(nullptr) {}

Node::Node(const Document &doc) {
    *this = doc.GetRoot();
}

bool Node::IsInt() const {
    return std::holds_alternative<int>(value_);
}

bool Node::IsPureDouble() const {
     return std::holds_alternative<double>(value_);
}

bool Node::IsDouble() const {
    return IsInt() || IsPureDouble();
}

bool Node::IsBool() const {
    return std::holds_alternative<bool>(value_);
}

bool Node::IsString() const {
    return std::holds_alternative<std::string>(value_);
}

bool Node::IsNull() const {
    return std::holds_alternative<std::nullptr_t>(value_);
}

bool Node::IsArray() const {
    return std::holds_alternative<Array>(value_);
}

bool Node::IsMap() const {
    return std::holds_alternative<Dict>(value_);
}

int Node::AsInt() const {
    if(!IsInt()) throw std::logic_error{""};
    return std::get<int>(value_);
}

bool Node::AsBool() const {
    if(!IsBool()) throw std::logic_error{""};
    return std::get<bool>(value_);
}

double Node::AsDouble() const {
    // if(IsPureDouble()) {
    //     return std::round(std::get<double>(value_) * 1000000) / 1000000;
    // }

    if (IsInt()) {
        return static_cast<double>(std::get<int>(value_));
    }

    return std::get<double>(value_);

    throw std::logic_error{""};
}

const Node::Value& Node::GetValue() const {
    return value_;
}

const string &Node::AsString() const {
    if(!IsString()) throw std::logic_error{""};
    return std::get<std::string>(value_);
}

const Node::Array &Node::AsArray() const {
    if(!IsArray()) throw std::logic_error{""};
    return std::get<Array>(value_);
}

const Node::Dict &Node::AsMap() const {
    if(!IsMap()) throw std::logic_error{""};
    return std::get<Dict>(value_);
}

Document::Document(Node root)
    : root_(std::move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

std::ostream& PrintString(std::string_view str, std::ostream& output) {
    std::string escape_string;
    escape_string += '\"';

    for(char symbol : str) {
        switch (symbol) {
        case '"':
            escape_string += R"(\")";
            break;
        case '\n':
            escape_string += R"(\n)";
            break;
        case '\r':
            escape_string += R"(\r)";
            break;
        case '\t':
            escape_string += R"(\t)";
            break;
        case '\\':
            escape_string += R"(\\)";
            break;
        default:
            escape_string += symbol;
            break;
        }
    }

    escape_string += '\"';
    output << escape_string;

    return output;
}

std::ostream& PrintNode(const Node node, std::ostream& output);

std::ostream& PrintArray(const Array& array, std::ostream& output) {
    output << '[';

    if(!array.empty()) {
        PrintNode(array.front(), output);

        for (const Node& array_node : std::span(array.begin() + 1, array.end())) {
            output << ',';
            PrintNode(array_node, output);
        }
    }

    output << ']';

    return output;
}


std::ostream& PrintMap(const Dict& map, std::ostream& output) {
    output << '{';

    if(!map.empty()) {
        const std::string& first_key = map.begin()->first;
        const Node& first_node = map.begin()->second;
        PrintNode(Node{first_key}, output);
        output << ':';
        PrintNode(first_node, output);

        for (auto it = ++map.begin(); it != map.end(); ++it) {
            const std::string& key = it->first;
            const Node& node = it->second;

            output << ',';
            PrintNode(Node{key}, output);
            output << ':';
            PrintNode(node, output);
        }
    }

    output << '}';

    return output;
}

std::ostream& PrintNode(const Node node, std::ostream& output) {
    if (node.IsNull()) {
        output << "null";
    } else if (node.IsInt()) {
        output << node.AsInt();
    } else if (node.IsPureDouble()) {
        output << node.AsDouble();
    } else if (node.IsString()) {
        PrintString(node.AsString(), output);
    } else if (node.IsBool()) {
        output << boolalpha << node.AsBool();
    } else if (node.IsArray()) {
        PrintArray(node.AsArray(), output) << '\n';
    } else if (node.IsMap()) {
        PrintMap(node.AsMap(), output) << '\n';
    }

    return output;
}

std::ostream& Print(const Document& doc, std::ostream& output) {
    PrintNode(doc.GetRoot(), output);
    return output;
}

bool operator==(const Node &lhs, const Node &rhs) {

    if(lhs.IsPureDouble() && rhs.IsPureDouble()) {
        return std::fabs(lhs.AsDouble() - rhs.AsDouble()) <= 1e-9;
    }

    return lhs.GetValue() == rhs.GetValue();
}

bool operator!=(const Node &lhs, const Node &rhs) {
    return !(lhs == rhs);
}

}  // namespace json

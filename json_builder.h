#pragma once

#include <stack>
#include "json.h"
#include <optional>

namespace json {

class Builder {
public:
    class KeyRef;
    class ArrayRef;

    class DictRef {
    public:
        DictRef(Builder& builder);

        KeyRef Key(const std::string& key);
        Builder& EndDict();
    private:
        Builder& builder_;
    };

    class KeyRef {
    public:
        KeyRef(Builder& builder);

        DictRef Value(const json::Node& value);
        DictRef StartDict();
        ArrayRef StartArray();
    private:
        Builder& builder_;
    };

    class ArrayRef {
    public:
        ArrayRef(Builder& builder);

        ArrayRef Value(const json::Node& value);
        ArrayRef StartArray();
        DictRef StartDict();
        Builder& EndArray();
    private:
        Builder& builder_;
    };

    Builder& Value(const json::Node& value);
    Builder& Key(const std::string& key);
    DictRef StartDict();
    Builder& EndDict();
    ArrayRef StartArray();
    Builder& EndArray();
    json::Node Build();
private:
    bool PushToStack(const json::Node& node);

    json::Node root_ = nullptr;
    std::stack<Node*> stack_;
    std::optional<std::string> current_key_;
};

}

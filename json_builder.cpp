#include "json_builder.h"

namespace json {

json::Builder& json::Builder::Value(const json::Node& value) {
    if(root_ == nullptr) {
        root_ = value;
    } else if (!stack_.empty() && stack_.top()->IsArray()) {
        Array& array = stack_.top()->AsArray();
        array.insert(array.end(), value);
    } else if (!stack_.empty() && stack_.top()->IsDict() && current_key_) {
        Dict& dict = stack_.top()->AsDict();
        dict.at(current_key_.value()) = value;
        current_key_= std::nullopt;
    } else {
        throw std::logic_error("Can't put value here");
    }

    return *this;
}

Builder& Builder::Key(const std::string& key) {
    if (!stack_.empty() && stack_.top()->IsDict() && !current_key_) {
         Dict& dict = stack_.top()->AsDict();
         dict[key] = nullptr;
         current_key_ = key;
    } else {
        throw std::logic_error("Can't put key here");
    }

    return *this;
}

DictRef Builder::StartDict() {
    if(root_ == nullptr) {
        root_ = Dict{};
        stack_.push(&root_);
    } else if (!stack_.empty() && stack_.top()->IsArray()) {
        Array& array = stack_.top()->AsArray();
        auto dict_node = array.insert(array.end(), Dict{});
        stack_.push(dict_node.base());
    } else if (!stack_.empty() && stack_.top()->IsDict() && current_key_) {
        Dict& dict = stack_.top()->AsDict();
        dict.at(current_key_.value()) = Dict{};
        stack_.push(&dict.at(current_key_.value()));
        current_key_= std::nullopt;
    } else {
        throw std::logic_error("Can't put dict here");
    }

    return *this;
}

Builder &Builder::EndDict() {
    if (!stack_.empty() && stack_.top()->IsDict()) {
        stack_.pop();
    } else {
        throw std::logic_error("No dict to close");
    }

    return *this;
}

ArrayRef Builder::StartArray() {
    if(root_ == nullptr) {
        root_ = Array{};
        stack_.push(&root_);
    } else if (!stack_.empty() && stack_.top()->IsArray()) {
        Array& array = stack_.top()->AsArray();
        auto array_node = array.insert(array.end(), Array{});
        stack_.push(array_node.base());
    } else if (!stack_.empty() && stack_.top()->IsDict() && current_key_) {
        Dict& dict = stack_.top()->AsDict();
        dict.at(current_key_.value()) = Array{};
        stack_.push(&dict.at(current_key_.value()));
        current_key_= std::nullopt;
    } else {
        throw std::logic_error("Can't put array here");
    }

    return *this;
}

Builder &Builder::EndArray() {
    if (!stack_.empty() && stack_.top()->IsArray()) {
        stack_.pop();
    } else {
        throw std::logic_error("No array to close");
    }

    return *this;
}

Node Builder::Build() {
    if(root_ == nullptr || !stack_.empty()) {
        throw std::logic_error("Json document is not completed");
    }

    return root_;
}

DictRef::DictRef(Builder &builder)
    : builder_(builder) {}

KeyRef DictRef::Key(const std::string &key) {
    return builder_.Key(key);
}

Builder& DictRef::EndDict() {
    return builder_.EndDict();
}

KeyRef::KeyRef(Builder &builder)
    : builder_(builder) {}


ArrayRef KeyRef::StartArray() {
    return builder_.StartArray();
}

DictRef KeyRef::Value(const Node &value) {
    return builder_.Value(value);
}

DictRef KeyRef::StartDict() {
    return builder_.StartDict();
}

ArrayRef::ArrayRef(Builder &builder)
    : builder_(builder) {}

ArrayRef ArrayRef::Value(const Node &value) {
    return builder_.Value(value);
}

ArrayRef ArrayRef::StartArray() {
    return builder_.StartArray();
}

DictRef ArrayRef::StartDict() {
    return builder_.StartDict();
}

Builder &ArrayRef::EndArray() {
    return builder_.EndArray();
}

}



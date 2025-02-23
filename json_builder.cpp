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

Builder::DictRef Builder::StartDict() {
    if(!PushToStack(Dict{})) {
        throw std::logic_error("Can't put array here");
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

Builder::ArrayRef Builder::StartArray() {
    if(!PushToStack(Array{})) {
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

bool Builder::PushToStack(const Node& node)
{
    if(root_ == nullptr) {
        root_ = node;
        stack_.push(&root_);
    } else if (!stack_.empty() && stack_.top()->IsArray()) {
        Array& array = stack_.top()->AsArray();
        auto array_node = array.insert(array.end(), node);
        stack_.push(array_node.base());
    } else if (!stack_.empty() && stack_.top()->IsDict() && current_key_) {
        Dict& dict = stack_.top()->AsDict();
        dict.at(current_key_.value()) = node;
        stack_.push(&dict.at(current_key_.value()));
        current_key_= std::nullopt;
    } else {
        return false;
    }

    return true;
}

Builder::DictRef::DictRef(Builder &builder)
    : builder_(builder) {}

Builder::KeyRef Builder::DictRef::Key(const std::string &key) {
    return builder_.Key(key);
}

Builder& Builder::DictRef::EndDict() {
    return builder_.EndDict();
}

Builder::KeyRef::KeyRef(Builder &builder)
    : builder_(builder) {}

Builder::ArrayRef Builder::KeyRef::StartArray() {
    return builder_.StartArray();
}

Builder::DictRef Builder::KeyRef::Value(const Node &value) {
    return builder_.Value(value);
}

Builder::Builder::DictRef Builder::KeyRef::StartDict() {
    return builder_.StartDict();
}

Builder::ArrayRef::ArrayRef(Builder &builder)
    : builder_(builder) {}

Builder::ArrayRef Builder::ArrayRef::Value(const Node &value) {
    return builder_.Value(value);
}

Builder::ArrayRef Builder::ArrayRef::StartArray() {
    return builder_.StartArray();
}

Builder::DictRef Builder::ArrayRef::StartDict() {
    return builder_.StartDict();
}

Builder &Builder::ArrayRef::EndArray() {
    return builder_.EndArray();
}

}



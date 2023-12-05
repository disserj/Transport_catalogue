#include "json_builder.h"

namespace json {

Builder::Builder() {
    Node* to_back = &root_;
    nodes_stack_.emplace_back(to_back);
}

DictKeyContext Builder::Key(std::string key) {
    Node* top = nodes_stack_.back();
    if (top->IsMap() && !key_) key_ = std::move(key);
    else throw std::logic_error("Wrong key");
    return *this;
}

Builder& Builder::Value(Node::Value value) {
    Node* top = nodes_stack_.back();

    if (top->IsMap()) {
        if (!key_) throw std::logic_error("Value without key");
        Dict& dict = std::get<Dict>(top->GetValue());
        auto [it, flag] = dict.emplace(std::move(key_.value()), Node{});
        key_ = std::nullopt;
        top = &it->second;
        top->GetValue() = std::move(value);
    }
    else if (top->IsArray()) {
        Array& array = std::get<Array>(top->GetValue());
        array.emplace_back(GetNode(value));
        top = &array.back();
    }
    else if (root_.IsNull()) {
        root_.GetValue() = std::move(value);
    }
    else throw std::logic_error("Unknown container");

    return *this;
}

DictItemContext Builder::StartDict() {
    Node* top = nodes_stack_.back();

    if (top->IsMap()) {
        if (!key_) throw std::logic_error("Dictionary without key");
        Dict& dict = std::get<Dict>(top->GetValue());
        auto [it, flag] = dict.emplace(std::move(key_.value()), Dict());
        key_ = std::nullopt;
        nodes_stack_.emplace_back(&it->second);
    }
    else if (top->IsArray()) {
        Array& array = std::get<Array>(top->GetValue());
        array.emplace_back(Dict());
        nodes_stack_.emplace_back(&array.back());
    }
    else if (top->IsNull()) {
        top->GetValue() = Dict();
    }
    else throw std::logic_error("Wrong previous Node");

    return *this;
}

Builder& Builder::EndDict() {
    Node* top = nodes_stack_.back();
    if (!top->IsMap()) throw std::logic_error("Previous Node is not a Dict");
    nodes_stack_.pop_back();
    return *this;
}

ArrayItemContext Builder::StartArray() {
    Node* top = nodes_stack_.back();

    if (top->IsMap()) {
        if (!key_) throw std::logic_error("No key");
        Dict& dict = std::get<Dict>(top->GetValue());
        auto [it, flag] = dict.emplace(std::move(key_.value()), Array());
        key_ = std::nullopt;
        nodes_stack_.emplace_back(&it->second);
    }
    else if (top->IsArray()) {
        Array& array = std::get<Array>(top->GetValue());
        array.emplace_back(Array());
        nodes_stack_.emplace_back(&array.back());
    }
    else if (top->IsNull()) {
        top->GetValue() = Array();
    }
    else throw std::logic_error("Wrong previous Node");

    return *this;
}

Builder& Builder::EndArray() {
    Node* top = nodes_stack_.back();
    if (!top->IsArray()) throw std::logic_error("Wrong previous Node");
    nodes_stack_.pop_back();
    return *this;
}

Node Builder::Build() {
    if (root_.IsNull() || nodes_stack_.size() > 1) throw std::logic_error("Wrong Build()");
    return root_;
}

Node Builder::GetNode(Node::Value value) {
    if (std::holds_alternative<int>(value)) return Node(std::get<int>(value));
    if (std::holds_alternative<double>(value)) return Node(std::get<double>(value));
    if (std::holds_alternative<std::string>(value)) return Node(std::get<std::string>(value));
    if (std::holds_alternative<std::nullptr_t>(value)) return Node(std::get<std::nullptr_t>(value));
    if (std::holds_alternative<bool>(value)) return Node(std::get<bool>(value));
    if (std::holds_alternative<Dict>(value)) return Node(std::get<Dict>(value));
    if (std::holds_alternative<Array>(value)) return Node(std::get<Array>(value));
    return {};
}


DictKeyContext DictItemContext::Key(std::string&& key) {
    return builder_.Key(std::move(key));
}

Builder& DictItemContext::EndDict() {
    return builder_.EndDict();
}



ArrayItemContext ArrayItemContext::Value(Node::Value value) {
    return ArrayItemContext(builder_.Value(value));
}

DictItemContext BaseContext::StartDict() {
    return builder_.StartDict();
}

ArrayItemContext BaseContext::StartArray() {
    return builder_.StartArray();
}

Builder& ArrayItemContext::EndArray() {
    return builder_.EndArray();
}



DictItemContext DictKeyContext::Value(Node::Value value) {
    return DictItemContext(builder_.Value(value));
}


} // exit json
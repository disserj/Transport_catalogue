#pragma once

#include "json.h"

#include <optional>

namespace json {

class DictItemContext;
class DictKeyContext;
class ArrayItemContext;

class Builder {
public:
    Builder();
    DictKeyContext Key(std::string key);
    Builder& Value(Node::Value value);
    DictItemContext StartDict();
    Builder& EndDict();
    ArrayItemContext StartArray();
    Builder& EndArray();
    Node Build();
    Node GetNode(Node::Value value);

private:
    Node root_ = nullptr ;
    std::vector<Node*> nodes_stack_;
    std::optional<std::string> key_ =  std::nullopt ;
};
    

class BaseContext  {
public:    
    BaseContext(Builder& builder)
        : builder_(builder)
    {}

    ArrayItemContext StartArray();
    DictItemContext StartDict();
    
    virtual ~BaseContext()=default;
protected:
    Builder& builder_;
};

class DictItemContext : public BaseContext {
public:
    DictItemContext(Builder& builder) 
        : BaseContext(builder) {} 

    DictKeyContext Key(std::string&& key) ;
    Builder& EndDict() ;
    
    ArrayItemContext StartArray()=delete;
    DictItemContext StartDict()=delete;

};

class ArrayItemContext : public BaseContext {
public:
    ArrayItemContext(Builder& builder) : BaseContext(builder) {}
    
    ArrayItemContext Value(Node::Value value);
    Builder& EndArray();

};

class DictKeyContext : public BaseContext{
public:
    DictKeyContext(Builder& builder): BaseContext (builder) {}

    DictItemContext Value(Node::Value value);
};

} // exit json
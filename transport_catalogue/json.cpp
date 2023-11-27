#include "json.h"

using namespace std;

namespace json {

namespace {

Node LoadNode(istream& input);

Node LoadNull(istream& input) {
    std::string line;
		while (std::isalpha(input.peek()))
		{
			line.push_back(static_cast<char>(input.get()));
		}
		if (line != "null")
		{
			throw ParsingError("null error"s);
		}
		return Node(nullptr);
}

std::string LoadString(std::istream& input) {

    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            ++it;
            break;
        }
        else if (ch == '\\') {
            
            ++it;
            if (it == end) {
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);

            switch (escaped_char) {
            case '\"':
                s.push_back('\"');
                break;
                    // reverse solidus
           case '\\':
                s.push_back('\\');
                break;
                    // solidus
            case '/':
                s.push_back('/');
                break;
                    // backspace
            case 'b':
                s.push_back('\b');
                break;
                    // form feed
            case 'f':
                s.push_back('\f');
                break;
                    // line feed
            case 'n':
                s.push_back('\n');
                break;
                    // carriage return
            case 'r':
                s.push_back('\r');
                break;
                    // tab
            case 't':
                s.push_back('\t');
                break;

            default:
                throw ParsingError("Unrecognized escape sequence \\" + escaped_char);
            }
        }
        else if (ch == '\n' || ch == '\r') {
            throw ParsingError("Unexpected end of line");
        }
        else {
            s.push_back(ch);
        }
        ++it;
    }

    return s;
}

Node LoadNumber(std::istream& input) {

    std::string parsed_num;
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number ");
        }
    };

    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }

    if (input.peek() == '0') {
        read_char();

    }
    else {
        read_digits();
    }

    bool is_int = true;

    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            
            try {
                return std::stoi(parsed_num);
            }
            catch (...) {
                
            }
        }
        return std::stod(parsed_num);
    }
    catch (...) {
        throw ParsingError("Failed to convert " + parsed_num + " to number");
    }
}

Node LoadBool(istream& input) {
    std::string line;
    while (std::isalpha(input.peek()))
    {
        line.push_back(static_cast<char>(input.get()));
    }
    if (line == "true")
    {
        return Node{ true };
    }
    else if (line == "false")
    {
        return Node{ false };
    }
    else
    {
        throw ParsingError("Bool error");
    }
}

Node LoadArray(istream& input) {
    Array result;
    if (input.peek() == -1) throw ParsingError("Array parsing error");

    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    return Node(std::move(result));
}

Node LoadDict(istream& input) {
    Dict result;
    if (input.peek() == -1) throw ParsingError("Array parsing error");

    for (char c; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input);
        input >> c;
        result.insert({ std::move(key), LoadNode(input) });
    }

    return Node(std::move(result));
}

Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == 'n') {
        input.putback(c);
        return LoadNull(input);
    }
    else if (c == '"') {
        return LoadString(input);
    }
    else if (c == 't' || c == 'f') {
        input.putback(c);
        return LoadBool(input);
    }
    else if (c == '[') {
        return LoadArray(input);
    }
    else if (c == '{') {
        return LoadDict(input);
    }
    else {
        input.putback(c);
        return LoadNumber(input);
    }
}
 

}  // namespace

//-----------Node: 
bool Node::IsInt() const {
    return holds_alternative<int>(value_);
}

bool Node::IsDouble() const {
    return holds_alternative<double>(value_) || holds_alternative<int>(value_);
}

bool Node::IsPureDouble() const {
    return holds_alternative<double>(value_);
}

bool Node::IsBool() const {
    return holds_alternative<bool>(value_);
}

bool Node::IsString() const {
    return holds_alternative<std::string>(value_);
}

bool Node::IsNull() const {
    return holds_alternative<std::nullptr_t>(value_);
}

bool Node::IsArray() const {
    return holds_alternative<Array>(value_);
}

bool Node::IsMap() const {
    return holds_alternative<Dict>(value_);
}

int Node::AsInt() const {
    if (!IsInt()) throw std::logic_error("wrong type");
    return std::get<int>(value_);
}

bool Node::AsBool() const {
    if (!IsBool()) throw std::logic_error("wrong type");
    return std::get<bool>(value_);
}

double Node::AsDouble() const {
    if (!IsDouble()) throw std::logic_error("wrong type");
    if (IsInt()) return static_cast<double>(std::get<int>(value_));
    return std::get<double>(value_);
}

const std::string& Node::AsString() const {
    if (!IsString()) throw std::logic_error("wrong type");
    return std::get<std::string>(value_);
}

const Array& Node::AsArray() const {
    if (!IsArray()) throw std::logic_error("wrong type");
    return std::get<Array>(value_);
}

const Dict& Node::AsMap() const {
    if (!IsMap()) throw std::logic_error("wrong type");
    return std::get<Dict>(value_);
}

const Node::Value& Node::GetValue() const {
    return value_;
}

bool Node::operator==(const Node& rhs) const {
    return value_ == rhs.value_;
}

bool Node::operator!=(const Node& rhs) const {
    return !(value_ == rhs.value_);
}

    
//-----------Document:
Document::Document(Node root)
    : root_(std::move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

bool Document::operator==(const Document& rhs) const {
    return root_ == rhs.root_;
}

bool Document::operator!=(const Document& rhs) const {
    return !(root_ == rhs.root_);
}


Document Load(istream& input) {
        return Document{LoadNode(input)};
    }

//-----------Print:

struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }
    PrintContext Indented() const {
        return { out, indent_step, indent_step + indent };
    }
};

struct ValuePrinter {
    std::ostream& out;
    void operator()(std::nullptr_t) {
        out << "null";
    }
    void operator()(std::string value){
        out << "\"";
        for (const char& c : value) {
            if (c == '\n') {
                out << "\\n";
                continue;
            }
            if (c == '\r') {
                out << "\\r";
                continue;
            }
            if (c == '\"') out << "\\";
            if (c == '\t') {
                out << "\\t";
                continue;
            }
            if (c == '\\') out << "\\";
            out << c;
        }
        out << "\"";
    }
    void operator()(int value){
        out << value;
    }
    void operator()(double value){
        out << value;
    }
    void operator()(bool value){
        out << std::boolalpha << value;
    }
    void operator()(Array array){
        out << "[";
        bool first = true;
        for (const auto& elem : array) {
            if (first) first = false;
            else out << ", ";

            std::visit(ValuePrinter{ out }, elem.GetValue());
        }
        out << "]";
    }
    void operator()(Dict dict){
        out << "{ ";
        bool first = true;
        for (auto& [key, node] : dict) {
            if (first) first = false;
            else out << ", ";
            out << "\"" << key << "\": ";
            std::visit(ValuePrinter{ out }, node.GetValue());
        }
        out << " }";
    }
};

void Print(const Document& doc, std::ostream& out) {
    std::visit(ValuePrinter{ out }, doc.GetRoot().GetValue());
}
    // Реализуйте функцию самостоятельно


}  // namespace json
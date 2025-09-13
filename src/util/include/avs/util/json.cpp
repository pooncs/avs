#include "avs/util/json.hpp"
#include <cctype>
#include <sstream>
#include <iomanip>

namespace avs { namespace json {

namespace {
struct Reader {
    std::string_view s; size_t i = 0; std::string* err = nullptr;
    bool eof() const { return i >= s.size(); }
    char peek() const { return eof()? '\0' : s[i]; }
    char get() { return eof()? '\0' : s[i++]; }
    void skip_ws(){ while(!eof() && (s[i]==' '||s[i]=='\n'||s[i]=='\r'||s[i]=='\t')) ++i; }
    bool expect(char c){ skip_ws(); if(peek()!=c){ fail(std::string("expected '")+c+"'"); return false;} ++i; return true; }
    [[noreturn]] void fail(const std::string& m){ if(err) *err = m; throw 1; }
};

Value parse_value(Reader& R);

std::string parse_string(Reader& R){
    if(!R.expect('\"')) return {};
    std::string out; out.reserve(16);
    while(!R.eof()){
        char c = R.get();
        if(c=='\"') break;
        if(c=='\\'){
            char e = R.get();
            switch(e){
                case '"': out.push_back('"'); break;
                case '\\': out.push_back('\\'); break;
                case '/': out.push_back('/'); break;
                case 'b': out.push_back('\b'); break;
                case 'f': out.push_back('\f'); break;
                case 'n': out.push_back('\n'); break;
                case 'r': out.push_back('\r'); break;
                case 't': out.push_back('\t'); break;
                case 'u': {
                    // read 4 hex digits
                    unsigned code = 0; for(int k=0;k<4;++k){ char h = R.get(); if(!isxdigit((unsigned char)h)) R.fail("bad \\uXXXX"); code = code*16 + (h>='0'&&h<='9'? h-'0' : (h>='a'&&h<='f'? 10+h-'a' : 10+h-'A')) ; }
                    // encode as UTF-8
                    if(code <= 0x7F) out.push_back((char)code);
                    else if(code <= 0x7FF){ out.push_back((char)(0xC0 | (code>>6))); out.push_back((char)(0x80 | (code & 0x3F))); }
                    else { out.push_back((char)(0xE0 | (code>>12))); out.push_back((char)(0x80 | ((code>>6)&0x3F))); out.push_back((char)(0x80 | (code & 0x3F))); }
                } break;
                default: R.fail("bad escape");
            }
        } else {
            out.push_back(c);
        }
    }
    return out;
}

Value parse_number(Reader& R){
    R.skip_ws(); size_t start = R.i; bool has_dot=false, has_exp=false;
    if(R.peek()=='-' ) R.get();
    while(!R.eof()){
        char c = R.peek();
        if(c=='.'){ has_dot=true; R.get(); }
        else if(c=='e'||c=='E'){ has_exp=true; R.get(); if(R.peek()=='+'||R.peek()=='-') R.get(); while(!R.eof() && isdigit((unsigned char)R.peek())) R.get(); break; }
        else if(isdigit((unsigned char)c)) R.get();
        else break;
    }
    std::string num(R.s.substr(start, R.i-start));
    if(has_dot || has_exp){
        try { return Value(std::stod(num)); } catch(...) { R.fail("bad number"); }
    } else {
        try { long long v = std::stoll(num); return Value((int64_t)v); } catch(...) { R.fail("bad integer"); }
    }
    return Value();
}

Value parse_array(Reader& R){
    Value::Storage::index_type dummy=0; // silence warnings
    if(!R.expect('[')) return {};
    Array arr;
    R.skip_ws();
    if(R.peek()==']'){ R.get(); return Value(std::move(arr)); }
    while(true){
        Value v = parse_value(R); arr.push_back(std::move(v));
        R.skip_ws();
        if(R.peek()==','){ R.get(); continue; }
        if(R.peek()==']'){ R.get(); break; }
        R.fail("expected ',' or ']' in array");
    }
    return Value(std::move(arr));
}

Value parse_object(Reader& R){
    if(!R.expect('{')) return {};
    Object obj;
    R.skip_ws();
    if(R.peek()=='}'){ R.get(); return Value(std::move(obj)); }
    while(true){
        R.skip_ws();
        if(R.peek()!='\"') R.fail("expected string key");
        std::string key = parse_string(R);
        R.skip_ws();
        if(!R.expect(':')) return {};
        Value val = parse_value(R);
        obj.emplace(std::move(key), std::move(val));
        R.skip_ws();
        if(R.peek()==','){ R.get(); continue; }
        if(R.peek()=='}'){ R.get(); break; }
        R.fail("expected ',' or '}' in object");
    }
    return Value(std::move(obj));
}

Value parse_value(Reader& R){
    R.skip_ws();
    char c = R.peek();
    if(c=='{') return parse_object(R);
    if(c=='[') return parse_array(R);
    if(c=='\"') return Value(parse_string(R));
    if(c=='t'){ // true
        const char* k = "true"; for(int j=0;j<4;++j){ if(R.get()!=k[j]) R.fail("bad literal"); }
        return Value(true);
    }
    if(c=='f'){ const char* k = "false"; for(int j=0;j<5;++j){ if(R.get()!=k[j]) R.fail("bad literal"); } return Value(false); }
    if(c=='n'){ const char* k = "null"; for(int j=0;j<4;++j){ if(R.get()!=k[j]) R.fail("bad literal"); } return Value(nullptr); }
    if(c=='-' || isdigit((unsigned char)c)) return parse_number(R);
    R.fail("unexpected token");
    return Value();
}

void dump_impl(const Value& v, std::ostream& os, int indent, int level){
    auto pad = [&](int lvl){ if(indent>0) for(int i=0;i<lvl*indent;++i) os.put(' '); };
    if(v.is_null()) os << "null";
    else if(v.is_bool()) os << (v.as_bool()?"true":"false");
    else if(v.is_int()) os << v.as_int();
    else if(std::holds_alternative<double>(v.data)){
        os << std::setprecision(15) << v.as_double();
    } else if(v.is_string()){
        os << '\"';
        for(char c: v.as_string()){
            switch(c){
                case '"': os << "\\\""; break; case '\\': os << "\\\\"; break; case '\n': os << "\\n"; break; case '\r': os << "\\r"; break; case '\t': os << "\\t"; break; default: os << c; break; }
        }
        os << '\"';
    } else if(v.is_array()){
        os << '['; if(indent>0) os << '\n';
        const auto& a = v.as_array();
        for(size_t i=0;i<a.size();++i){ if(indent>0) pad(level+1); dump_impl(a[i], os, indent, level+1); if(i+1<a.size()) os << ','; if(indent>0) os << '\n'; }
        if(indent>0) pad(level); os << ']';
    } else if(v.is_object()){
        os << '{'; if(indent>0) os << '\n';
        const auto& o = v.as_object();
        auto it=o.begin();
        while(it!=o.end()){
            if(indent>0) pad(level+1);
            os << '"' << it->first << '"' << ':'; if(indent>0) os << ' ';
            dump_impl(it->second, os, indent, level+1);
            ++it; if(it!=o.end()) os << ','; if(indent>0) os << '\n';
        }
        if(indent>0) pad(level); os << '}';
    }
}

} // anon

bool parse(std::string_view text, Value& out, std::string* err){
    Reader R{ text, 0, err };
    try{
        out = parse_value(R);
        R.skip_ws();
        if(!R.eof()){
            if(err) *err = "trailing characters";
            return false;
        }
        return true;
    } catch(...) {
        return false;
    }
}

std::string dump(const Value& v, int indent){
    std::ostringstream os; dump_impl(v, os, indent, 0); return os.str();
}

}} // namespace avs::json

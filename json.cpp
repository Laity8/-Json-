//
// Created by Shota on 2022/12/27.
//
#include "json.h"

using namespace shotacon::json;

Json::Json() : m_type(json_null) {
    m_value.m_int = 0;
}

Json::Json(bool v) : m_type(json_bool) {
    m_value.m_bool = v;
}

Json::Json(int v) : m_type(json_int) {
    m_value.m_int = v;
}

Json::Json(double v) : m_type(json_double) {
    m_value.m_double = v;
}

Json::Json(const char *v) : m_type(json_string) {

    m_value.mp_string = new std::string(v);
}

Json::Json(const std::string &v) : m_type(json_string){
    m_value.mp_string = new std::string(v);
}

Json::Json(json_type type) {
    m_type = type;
    switch (type) {
        case json_null:
            break;
        case json_bool:
            m_value.m_bool = false;
            break;
        case json_int:
            m_value.m_int = 0;
            break;
        case json_double:
            m_value.m_double = 0.0;
            break;
        case json_string:
            m_value.mp_string = new std::string;
            break;
        case json_array:
            m_value.mp_array = new std::vector<Json>;
            break;
        case json_object:
            m_value.mp_object = new std::map<std::string, Json>;
            break;
        default:
            break;
    }
}

//拷贝构造，对于数组，对象和字符串进行浅拷贝提升效率
Json::Json(const Json &j) {
    shallow_copy(j);
}

Json::operator bool() {
    if (m_type != json_bool) {
        throw std::logic_error("type error, not bool type");
    }
    return m_value.m_bool;
}

Json::operator int() {
    if (m_type != json_int) {
        throw std::logic_error("type error, not int type");
    }
    return m_value.m_int;
}

Json::operator double() {
    if (m_type != json_double) {
        throw std::logic_error("type error, not double type");
    }
    return m_value.m_double;
}

Json::operator std::string() {
    if (m_type != json_string) {
        throw std::logic_error("type error, not string type");
    }
    return *(m_value.mp_string);
}

Json::operator const char *() {
    if (m_type != json_string) {
        throw std::logic_error("type error, not string type");
    }
    return m_value.mp_string->c_str();
}

/// 重载[]操作符访问数组
/// \param index 数组下标
/// \return 数组下标指向的Json数据
Json & Json::operator [] (int index){
    if(m_type != json_array){
        clear();
        m_type = json_array;
        m_value.mp_array = new std::vector<Json>;
    }
    if(index < 0){
        throw std::logic_error("array's index < 0");
    }
    int size = (m_value.mp_array)->size();
//    if(index >= size)
//        throw std::logic_error("index over bound");
    //若下标大于当前容量，进行扩容填充空Json
    if(index >= size){
        for(int i = size; i <= index; ++i){
            (m_value.mp_array)->push_back(Json());
        }
    }
    return (m_value.mp_array)->at(index);
}

//方法
/// 数组添加元素
/// \param other Json数据
void Json::append(const Json & other){
    if(m_type != json_array){
        clear();
        m_type = json_array;
        m_value.mp_array = new std::vector<Json>;
    }
    (m_value.mp_array)->push_back(other);
}

/// 以string返回Json内容
/// \param n 通过记录递归层数来控制制表符的次数
/// \return
std::string Json::get(int n) const {
    int c = n;
    std::stringstream ss;
    switch (m_type) {
        case json_null:
            ss << "null";
            break;
        case json_bool:
            if(m_value.m_bool)
                ss << "true";
            else
                ss << "false";
            break;
        case json_int:
            ss << m_value.m_int;
            break;
        case json_double:
            ss << m_value.m_double;
            break;
        case json_string:
            ss << "\"" << *m_value.mp_string << "\"";
            break;
        case json_array:
            ss << "[\n";
            for(auto it = m_value.mp_array->begin(); it != m_value.mp_array->end(); ++it){
                c = n;
                if(it != (m_value.mp_array)->begin()){
                    ss << ",\n";
                }
                while(c--){
                    ss << "\t";
                }
                ss << "\t" << it->get(n+1);
            }
            c = n;
            ss << "\n";
            while(c--){
                ss << "\t";
            }
            ss << "]";
            break;
        case json_object:
            ss << "{\n";
            for(auto it = m_value.mp_object->begin(); it != m_value.mp_object->end(); ++it){
                c = n;
                if(it != m_value.mp_object->begin()){
                    ss << ",\n";
                }
                while(c--){
                    ss << "\t";
                }
                ss << "\t\"" << it->first << "\"" << ": " << it->second.get(n+1);
            }
            c = n;
            ss << "\n";
            while(c--){
                ss << "\t";
            }
            ss << "}";
            break;
        default:
            break;
    }
    return ss.str();
}

/// 重载Json对象的[]访问符
/// \param key json_object数据的key值，C风格字符串
/// \return
Json & Json::operator [] (const char * key){
    return ((*(this))[std::string(key)]);
}

/// 重载Json对象的[]访问符
/// \param key json_object数据的key值，C++风格字符串
/// \return
Json & Json::operator [] (const std::string & key){
    if(m_type != json_object){
        clear();
        m_type = json_object;
        m_value.mp_object = new std::map<std::string, Json>;
    }
    return ((*m_value.mp_object)[key]);//无key插入，有key修改
}

/// 拷贝函数，使用浅拷贝（使用拷贝函数前，确保自身已经释放）
/// \param j
void Json::shallow_copy(const Json &j) {
    m_type = j.m_type;
    switch (m_type) {
        case json_null:
            break;
        case json_bool:
            m_value.m_bool = j.m_value.m_bool;
            break;
        case json_int:
            m_value.m_int = j.m_value.m_int;
            break;
        case json_double:
            m_value.m_double = j.m_value.m_double;
            break;
        case json_string:
            m_value.mp_string = j.m_value.mp_string;
            break;
        case json_array:
            m_value.mp_array = j.m_value.mp_array;
            break;
        case json_object:
            m_value.mp_object = j.m_value.mp_object;
            break;
        default:
            break;
    }
}

/// 拷贝函数，使用深拷贝
/// \param j
void Json::deep_copy(const Json &j) {
    m_type = j.m_type;
    switch (m_type) {
        case json_null:
            break;
        case json_bool:
            m_value.m_bool = j.m_value.m_bool;
            break;
        case json_int:
            m_value.m_int = j.m_value.m_int;
            break;
        case json_double:
            m_value.m_double = j.m_value.m_double;
            break;
        case json_string:
            m_value.mp_string = new std::string(*(j.m_value.mp_string));
            break;
        case json_array:
            m_value.mp_array = new std::vector<Json>(*(j.m_value.mp_array));
            break;
        case json_object:
            m_value.mp_object = new std::map<std::string, Json>(*(j.m_value.mp_object));
            for(auto it1 = m_value.mp_object->begin(), it2 = j.m_value.mp_object->begin();
            it1 != m_value.mp_object->end() && it2 != j.m_value.mp_object->end(); ++it1, ++it2){
                it1->second.deep_copy(it2->second);
            }
            break;
        default:
            break;
    }
}

/// 清除内部指针指向的内存
void Json::clear(){
    switch (m_type) {
        case json_bool:
            m_value.m_bool = false;
            break;
        case json_int:
            m_value.m_int = 0;
            break;
        case json_double:
            m_value.m_double = 0.0;
            break;
        case json_null:
            break;
        case json_string:
            delete m_value.mp_string;
            m_value.mp_string = nullptr;
            break;
        case json_array:
            for(auto & it : (*m_value.mp_array)){
                it.clear();
            }
            delete m_value.mp_array;
            m_value.mp_array = nullptr;
            break;
        case json_object:
            for(auto & it : (*m_value.mp_object)){
                it.second.clear();
            }
            delete m_value.mp_object;
            m_value.mp_object = nullptr;
            break;
        default:
            break;
    }
    m_type = json_null;
    m_value.m_int = 0;
}

/// 赋值运算符
/// \param j Json object
void Json::operator = (const Json & j){
    shallow_copy(j);
}

//返回Json的大小（主要为数组和对象服务）
int Json::size() const{
    if(m_type == json_string){
        return m_value.mp_string->size();
    }else if(m_type == json_array){
        return m_value.mp_array->size();
    }else if(m_type == json_object){
        return m_value.mp_object->size();
    }else if(m_type == json_null){
        return 0;
    }else{
        return 1;
    }
}

/// 扩展Json的 == 功能
/// \param j
/// \return
bool Json::operator == (const Json & j){
    if(m_type != j.m_type) return false;
    switch (m_type) {
        case json_null:
            return true;
        case json_int:
            return m_value.m_int == j.m_value.m_int;
        case json_double:
            return m_value.m_double == j.m_value.m_double;
        case json_string:
            if(this->size() != j.size()) return false;
            return *(m_value.mp_string) == *(j.m_value.mp_string);
        case json_bool:
            return m_value.m_bool == j.m_value.m_bool;
        case json_array:
            if(this->size() != j.size()) return false;
            for(int i = 0; i < this->size(); ++i){
                if(m_value.mp_array->at(i) != j.m_value.mp_array->at(i))
                    return false;
            }
            return true;
        case json_object:
            if(this->size() != j.size()) return false;
            for(auto it1 = m_value.mp_object->begin(), it2 = j.m_value.mp_object->begin();
            it1 != m_value.mp_object->end() && it2 != j.m_value.mp_object->end(); ++it1, ++it2){
                if(it1->first != it2->first && it2->second != it1->second) return false;
            }
            return true;
        default:
            break;
    }
    return false;
}

/// 扩展Json的 != 功能
bool Json::operator != (const Json & j){
    return !(this->operator==(j));
}

/// Json数据中参数是否合理
/// \param index 输入数组下标以及对象的key值
/// \return 返回下标是否越界，key值是否存在
bool Json::has(int index) const{
    if(m_type != json_array){
        return false;
    }
    return index >= 0 && index < size();
}

bool Json::has(const char * key) const{
    return has(std::string(key));
}

bool Json::has(const std::string & key) const{
    if(m_type != json_object){
        return false;
    }
    return m_value.mp_object->count(key);
}

/// 删除Json中指定数据（会清除指向的空间，浅拷贝）
/// \param index 参数为数组下标或者对象key
/// \return 删除是否成功
bool Json::remove(int index) const {
    if(!has(index)) return false;
    (m_value.mp_array)->at(index).clear();//注意浅拷贝
    (m_value.mp_array)->erase(m_value.mp_array->begin() + index);
    return true;
}

bool Json::remove(const char *key) const {
    return remove(std::string(key));
}

bool Json::remove(const std::string &key) const {
    if(!has(key)) return false;
    (*(m_value.mp_object))[key].clear();
    (m_value.mp_object)->erase(key);
    return true;
}

void Json::parser(const std::string & str){
    Parser p;
    p.load(str);
    *this = p.parse();
}

void Json::parser(const char * str){
    parser(std::string(str));
}
//----解析部分

Parser::Parser() : m_str(""), m_index(0){

}

void Parser::load(const std::string & str){
    m_str = str;
    m_index = 0;
}

Json Parser::parse() {
    skip_space();
    char ch = m_str[m_index];

    if(ch == '-' || std::isdigit(ch)) {
        return parse_number();
    }else if(ch == 'n') return parse_null();
    else if(ch == 't' || ch == 'f') return parse_bool();
    else if(ch == '"') return parse_string();
    else if(ch == '[') return parse_array();
    else if(ch == '{') return parse_object();

    throw std::logic_error("Parser::parse >> type non match");
}

char Parser::skip_space(){
    while(m_str[m_index] == ' ' || m_str[m_index] == '\n' || m_str[m_index] == '\r' || m_str[m_index] == '\t') ++m_index;
    return m_str[m_index];
}

char Parser::get_next_json(){
    skip_space();
    //return m_str[m_index++];
    return m_str[m_index];
}

Json Parser::parse_null() {
    if(m_str.compare(m_index, 4, "null") == 0) {
        m_index += 4;
        return Json();
    }
    throw std::logic_error("Parser::parse >> type non match");
}

Json Parser::parse_number() {
    int pos = m_index;
    int count = 0;
    if(m_str[m_index] == '-'){
        count++;
        m_index++;
    }

    bool is_double = false;
    while(m_str[m_index] == '.' || std::isalnum(m_str[m_index]) ){
        if(m_str[m_index++] == '.') is_double = true;
        ++count;
    }
    std::string number = m_str.substr(pos, count);
    if(is_double)
        return Json(std::stod(number));
    else
        return Json(std::stoi(number));
}

Json Parser::parse_bool() {
    if(m_str.compare(m_index, 4, "true") == 0){
        m_index += 4;
        return Json(true);
    }

    else if(m_str.compare(m_index, 5, "false") == 0){
        m_index += 5;
        return Json(false);
    }
    throw std::logic_error("Parser::parse >> type non match");
}

std::string Parser::parse_string() {
    std::string out;
    while(1){
        char ch = m_str[++m_index];
        if(ch == '"') break;
        out += ch;
    }
    ++m_index;//脱离字符串，指向引号下一位
    return out;
}

Json Parser::parse_array() {
    Json arr(Json::json_array);
    ++m_index;
    char ch = skip_space();
    if(ch == ']') return arr;
    while(1){
        arr.append(parse());
        ch = skip_space();
        if(ch == ']') break;
        if(m_str[m_index++] != ',') throw std::logic_error("Parser :: parse_array >> ',' isn't exist");//++越过 ','
        ch = skip_space();
    }
    ++m_index;//越过数组范围，跳过']'
    return arr;
}

Json Parser::parse_object() {
    Json obj(Json::json_object);
    ++m_index;//越过'{'
    char ch = skip_space();
    if(ch == '}') return obj;
    while(1){
        if(ch != '"') throw std::logic_error("Parse::parse_object >> key type is error");
        std::string key = parse_string();
        ch = skip_space();
        if(ch != ':') throw std::logic_error("Parse::parse_object >>  ':' isn't exist");
        ++m_index;//越过 ':'
        obj[key] = parse();
        ch = skip_space();
        if(ch == '}') break;
        if(m_str[m_index++] != ',') throw std::logic_error("Parser :: parse_object >> ',' isn't exist");//++越过 ','
        ch = skip_space();
    }
    ++m_index;//越过'}'
    return obj;
}
//
// Created by Shota on 2022/12/27.
//
#include "json.h"

using namespace shotacon::json;

//构造函数
Json::Json() : m_type(json_null) {}

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
Json::Json(const Json &v) {
    m_type = v.m_type;
    switch (m_type) {
        case json_null:
            break;
        case json_bool:
            m_value.m_bool = v.m_value.m_bool;
            break;
        case json_int:
            m_value.m_int = v.m_value.m_int;
            break;
        case json_double:
            m_value.m_double = v.m_value.m_double;
            break;
        case json_string:
            m_value.mp_string = v.m_value.mp_string;
            break;
        case json_array:
            m_value.mp_array = v.m_value.mp_array;
            break;
        case json_object:
            m_value.mp_object = v.m_value.mp_object;
            break;
        default:
            break;
    }
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
        m_type = json_array;
        m_value.mp_array = new std::vector<Json>;
    }
    (m_value.mp_array)->push_back(other);
}

/// 以string返回Json内容
/// \return
std::string Json::get() const {
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
            ss << "[";
            for(auto it = m_value.mp_array->begin(); it != m_value.mp_array->end(); ++it){
                if(it != (m_value.mp_array)->begin()){
                    ss << ", ";
                }
                ss << it->get();
            }
            ss << "]";
            break;
        case json_object:
            ss << "{";
            for(auto it = m_value.mp_object->begin(); it != m_value.mp_object->end(); ++it){
                if(it != m_value.mp_object->begin()){
                    ss << ", ";
                }
                ss << "\"" << it->first << "\"" << ":" << it->second.get();
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
    std::string name(key);
    return ((*(this))[name]);
}

/// 重载Json对象的[]访问符
/// \param key json_object数据的key值，C++风格字符串
/// \return
Json & Json::operator [] (const std::string & key){
    return ((*m_value.mp_object)[key]);//无key插入，有key修改
}
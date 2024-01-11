#pragma once

#include "BG_Fields.h"

#include <iostream>
#include <sstream>
#include <type_traits>
#include <vector>

// Importing custom functionalities to header
float get_random(float min, float max);
int get_random_int(int min, int max);
void GetAnglesForDirection(const vec3_t p1, const vec3_t p2, vec3_t out);
bool FileExists(const char* filePath);
std::string get_random_message(std::initializer_list<const char*> messages);
void listRemaps(gentity_t* ent, int offset);
int AddRemap(const char* oldShader, const char* newShader, float timeOffset);
bool removeRemap(int index);
const char* BuildShaderStateConfig(void);



// ### TEMPLATES
// # Data conversion templates

// Check if the type is a string (const char*, char*, std::string)
template <typename T>
struct is_string : std::integral_constant<bool, std::is_same<T, std::string>::value ||
    std::is_same<T, const char*>::value ||
    std::is_same<T, char*>::value> {};

// Check if the type is numeric
template <typename T, typename = void>
struct is_numeric : std::false_type {};

template <typename T>
struct is_numeric<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> : std::true_type {};

// Convert numeric to string (generic)
template <typename TargetType, typename SourceType>
typename std::enable_if<is_numeric<TargetType>::value&& is_string<SourceType>::value, TargetType>::type
convert(const SourceType& input) {
    return convert<TargetType>(input);
}

// Convert string to numeric (generic)
template <typename T>
T convert(const std::string& input) {
    std::istringstream iss(input);
    T result;
    iss >> result;
    return result;
}

// # convertToString
// Convert every std type to string
template <typename T>
std::string convertToString(const T& input) {
    return std::to_string(input);
}

// Convert Vec3 to String 
template <>
std::string convertToString(const vec3_t& input) {
    return va("%f %f %f", input[0], input[1], input[2]);
}

// Convert std::vector<float> to String
template <>
std::string convertToString(const std::vector<float>& input) {
    return va("%f %f %f", input[0], input[1], input[2]);
}

// Convert string to std::vector<float>
template <>
std::vector<float> convert(const std::string& input) {
    std::vector<float> result(3, 0.0f);
    sscanf(input.c_str(), "%f %f %f", &result[0], &result[1], &result[2]);
    return result;
}

// Convert gitem_t to string (to be implemented)
template <>
std::string convertToString(const gitem_t& input) {
    // Implement the conversion for gitem_t
    // Example: return input.some_member;
}

// Convert gclient_t to string (to be implemented)
template <>
std::string convertToString(const gclient_t& input) {
    // Implement the conversion for gclient_t
    // Example: return input.some_member;
}

// Convert gentity_t to string (to be implemented)
template <>
std::string convertToString(const gentity_t& input) {
    // Implement the conversion for gentity_t
    // Example: return input.some_member;
}

// Convert string to every std<type>
template <typename T>
T convertFromString(const std::string& input) {
    std::istringstream iss(input);
    T result;
    iss >> result;
    return result;
}

// Convert to int
template <>
int convert<int>(const std::string& input) {
    return std::stoi(input);
}

// Convert to float
template <>
float convert<float>(const std::string& input) {
    return std::stof(input);
}

// Convert to bool
template <>
bool convert<bool>(const std::string& input) {
    std::istringstream iss(input);
    bool result;
    iss >> std::boolalpha >> result;
    return result;
}

// Convert to char (gets first letter or empty)
template <>
char convert<char>(const std::string& input) {
    if (!input.empty()) {
        return input[0];
    }
    else {
        return '\0';
    }
}

// Convert to double
template <>
double convert<double>(const std::string& input) {
    std::istringstream iss(input);
    double result;
    iss >> result;
    return result;
}

// Convert numeric to string (generic)
template <typename TargetType, typename SourceType>
typename std::enable_if<std::is_same<TargetType, std::string>::value&& is_numeric<SourceType>::value, TargetType>::type
convert(const SourceType& input) {
    std::ostringstream oss;
    oss << input;
    return oss.str();
}


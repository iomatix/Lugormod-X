#pragma once
// iomatix
#include <ostream>

// Enum for field types (assuming it's already defined in your code)
enum fieldtype_t {
    // ... (your field types)
};

// Struct for field information
struct BG_field_t {
    const char* name;
    int ofs;
    fieldtype_t type;
    // ... (other members if any)
};

// Function to parse a field type based on key and value
bool BG_ParseType(fieldtype_t type, const char* value, void* target);

// Function to parse a field based on key and value
bool BG_ParseField(BG_field_t* l_fields, const char* key, const char* value, void* target);

// Function to get a field value as a string
bool BG_GetField(BG_field_t* l_field, std::ostringstream& value, byte* ent);







// # TEMPALTES
//
 
//std::ostringstream stringValue;
//BG_GetField<std::string>(field, stringValue, entityBytes);
//
//std::ostringstream intValue;
//BG_GetField<int>(field, intValue, entityBytes);
// Getting fields
template <typename T>
bool BG_GetField(BG_field_t* l_field, std::ostringstream& value, byte* ent) {
    BG_field_t* f = l_field;
    byte* b = ent;
    bool success = false;

    // Using std::ostringstream for string concatenation
    std::ostringstream oss;

    switch (l_field->type) {
    case F_QSTRING:
    case F_LSTRING:
    case F_GSTRING:
        oss << convertToString(*(T*)(b + f->ofs));
        success = true;
        break;
    case F_VECTOR:
        oss << convertToString(*((T*)(b + f->ofs)));
        success = true;
        break;
    case F_INT:
        oss << convertToString(*(T*)(b + f->ofs));
        success = true;
        break;
    case F_FLOAT:
        oss << convertToString(*(T*)(b + f->ofs));
        success = true;
        break;
    case F_ANGLEHACK:
        oss << va("0 %f 0", convertToString(((T*)(b + f->ofs))[1]));
        success = true;
        break;
    case F_ITEM:
        // needs overloading
        // oss << convertToString(*(gitem_t*)(b + f->ofs));
        // success = true;
        break;
    case F_CLIENT:
        // needs overloading
        // oss << convertToString(*(gclient_t*)(b + f->ofs));
        // success = true;
        break;
    case F_ENTITY:
        // needs overloading
        // oss << convertToString(*(gentity_t*)(b + f->ofs));
        // success = true;
        break;
    default:
        break;
    }

    // Assign the concatenated string to value
    value.str("");
    value << oss.str();
    return success;
}


// Function to copy fields
template <typename T>
void BG_CopyField(BG_field_t* f, byte* dst, byte* src) {
    assert(f->type == F_INT || f->type == F_FLOAT || f->type == F_GSTRING || f->type == F_LSTRING || f->type == F_QSTRING);

    if constexpr (std::is_same<T, int>::value && (f->type == F_INT)) {
        *(int*)(dst + f->ofs) = *(int*)(src + f->ofs);
    }
    else if constexpr (std::is_same<T, float>::value && (f->type == F_FLOAT)) {
        *(float*)(dst + f->ofs) = *(float*)(src + f->ofs);
    }
    else if constexpr (std::is_same<T, const char*>::value && (f->type == F_GSTRING || f->type == F_LSTRING || f->type == F_QSTRING)) {
#ifdef QAGAME
        if (*(char**)(dst + f->ofs))
            G_Free(*(char**)(dst + f->ofs));
        *(char**)(dst + f->ofs) = G_NewString(*(char**)(src + f->ofs));
#else
        * (char**)(dst + f->ofs) = CG_NewString(*(char**)(src + f->ofs));
#endif
    }
    else {
        static_assert(always_false<T>::value, "Unhandled type in BG_CopyField");
    }
}


//int intValue;
//BG_ParseType(F_INT, "42", &intValue);
//
//float floatValue;
//BG_ParseType(F_FLOAT, "3.14", &floatValue);
//Parsing type
template <typename T>
bool BG_ParseType(fieldtype_t type, const char* value, T* target) {
    float v;
    vec3_t vec;
    const char* p;
    byte* b = reinterpret_cast<byte*>(target);

    switch (type) {
    case F_QSTRING:
    case F_LSTRING:
    case F_GSTRING:
        *(const char**)(b) = convert<std::string>(value).c_str();
        return true;
        break;
    case F_VECTOR:
        memset(vec, 0, sizeof(vec));
        sscanf(value, "%f %f %f", &vec[0], &vec[1], &vec[2]);
        ((float*)(b))[0] = vec[0];
        ((float*)(b))[1] = vec[1];
        ((float*)(b))[2] = vec[2];
        return true;
        break;
    case F_INT:
        *(int*)(b) = convert<int>(value);
        return true;
        break;
    case F_FLOAT:
        *(float*)(b) = convert<float>(value);
        return true;
        break;
    case F_ANGLEHACK:
        v = convert<float>(value);
        ((float*)(b))[0] = 0;
        ((float*)(b))[1] = v;
        ((float*)(b))[2] = 0;
        return true;
        break;

#ifdef QAGAME
    case F_PARM1:
    case F_PARM2:
    case F_PARM3:
    case F_PARM4:
    case F_PARM5:
    case F_PARM6:
    case F_PARM7:
    case F_PARM8:
    case F_PARM9:
    case F_PARM10:
    case F_PARM11:
    case F_PARM12:
    case F_PARM13:
    case F_PARM14:
    case F_PARM15:
    case F_PARM16:
        Q3_SetParm(reinterpret_cast<gentity_t*>(target)->s.number, (type - F_PARM1), const_cast<char*>(value));
        return true;
        break;
#endif
    default:
    case F_IGNORE:
        break;
    }

    return false;
}



//gentity_t entity;
//BG_ParseType(F_INT, "42", &entity);
//Parsing types
template <typename T>
bool BG_ParseType(fieldtype_t type, const char* value, T* target) {
    float v;
    vec3_t vec;
    const char* p;
    byte* b = (byte*)target;

    switch (type) {
    case F_QSTRING:
    case F_LSTRING:
    case F_GSTRING:
        *(const char**)(b) = convert<std::string>(value).c_str();
        return true;
        break;
    case F_VECTOR:
        memset(vec, 0, sizeof(vec));
        sscanf(value, "%f %f %f", &vec[0], &vec[1], &vec[2]);
        ((float*)(b))[0] = vec[0];
        ((float*)(b))[1] = vec[1];
        ((float*)(b))[2] = vec[2];
        return true;
        break;
    case F_INT:
        *(int*)(b) = convert<int>(value);
        return true;
        break;
    case F_FLOAT:
        *(float*)(b) = convert<float>(value);
        return true;
        break;
    case F_ANGLEHACK:
        v = convert<float>(value);
        ((float*)(b))[0] = 0;
        ((float*)(b))[1] = v;
        ((float*)(b))[2] = 0;
        return true;
        break;

#ifdef QAGAME
    case F_PARM1:
    case F_PARM2:
    case F_PARM3:
    case F_PARM4:
    case F_PARM5:
    case F_PARM6:
    case F_PARM7:
    case F_PARM8:
    case F_PARM9:
    case F_PARM10:
    case F_PARM11:
    case F_PARM12:
    case F_PARM13:
    case F_PARM14:
    case F_PARM15:
    case F_PARM16:
        Q3_SetParm(((gentity_t*)(target))->s.number, (type - F_PARM1), (char*)value);
        return true;
        break;
#endif
    default:
    case F_IGNORE:
        break;
    }

    return false;
}

// Function to parse a field based on key and value
//gentity_t entity;
//BG_ParseField(l_fields, "some_key", "some_value", &entity);
template <typename T>
bool BG_ParseField(BG_field_t* l_fields, const char* key, const char* value, T* target) {
    BG_field_t* f;

    for (f = l_fields; f->name; f++) {
        if (!Q_stricmp(f->name, key)) {
            BG_ParseType(f->type, value, (static_cast<byte*>(target) + f->ofs));
            return true;
        }
    }
    return false;
}


// Function to compare fields
template <typename T>
bool BG_CompareFields(BG_field_t* f, byte* v1, byte* v2) {
    
    static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, const char*>, "Unsupported type in BG_CompareFields");

    if constexpr (std::is_same_v<T, int> && f->type == F_INT) {
        return *(int*)(v1 + f->ofs) == *(int*)(v2 + f->ofs);
    }
    else if constexpr (std::is_same_v<T, float> && f->type == F_FLOAT) {
        return *(float*)(v1 + f->ofs) == *(float*)(v2 + f->ofs);
    }
    else if constexpr (std::is_same_v<T, const char*> && (f->type == F_GSTRING || f->type == F_LSTRING || f->type == F_QSTRING)) {
        return (Q_stricmp(*(char**)(v1 + f->ofs), *(char**)(v2 + f->ofs)) == 0);
    }
    else {
        static_assert(always_false<T>::value, "Unsupported type in BG_CompareFields");
        return false;
    }
}


#ifdef QAGAME


// Function to free a field
template <typename T>
void BG_FreeField(T* target) {
    static_assert(std::is_same_v<T, const char*>, "Unsupported type in BG_FreeField");

    if constexpr (std::is_same_v<T, const char*>) {
        G_Free(*target);
    }
}

// Function to free fields
template <typename T>
void BG_FreeFields(BG_field_t* fields, byte* structure) {
    BG_field_t* f = fields;
    while (f->name) {
        BG_FreeField(static_cast<T*>(structure + f->ofs));
        f++;
    }
}

#endif


// Template Copy Fields
template <typename T>
void BG_CopyField(BG_field_t* f, byte* dst, byte* src) {
    assert(f->type == F_INT || f->type == F_FLOAT || f->type == F_GSTRING || f->type == F_LSTRING || f->type == F_QSTRING);

    if constexpr (std::is_same<T, int>::value && (f->type == F_INT)) {
        *(int*)(dst + f->ofs) = *(int*)(src + f->ofs);
    }
    else if constexpr (std::is_same<T, float>::value && (f->type == F_FLOAT)) {
        *(float*)(dst + f->ofs) = *(float*)(src + f->ofs);
    }
    else if constexpr (std::is_same<T, const char*>::value && (f->type == F_GSTRING || f->type == F_LSTRING || f->type == F_QSTRING)) {
#ifdef QAGAME
        if (*(char**)(dst + f->ofs))
            G_Free(*(char**)(dst + f->ofs));
        *(char**)(dst + f->ofs) = G_NewString(*(char**)(src + f->ofs));
#else
        * (char**)(dst + f->ofs) = CG_NewString(*(char**)(src + f->ofs));
#endif
    }
    else {
        static_assert(always_false<T>::value, "Unhandled type in BG_CopyField");
    }
}


//bool fileExists = BG_GenericFileExists<std::string>("example.txt");
//Checks if file exists 
template <typename T>
bool BG_GenericFileExists(const T& fileName) {
    if (!fileName.empty()) {
        int fh = 0;
        trap_FS_FOpenFile(fileName.c_str(), &fh, FS_READ);
        if (fh > 0) {
            trap_FS_FCloseFile(fh);
            return true;
        }
    }

    return false;
}

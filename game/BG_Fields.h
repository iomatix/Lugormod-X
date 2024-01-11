// iomatix
#pragma once

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

// Function to compare fields
bool BG_CompareFields(BG_field_t* f, byte* v1, byte* v2);

// Function to copy fields
void BG_CopyField(BG_field_t* f, byte* dst, byte* src);

#ifdef QAGAME
// Function to free a field based on type
void BG_FreeField(fieldtype_t type, void* target);

// Function to free fields in a structure
void BG_FreeFields(BG_field_t* fields, byte* structure);
#endif
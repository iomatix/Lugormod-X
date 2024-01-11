#pragma once

#include "Lmd_Data_Public.h"
#include <fstream>
#include <sstream>

enum class FieldType {
	Auto,
	Func,
	Prefix,
	Default,
};

typedef struct {
	unsigned int func;
	int offset;
}DBSaveFileCallbackReturn_t;

bool Lmd_Data_IsCleanPath(const std::string& path);

// Opens a data file from the current data directory.
fileHandle_t Lmd_Data_OpenDataFile(const std::string& directory, const std::string& name, fsMode_t mode);

// Check if file exist
bool Lmd_Data_isFileValid(const std::string& directory, const std::string& name);

// Copies the file content of the non-data path to a G_Alloc string.
std::vector<char> Lmd_Data_AllocFileContents(const std::string& filename);

unsigned int Lmd_Data_ProcessFiles(const std::string& directory, const std::string& ext, qboolean(*Callback)(const std::string& fileName, const char* fileBuf), int maxFiles, const std::string& specificFile = NULL);
unsigned int Lmd_Data_ProcessFile(const std::string& directory, const std::string& fileName,
	std::function<bool(const std::string&, const std::vector<char>&)> Callback);


// Parses a set of fields as
//key,value,key,value
unsigned int Lmd_Data_ParseDatastring(char** str, std::function<bool(byte*, bool, const std::string&, const std::string&)> Callback, BG_field_t* fields, byte* structure);

//Match Fields
int Lmd_Data_MatchField(char* key, const DataField_t* fields, int fieldCount, int start);

bool Lmd_Data_DeleteFile(const std::string& directory, const std::string& name);


bool Lmd_Data_SaveDatafile(const std::string& directory, const std::string& name, BG_field_t* fields, byte* structure,
	bool (*Override)(byte* structure, const std::string& key, std::string& value),
	DBSaveFileCallbackReturn_t* (*MoreKeys)(byte* structure, DBSaveFileCallbackReturn_t* arg, std::string& key, std::string& value));
bool Lmd_Data_WriteDatastringField(BG_field_t* f, char* value, unsigned int sze, byte* b);
bool Lmd_Data_WriteDatafileField(BG_field_t* f, std::string& value, unsigned int sze, byte* b);
std::string Lmd_Data_GetDataPath(const std::string& directory);

typedef struct DataAutoFieldArgs_s
{
	int ofs;
	fieldtype_t	type;
} DataAutoFieldArgs_t;


int Lmd_Data_Parse_LineDelimited(
	char** str,
	void* target,
	const DataField_t fields[],
	int fieldCount,
	const char* keyValueSeparator,
	const char* fieldSeparator);


// TODO: Define macros for recursive key value pair delegation.
bool Lmd_Data_Parse_KeyValuePair(const std::string& key, const std::string& value, void* target, const std::vector<DataField_t>& fields);


int Lmd_Data_WriteToFile_LinesDelimited(fileHandle_t file, const DataField_t fields[], int fieldCount, void* target);



void Lmd_Data_FreeFields(void *target, const DataField_t fields [], int fieldCount);


// Predefined callbacks
bool Lmd_Data_AutoFieldCallback_Parse(const std::string& key, const std::string& value, void* target, void* args);
DataWriteResult_t Lmd_Data_AutoFieldCallback_Write(void* target, std::string& key, std::string& value, void** writeState, void* args);
void Lmd_Data_AutoFieldCallback_Free(void* state, void* args);





#define AUTOFIELD(name, ofs, type) {name, false, Lmd_Data_AutoFieldCallback_Parse, std::make_any<DataAutoFieldArgs_t>(ofs, type), Lmd_Data_AutoFieldCallback_Write, std::make_any<DataAutoFieldArgs_t>(ofs, type), Lmd_Data_AutoFieldCallback_Free, std::make_any<DataAutoFieldArgs_t>(ofs, type)}

#define DEFINE_FIELD_PRE_AUTO(name, ofs, type) const DataAutoFieldArgs_t DataFieldArgs_##name = { ofs, type };
#define DEFINE_FIELD_PRE_FUNC(name, parseFunc, writeFunc, args)
#define DEFINE_FIELD_PRE_PREF(prefix, parseFunc, writeFunc, args)
#define DEFINE_FIELD_PRE_DEFL(parseFunc, writeFunc, args)

#define DEFINE_FIELD_LIST_AUTO(name, ofs, type) {#name, false, Lmd_Data_AutoFieldCallback_Parse, std::make_any<DataAutoFieldArgs_t>(ofs, type), Lmd_Data_AutoFieldCallback_Write, std::make_any<DataAutoFieldArgs_t>(ofs, type), Lmd_Data_AutoFieldCallback_Free, std::make_any<DataAutoFieldArgs_t>(ofs, type)}
#define DEFINE_FIELD_LIST_FUNC(name, parseFunc, writeFunc, args) {#name, false, parseFunc, args, writeFunc, args}
#define DEFINE_FIELD_LIST_PREF(prefix, parseFunc, writeFunc, args) {#prefix, true, parseFunc, args, writeFunc, args}
#define DEFINE_FIELD_LIST_DEFL(parseFunc, writeFunc, args) {NULL, true, parseFunc, args, writeFunc, args}

#define DATAFIELDS_BEGIN(name) const DataField_t name[] = {
#define DATAFIELDS_END };

#define DATAFIELDS_COUNT(name) (sizeof(name) / sizeof(DataField_t))




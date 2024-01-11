#pragma once
#include "g_local.h"

#include "Lmd_Data.h"

#include "BG_Fields.h"
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <iostream>

constexpr int LMD_DATABASE_FILELIST_MAX = 2048;
constexpr int  LMD_DATABASE_FILENAME_SIZE = MAX_NETNAME;
constexpr const char* LMD_DATABASE_DATAPATH = "data";
extern vmCvar_t lmd_DataPath;

namespace fs = std::filesystem;

bool Lmd_Data_IsCleanPath(const std::string& path) {
	static const char invalidChars[] = { '?', '%', '*', ':', '|', '\"', '<', '>' };

	if (path.find("..") != std::string::npos) {
		return false;
	}

	std::ostringstream oss;
	for (char c : path) {
		if (std::find(std::begin(invalidChars), std::end(invalidChars), c) != std::end(invalidChars)) {
			return false;
		}
	}

	return true;
}

std::string Lmd_Data_GetDataPath(const std::string& directory) {
	std::string datapath = lmd_DataPath.string;
	if (datapath.empty()) {
		datapath = "default";
	}

	fs::path dataPath = LMD_DATABASE_DATAPATH;
	dataPath /= datapath;
	dataPath /= directory;

	return dataPath.string();
}


fileHandle_t Lmd_Data_OpenDataFile(const std::string& directory, const std::string& name, fsMode_t mode) {
	fs::path filePath = Lmd_Data_GetDataPath(directory);
	filePath.append(name);

	fileHandle_t f;
	trap_FS_FOpenFile(filePath.string().c_str(), &f, mode);
	return f;
}


bool Lmd_Data_isFileValid(const std::string& directory, const std::string& name) {
	fs::path filePath = Lmd_Data_GetDataPath(directory);
	filePath.append(name);

	fileHandle_t f;
	if (trap_FS_FOpenFile(filePath.string().c_str(), &f, FS_READ) && f >= 0) {
		trap_FS_FCloseFile(f);
		return true;
	}
	return false;
}

// Function to allocate memory and read the contents of a file
std::vector<char> Lmd_Data_AllocFileContents(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);

	// Check if the file is open
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return std::vector<char>();
	}

	// Determine the size of the file
	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// Create a vector and read the file contents
	std::vector<char> buf(fileSize);
	if (file.read(buf.data(), fileSize)) {
		// File read successfully
		file.close();
		return buf;
	}
	else {
		// Failed to read file
		std::cerr << "Failed to read file: " << filename << std::endl;
		file.close();
		return std::vector<char>();
	}
}




unsigned int Lmd_Data_ProcessFile(const std::string& directory, const std::string& fileName,
	std::function<bool(const std::string&, const std::vector<char>&)> Callback) {
	std::string path = Lmd_Data_GetDataPath(directory);
	std::string fullPath = path + "/" + fileName;

	// Allocate and read the file contents
	std::vector<char> fileBufVec = Lmd_Data_AllocFileContents(fullPath);
	if (fileBufVec.empty()) {
		return 0;
	}

	// Remove the extension from the file name
	std::string fileWithoutExtension = fileName.substr(0, fileName.find_last_of('.'));

	// Use the std::function callback
	if (Callback(fileWithoutExtension, fileBufVec)) {
		return 1;
	}

	// No need to free memory when using std::vector
	return 0;
}

std::string Lmd_Data_ProcessFile_GetFileNameWithoutExtension(const std::string& fileName) {
	size_t pos = fileName.find_last_of('.');
	return (pos != std::string::npos) ? fileName.substr(0, pos) : fileName;
}

unsigned int Lmd_Data_ProcessFiles(const std::string& directory, const std::string& ext,
	qboolean(*Callback)(const std::string& fileName, const char* fileBuf),
	int maxFiles, const std::string& specificFile) {

	std::string path = Lmd_Data_GetDataPath(directory);
	fs::path directoryPath = path;

	unsigned int totalFiles = 0;

	try {
		for (const auto& entry : fs::directory_iterator(directoryPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ext) {
				std::string fileName = entry.path().filename().string();

				if (!specificFile.empty() && fileName != specificFile) {
					continue;  // Skip this file if it's not the specific one
				}

				// Convert the function pointer to std::function using a lambda
				auto processFileCallback = [Callback](const std::string& fName, const std::vector<char>& fileContent) {
					return Callback(fName, fileContent.data());
					};

				// Call the updated function to process a single file
				if (Lmd_Data_ProcessFile(directory, fileName, processFileCallback)) {
					totalFiles++;
				}

				if (totalFiles >= maxFiles) {
					break;
				}
			}
		}
	}
	catch (const fs::filesystem_error& ex) {
		std::cerr << "Error processing files: " << ex.what() << std::endl;
	}

	return totalFiles;
}


bool Lmd_Data_DeleteFile(const std::string& directory, const std::string& name) {
	vmCvar_t fs_game;
	std::string filename = Lmd_Data_GetDataPath(directory) + "/" + name;

	trap_Cvar_Register(&fs_game, "fs_game", "", CVAR_SERVERINFO | CVAR_ROM);

	std::string gamePath = (fs_game.string[0] == 0) ? "base" : fs_game.string;
	fs::path fullPath = gamePath + "/" + filename;

	try {
		fs::remove(fullPath);
		return true;
	}
	catch (const fs::filesystem_error& ex) {
		std::cerr << "Error deleting file: " << ex.what() << std::endl;
		return false;
	}
}



// Define the ParseFieldsState_s structure
struct ParseFieldsState_s {
	BG_field_t* fields;
	byte* structure;
	bool (*callback)(byte* object, bool pre, char* key, char* value);
};

// Class definition for ParseFieldsState
class ParseFieldsState {
public:
	ParseFieldsState(BG_field_t* fields, byte* structure, bool (*callback)(byte* object, bool pre, char* key, char* value))
		: fields(fields), structure(structure), callback(callback) {}

	// Getters
	BG_field_t* getFields() const { return fields; }
	byte* getStructure() const { return structure; }
	bool (*getCallback())(byte*, bool, char*, char*) { return callback; }

	// Setters
	void setFields(BG_field_t* newFields) { fields = newFields; }
	void setStructure(byte* newStructure) { structure = newStructure; }
	void setCallback(bool (*newCallback)(byte*, bool, char*, char*)) { callback = newCallback; }

private:
	BG_field_t* fields;
	byte* structure;
	bool (*callback)(byte* object, bool pre, char* key, char* value);
};


bool Lmd_Data_ParseFields_Callback(char* key, char* value, void* pstate) {
	ParseFieldsState* state = static_cast<ParseFieldsState*>(pstate);

	if (state->getCallback() && state->getCallback()(state->getStructure(), true, key, value)) {
		return true;
	}

	if (state->getFields() && state->getStructure()) {
		if (BG_ParseField(state->getFields(), key, value, state->getStructure()) ||
			(state->getCallback() && state->getCallback()(state->getStructure(), false, key, value))) {
			return true;
		}
	}

	return false;
}

bool ParseField(char** sourcep, char* dest, char start, char stop);
unsigned int Lmd_Data_ParseDatastring(char** str, std::function<bool(byte*, bool, const std::string&, const std::string&)> Callback, BG_field_t* fields, byte* structure) {
	char key[MAX_STRING_CHARS], value[MAX_STRING_CHARS];
	char* token;
	unsigned int count = 0;

	while (str[0]) {
		token = COM_ParseDatastring(const_cast<const char**>(str));
		if (!token[0])
			break;
		strncpy_s(key, token, sizeof(key));

		token = COM_ParseDatastring(const_cast<const char**>(str));
		if (!token[0])
			break;
		strncpy_s(value, token, sizeof(value));

		std::string keyStr(key);
		std::string valueStr(value);

		if ((Callback && Callback(structure, true, keyStr, valueStr)) ||
			BG_ParseField(fields, keyStr.c_str(), valueStr.c_str(), structure) ||
			(Callback && Callback(structure, false, keyStr, valueStr))) {
			count++;
		}
	}

	return count;
}

// Function to write a datafile field to a std::string
bool Lmd_Data_WriteDatafileField(BG_field_t* f, std::string& value, unsigned int sze, byte* b) {
	// Don't attempt if null or 0
	if ((f->type == F_LSTRING || f->type == F_GSTRING) && (*(char**)(b + f->ofs) == nullptr || (*(char**)(b + f->ofs))[0] == 0) ||
		(f->type == F_INT || f->type == F_FLOAT) && *(int*)(b + f->ofs) == 0) {
		return false;
	}

	// Using std::ostringstream for string concatenation
	std::ostringstream oss;
	BG_GetField(f, oss, b);

	// Assign the concatenated string to value
	value = va("%s: %s", f->name, oss.str().c_str());

	return true;
}



// Function to write a datastring field to a char array
bool Lmd_Data_WriteDatastringField(BG_field_t* f, char* value, unsigned int sze, byte* b) {
	// Don't attempt if null or 0
	if ((f->type == F_LSTRING || f->type == F_GSTRING) && (*(char**)(b + f->ofs) == nullptr || (*(char**)(b + f->ofs))[0] == 0) ||
		(f->type == F_INT || f->type == F_FLOAT) && *(int*)(b + f->ofs) == 0) {
		return false;
	}

	// Using std::ostringstream for string concatenation
	std::ostringstream oss;
	BG_GetField(f, oss, b);

	// Copy the concatenated string to the provided char array
	Q_strncpyz(value, va("%s,%s,", f->name, oss.str().c_str()), sze);

	return true;
}

bool Lmd_Data_SaveDatafile(const std::string& directory, const std::string& name, BG_field_t* fields, byte* structure,
	bool (*Override)(byte* structure, const std::string& key, std::string& value),
	DBSaveFileCallbackReturn_t* (*MoreKeys)(byte* structure, DBSaveFileCallbackReturn_t* arg, std::string& key, std::string& value)) {

	fs::path filePath = Lmd_Data_GetDataPath(directory);
	filePath /= name;
	std::ofstream file(filePath.string(), std::ios::out | std::ios::trunc);


	if (!file.is_open()) {
		return false;
	}

	if (fields && structure) {
		for (BG_field_t* bgf = fields; bgf->name != nullptr; bgf++) {
			std::string key = bgf->name;
			std::string value;

			if (Override && Override(structure, key, value)) {
				file << key << ": " << value << '\n';
			}
			else {
				std::ostringstream oss;
				if (Lmd_Data_WriteDatafileField(bgf, value, value.size(), structure)) {
					file << key << ": " << value << '\n';
				}
			}
		}
	}

	if (MoreKeys) {
		DBSaveFileCallbackReturn_t arg, * argPtr = &arg;
		std::string key, value;

		while ((argPtr = MoreKeys(structure, argPtr, key, value))) {
			if (!key.empty() && !value.empty()) {
				file << key << ": " << value << '\n';
				key.clear();
				value.clear();
			}
		}
	}

	file.close();
	return true;
}


int Lmd_Data_MatchField(const char* key, const DataField_t* fields, int fieldCount, int start) {
	for (int i = start; i < fieldCount; i++) {
		if (fields[i].isPrefix) {
			if (fields[i].key.empty() || strncmp(key, fields[i].key.c_str(), fields[i].key.size()) == 0) {
				return i;
			}
		}
		else {
			if (strcmp(key, fields[i].key.c_str()) == 0) {
				return i;
			}
		}
	}

	return -1;
}
int Lmd_Data_Parse_LineDelimited(
	char** str,
	void* target,
	const DataField_t fields[],
	int fieldCount,
	const char* keyValueSeparator,
	const char* fieldSeparator)
{
	char* p;
	char key[MAX_STRING_CHARS], value[MAX_STRING_CHARS];
	unsigned int count = 0;
	int colPos;

	char* valuePtr;

	while (*str) {
		p = COM_ParseExt((const char**)str, qtrue);
		if (p[0] == 0) {
			break;
		}

		Q_strncpyz(key, p, sizeof(key));
		colPos = strlen(key) - 1;
		if (colPos <= 0) {
			// Skip this line. Untested.
			COM_ParseLine((const char**)str);
			break;
		}

		// Use the provided keyValueSeparator to separate key and value
		if (key[colPos] == keyValueSeparator[0]) {
			key[colPos] = 0;
			Q_strncpyz(value, COM_ParseLine((const char**)str), sizeof(value));
			valuePtr = value; // We got a real value (may be empty).
		}
		else {
			valuePtr = nullptr; // No value.
		}

		if (Lmd_Data_Parse_KeyValuePair(key, valuePtr, target, std::vector<DataField_t>(fields, fields + fieldCount))) {
			count++;
		}

		// Use the provided fieldSeparator to skip fields
		if (fieldSeparator[0] != 0) {
			while (*str && Q_stricmp(COM_ParseExt((const char**)str, qtrue), fieldSeparator) != 0) {
				// Skip fields until the next fieldSeparator or end of string
			}
		}
	}

	return count;
}


bool Lmd_Data_Parse_KeyValuePair(const std::string& key, const std::string& value, void* target, const std::vector<DataField_t>& fields) {
	size_t start = 0;

	while (start < fields.size()) {
		size_t index = Lmd_Data_MatchField(key.c_str(), fields.data(), static_cast<int>(fields.size()), static_cast<int>(start));
		const DataField_t& field = fields[index];

		try {
			auto parseFunction = std::any_cast<ParseFunction>(field.parse);
			// Sprawdü, czy sygnatura funkcji jest zgodna
			if constexpr (std::is_invocable_r_v<DataParseResult_t, ParseFunction, const std::string&, const std::string&, void*, void*>) {
				
				DataParseResult_t dpar = field.parse(key, value, &field.parseArgs, target, nullptr);


				switch (dpar) {
				case DataParseResult_t::DPAR_OK:
					return true;
				case DataParseResult_t::DPAR_FAIL:
					return false;
				case DataParseResult_t::DPAR_IGNORE:
					// Continue to the next field
					break;
				}
			}
			else {
				// Error Handling
				std::cerr << "Error: Invalid function signature" << std::endl;
			}
		}
		catch (const std::bad_any_cast& e) {
			// Handle the exception (e.g., log an error)
			// This indicates that the stored type in field.parseArgs does not match ParseFunction
			// You might want to add proper error handling based on your requirements
			// For example, you could log the error and continue to the next field
			std::cerr << "Error in Lmd_Data_Parse_KeyValuePair: " << e.what() << std::endl;
		}

		start = index + 1;
	}

	return false;
}






// Assuming DataField_t and its types are already defined
int Lmd_Data_WriteToFile_LinesDelimited(fileHandle_t file, const DataField_t fields[], int fieldCount, void* target) {
	int writes = 0;

	for (int i = 0; i < fieldCount; i++) {
		DataWriteResult_t dwr;

		// If there is no write function for this field, skip to the next one
		if (!fields[i].write) continue;

		void* writeState = nullptr;
		do {
			// If the field has a key, copy it to the key variable
			// Otherwise, set the key to an empty string
			char key[MAX_STRING_CHARS];
			char value[MAX_STRING_CHARS];

			if (!fields[i].key.empty()) {
				Q_strncpyz(key, fields[i].key.c_str(), sizeof(key));
			}
			else {
				key[0] = 0;
			}

			// Call the write function for this field
			dwr = fields[i].write(target, key, value, &fields[i].writeArgs, nullptr);
			




			// If the write function returned a result other than NODATA and the key is not empty,
			// write the key-value pair to the file
			if (dwr != DataWriteResult_t::DWR_NODATA && key[0]) {
				char* line;
				// If the value is not empty, write both the key and the value
				// Otherwise, write only the key
				if (value[0]) {
					line = va("%s: %s\n", key, value);
				}
				else {
					line = key;
				}
				trap_FS_Write(line, strlen(line), file);
				writes++;
			}
			// Repeat the process if the write function returned CONTINUE or SKIP
		} while (dwr == DataWriteResult_t::DWR_CONTINUE || dwr == DataWriteResult_t::DWR_SKIP);
	}

	// Return the number of writes
	return writes;
}




void Lmd_Data_FreeFields(void* target, const DataField_t fields[], int fieldCount) {
	for (int i = 0; i < fieldCount; i++) {
		if (fields[i].freeData) {
			fields[i].freeData(target, fields[i].freeDataArgs);
		}
	}
}


bool Lmd_Data_AutoFieldCallback_Parse(const std::string& key, const std::string& value, void* target, void* args) {
	if (!value.empty()) {
		G_LogPrintf("Debug: %s<->%s %s !!\n", key.c_str(), value.c_str(), value[0]);
		G_LogPrintf("Debug: ARGS: %s !!\n", args);

		DataAutoFieldArgs_t* fieldArgs = static_cast<DataAutoFieldArgs_t*>(args);
		BG_ParseType(fieldArgs->type, value.c_str(), reinterpret_cast<byte*>(target) + fieldArgs->ofs);
		return true;
	}

	return false;
}

DataWriteResult_t Lmd_Data_AutoFieldCallback_Write(void* target, std::string& key, std::string& value, void** writeState, void* args) {
	if (args) {
		DataAutoFieldArgs_t* fieldArgs = static_cast<DataAutoFieldArgs_t*>(args);
		BG_field_t f = { key.c_str(), fieldArgs->ofs, fieldArgs->type };
		BG_GetField(&f, value.c_str(), value.size(), reinterpret_cast<byte*>(target));

		assert(!key.empty());

		return DataWriteResult_t::DWR_OK;
	}

	return DataWriteResult_t::DWR_SKIP;
}

void Lmd_Data_AutoFieldCallback_Free(void* state, void* args) {
	if (state && args) {
		DataAutoFieldArgs_t* fieldArgs = static_cast<DataAutoFieldArgs_t*>(args);
		BG_FreeField(fieldArgs->type, reinterpret_cast<byte*>(state) + fieldArgs->ofs);
	}
}

#ifndef LMD_PROF_COMMON_H
#define LMD_PROF_COMMON_H

#include "q_shared.h"
#include "gentity_t.h"

#include "Lmd_Accounts_Public.h"
#include "Lmd_Data_Public.h"

typedef struct profSkill_s profSkill_t;

typedef struct skillLevelDef_s {
	int min;
	int max;
	qboolean canRemove;
}skillLevelDef_t;

static skillLevelDef_t SkillLevels_Default = {0, 5, qfalse}; //Levels_5
static skillLevelDef_t SkillLevels_3 = { 0, 3, qfalse };
static skillLevelDef_t SkillLevels_4 = { 0, 4, qfalse };
static skillLevelDef_t SkillLevels_6 = { 0, 6, qfalse };
static skillLevelDef_t SkillLevels_7 = { 0, 7, qfalse }; 
static skillLevelDef_t SkillLevels_8 = { 0, 8, qfalse };
static skillLevelDef_t SkillLevels_20 = { 0, 20, qfalse };

typedef enum {
	SPT_NOCOST,
	SPT_TRIANGULAR, //The usual ((level * (level + 1) / 2) point values.
    SPT_LINEAR,
	SPT_LINEAR_2,
	SPT_LINEAR_5,
	SPT_LINEAR_10,
	SPT_LINEAR_12,
} skillPointType_t;

typedef struct skillPointData_s {
	skillPointType_t type;
}skillPointData_t;

static skillPointData_t SkillPoints_Default = {SPT_TRIANGULAR};
static skillPointData_t SkillPoints_Linear = { SPT_LINEAR };
static skillPointData_t SkillPoints_Linear_2 = { SPT_LINEAR_2 };
static skillPointData_t SkillPoints_Linear_5 = { SPT_LINEAR_5 };
static skillPointData_t SkillPoints_Linear_10 = { SPT_LINEAR_10 };
static skillPointData_t SkillPoints_Linear_12 = { SPT_LINEAR_12 };

struct profSkill_s{
	char *name;

	// Help text for the /skills command.
	const char *baseDescription;

	//per-level effects.  "/skills" will show the next level's help text.
	// terminate the array with a null entry.
	const char **levelDescriptions;

	int unique;
	skillLevelDef_t levels;
	skillPointData_t points;

	int (*getValue)(AccountPtr_t acc, profSkill_t *skill);
	qboolean (*canSetValue)(AccountPtr_t acc, profSkill_t *skill, int value);
	qboolean (*setValue)(AccountPtr_t acc, profSkill_t *skill, int value);

	struct{
		unsigned int count;
		profSkill_t *skill;
		qboolean pointGroup; //The subskills use a seperate point group.
		skillPointData_t points;
	}subSkills;

	//These are populated automatically on mod load.
	profSkill_t *parent;
};

typedef struct profession_s {
	char *name;
	char *key;

	struct{
		const DataField_t *fields;
		unsigned int count;
		unsigned int size;
	}data;

	profSkill_t primarySkill;

	void (*spawn)(gentity_t *ent);
}profession_t;

#endif
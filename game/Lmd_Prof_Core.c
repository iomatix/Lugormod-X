
#include "g_local.h"

#include "Lmd_Console.h"

#include "Lmd_Data.h"
#include "Lmd_Accounts_Data.h"
#include "Lmd_Accounts_Core.h"
#include "Lmd_Commands_Auths.h"
#include "Lmd_Commands_Core.h"
#include "Lmd_Prof_Core.h"
#include "Lmd_Confirm.h"
#include "Lmd_Time.h"
#include "BG_Fields.h"

int AccProfessionDataDataIndex = -1;
#define PROFDATA(acc) (profData_t *)Lmd_Accounts_GetAccountCategoryData(acc, AccProfessionDataDataIndex)






//This is multiplied by (level) X (the next level)
#define LEVEL_COST 25 // 

#define MAX_LEVEL  120 // 40 ->  120
#define MASTER_LEVEL 40 //Level after unlocks Jedi skills for Sith and Sith skills for Jedi

extern vmCvar_t lmd_accLevelDiscount_time;
extern vmCvar_t lmd_accLevelDiscount_maxTime;

//Max reduction time for level cost calculations.
// 60*60*24*5
#define LEVEL_TIME_THRESH (lmd_accLevelDiscount_maxTime.integer)
//Reduce cost at 1 credit every 5 minutes (300 seconds).
// 60*5
#define LEVEL_TIME_REDUCE (lmd_accLevelDiscount_time.integer)

#define MAX_SKILL_LVL 5


extern profession_t noProf;
extern profession_t godProf;
extern profession_t botProf;
extern profession_t mercProf;
extern profession_t jediProf;

profession_t *Professions[] = {
	&noProf, //none
	&godProf, //admin
	&botProf, //bot
	&jediProf, //jedi
	&mercProf, //merc
};

typedef struct profData_s {
	int profession;
	int level;
	int lastLevelUp;
	void *data;
}profData_t;
#define	PROFDATAOFS(x) ((int)&(((profData_t *)0)->x))


void SetSkillParents(profSkill_t *skill) {
	int i;
	for (i = 0; i < skill->subSkills.count; i++) {
		skill->subSkills.skill[i].parent = skill;
		SetSkillParents(&skill->subSkills.skill[i]);
	}
}

void Prof_Init() {
	//Set up parent relationships in the skills
	int i;
	for (i = 0; i < NUM_PROFESSIONS; i++) {
		if (Professions[i]) {
			SetSkillParents(&Professions[i]->primarySkill);
			Professions[i]->primarySkill.parent = NULL;
		}
	}
}

qboolean Accounts_Profs_Parse(char *key, char *value, void *target, void *args) {
	profData_t *profData = (profData_t *)target;
	if (Q_stricmpn(key, "prof_", 5) != 0)
		return qfalse;

	void **alloc = (void **)profData->data;
	int i;
	char *profName = key + 5;
	char *skillName;
	int len;
	for (i = 0; i < NUM_PROFESSIONS; i++)
	{
		if (!Professions[i] || !Professions[i]->key)
			continue;

		len = strlen(Professions[i]->key);

		if (Q_stricmpn(profName, Professions[i]->key, len) != 0 || profName[len] != '_')
			continue;

		if (!Professions[i]->data.fields)
			break;

		skillName = profName + (len + 1);
		Lmd_Data_Parse_KeyValuePair(skillName, value, (void *)alloc[i], Professions[i]->data.fields, Professions[i]->data.count);
		break;
	}

	return qtrue;
}

typedef struct ProfessionWriteState_s {
	int index;
	int dataFieldIndex;
	void *dataFieldState;
} ProfessionWriteState_t;

DataWriteResult_t Accounts_Profs_Write(void *target, char key[], int keySize, char value[], int valueSize, void **writeState, void *args)
{
	profData_t *profData = (profData_t *)target;

	profession_t *prof = Professions[profData->profession];
	int dataFieldsCount = prof->data.count;

	if (dataFieldsCount == 0) {
		// This prof has no data, dont bother allocating anything
		return DWR_NODATA;
	}

	void* statePtr = *writeState;
	if (statePtr == NULL) {
		statePtr = *writeState = G_Alloc(sizeof(ProfessionWriteState_t));
	}

	ProfessionWriteState_t *state = (ProfessionWriteState_t *)statePtr;

	if (state->index >= dataFieldsCount) {
		G_Free(state);
		return DWR_NODATA;
	}

nextField:
	if (state->index < dataFieldsCount) {
		// Write the field.
		const DataField_t *field = &prof->data.fields[state->index];
		if (field->write) {
			void *dataPtr = profData->data;

			Q_strcat(key, keySize, va("%s_%s", prof->key, field->key));
			DataWriteResult_t result = field->write(dataPtr, key, keySize, value, valueSize, &state->dataFieldState, field->writeArgs);
			if (result == DWR_COMPLETE || result == DWR_NODATA) {
				// We are done with this field
				state->index++;
			}

			if (result == DWR_NODATA) {
				goto nextField;
			}

			return DWR_CONTINUE;
		}
		else {
			state->index++;
			goto nextField;
		}
	}
	else {
		G_Free(state);
		return DWR_NODATA;
	}
}

#define ProfsFields_Base(_m) \
	_m##_PREF(prof_, Accounts_Profs_Parse, Accounts_Profs_Write, NULL) \
	_m##_AUTO(profession, PROFDATAOFS(profession), F_INT) \
	_m##_AUTO(level, PROFDATAOFS(level), F_INT) \
	_m##_AUTO(lastLevelUp, PROFDATAOFS(lastLevelUp), F_INT)

ProfsFields_Base(DEFINE_FIELD_PRE)

DATAFIELDS_BEGIN(ProfsFields)
ProfsFields_Base(DEFINE_FIELD_LIST)
DATAFIELDS_END

const int ProfsFields_Count = DATAFIELDS_COUNT(ProfsFields);


void Lmd_Prof_Alloc(void *target) {
	profData_t *profData = (profData_t *)target;
	//Init the loader by giving us an array of all professions
	void **alloc = (void**)G_Alloc(sizeof(void *) * NUM_PROFESSIONS);
	int i;
	for (i = 0; i < NUM_PROFESSIONS; i++) {
		if (!Professions[i] || Professions[i]->data.size == 0) {
			alloc[i] = NULL;
			continue;
		}

		alloc[i] = (void *)G_Alloc(Professions[i]->data.size);
		memset(alloc[i], 0, Professions[i]->data.size);
	}

	profData->data = alloc;
}

void Lmd_Prof_AccountLoaded(AccountPtr_t accPtr, void *target) {
	Account_t *acc = (Account_t*)accPtr;
	//Remove the unused data.
	profData_t *profData = (profData_t *)target;
	void **alloc = (void **)profData->data;
	void *keep = alloc[profData->profession];
	int i;
	for (i = 0; i < NUM_PROFESSIONS; i++) {
		if (i == profData->profession)
			continue;
		if (alloc[i]) {
			Lmd_Data_FreeFields(alloc[i], Professions[i]->data.fields, Professions[i]->data.count);
			G_Free(alloc[i]);
		}
	}

	G_Free(profData->data);
	profData->data = keep;

	//Make sure they have a valid level up time
	int now = Time_Now();
	if (profData->lastLevelUp <= 0 || profData->lastLevelUp > now)
		profData->lastLevelUp = now;
}


void Lmd_Prof_Free(void *target) {
	profData_t *profData = (profData_t *)target;
	if (profData->data)
		G_Free(profData->data);
}

accDataModule_t Accounts_Profession = {
	// dataFields
	ProfsFields,

	// numDataFields
	ProfsFields_Count,

	// dataSize
	sizeof(profData_t),

	// allocData
	Lmd_Prof_Alloc,

	// freeData
	Lmd_Prof_Free,

	// Load completed
	Lmd_Prof_AccountLoaded
};

void Accounts_Prof_Register() {
	AccProfessionDataDataIndex = Lmd_Accounts_AddDataCategory(&Accounts_Profession);
}

void Accounts_Prof_ClearData(Account_t *acc) {
	profData_t *data = PROFDATA(acc);
	if (Professions[data->profession]->data.fields) {
		Lmd_Data_FreeFields(data->data, Professions[data->profession]->data.fields, Professions[data->profession]->data.count);
		memset(data->data, 0, Professions[data->profession]->data.size);
		Lmd_Accounts_Modify(acc);

	}

}

void* Accounts_Prof_GetFieldData(Account_t *acc) {
	if (!acc)
		return NULL;
	profData_t *data = PROFDATA(acc);
	return data->data;
}
void Accounts_Prof_SetModified(Account_t *acc) {
	Lmd_Accounts_Modify(acc);
}

int Accounts_Prof_GetProfession(Account_t *acc) {
	if (!acc)
		return 0;
	profData_t *data = PROFDATA(acc);
	return data->profession;
}

//FIXME: should be a profession callback.
int Jedi_GetAccSide(Account_t *acc);
qboolean PlayerAcc_Prof_CanUseProfession(gentity_t *ent) {
	if (!ent->client->pers.Lmd.account)
		return qfalse;
	int prof = Accounts_Prof_GetProfession(ent->client->pers.Lmd.account);
	if (prof == PROF_JEDI) {
		//Check if we are not the opposite, since someone with nothing set should become their profession.
		if (ent->client->ps.trueNonJedi)
			return qfalse;
		//Ufo: added missing g_forceBasedTeams check
		if (g_gametype.integer >= GT_TEAM && g_forceBasedTeams.integer) {
			int side = Jedi_GetAccSide(ent->client->pers.Lmd.account);
			if (side != 0) {
				if (side == FORCE_DARKSIDE && ent->client->sess.sessionTeam != TEAM_RED)
					return qfalse;
				else if (side == FORCE_LIGHTSIDE && ent->client->sess.sessionTeam != TEAM_BLUE)
					return qfalse;
			}
		}
	}
	else if (prof == PROF_MERC) {
		//Check if we are not the opposite, since someone with nothing set should become their profession.
		if (ent->client->ps.trueJedi)
			return qfalse;
	}
	return qtrue;
}

void Accounts_Prof_SetProfession(Account_t *acc, int value) {
	if (!acc) return;
	//iomatix save 
	int level = Accounts_Prof_GetLevel(acc);
	if (level < 1) level = 1;
	if (Accounts_Prof_GetProfession(acc) == PROF_JEDI)Accounts_SetLevel_jedi(acc, level);
	else if (Accounts_Prof_GetProfession(acc) == PROF_MERC)Accounts_SetLevel_merc(acc, level);
	//

	profData_t *data = PROFDATA(acc);

	Accounts_Prof_ClearData(acc);
	level = 0; //iomatix: reset level to 0!
	G_Free(data->data);
	data->profession = value;
	data->data = G_Alloc(Professions[value]->data.size);
	memset(data->data, 0, Professions[value]->data.size);
	//iomatix load
	if (value == PROF_JEDI) level = Accounts_GetLevel_jedi(acc);
	else if (value == PROF_MERC) level = Accounts_GetLevel_merc(acc);
	if (level < 1) level = 1;
	data->level = level;
	Lmd_Accounts_Modify(acc);



}

int Accounts_Prof_GetLevel(Account_t *acc) {
	if (!acc) return 0;
	profData_t *data = PROFDATA(acc);
	return data->level;
}

void Accounts_Prof_SetLevel(Account_t *acc, int value) {
	if (!acc) return;
	profData_t *data = PROFDATA(acc);
	if (value < 1)value = 1;
	data->level = value;
	data->lastLevelUp = Time_Now();
	Lmd_Accounts_Modify(acc);

	//iomatix saving
	if (Accounts_Prof_GetProfession(acc) == PROF_MERC)Accounts_SetLevel_merc(acc, value);
	else if (Accounts_Prof_GetProfession(acc) == PROF_JEDI)Accounts_SetLevel_jedi(acc, value);
}

int Accounts_Prof_GetLastLevelup(Account_t *acc) {
	if (!acc)
		return 0;
	profData_t *data = PROFDATA(acc);
	return data->lastLevelUp;
}

#if 0
int PlayerAcc_Prof_GetSkill(gentity_t *ent, int prof, int skill) {
	//For gametypes.
	//The team is set before a spawn, so we will never be a spectator and spawning.
	if (ent->client->sess.sessionTeam != TEAM_SPECTATOR) {
		//duplicated from Accounts_Prof_GetSkill, but we need to confirm it here.
		if (PlayerAcc_Prof_GetProfession(ent) != prof)
			return 0;

		if (!PlayerAcc_Prof_CanUseProfession(ent))
			return 0;

	}
	return Accounts_Prof_GetSkill(ent->client->pers.Lmd.account, prof, skill);
}

void Accounts_Prof_SetSkill(Account_t *acc, int prof, int skill, int value) {
	if (!acc)
		return;
	if (Accounts_Prof_GetProfession(acc) != prof)
		return;
	return Professions[prof]->setSkillValue(acc, skill, value);

}
#endif

char* Professions_GetName(int prof) {
	return Professions[prof]->name;
}

void Professions_PlayerSpawn(gentity_t *ent)
{
	if (PlayerAcc_Prof_CanUseProfession(ent)) {
		int prof = PlayerAcc_Prof_GetProfession(ent);
		if (Professions[prof]->spawn)
			Professions[prof]->spawn(ent);
	}
}

void SetDefaultSkills(Account_t *acc, int prof, profSkill_t *skill) {
	int i;
	for (i = 0; i < skill->subSkills.count; i++) {
		SetDefaultSkills(acc, prof, &skill->subSkills.skill[i]);
	}
	if (skill->setValue && skill->levels.min > 0) {
		skill->setValue(acc, skill, skill->levels.min);
	}
}

void Professions_SetDefaultSkills(Account_t *acc, int prof) {
	SetDefaultSkills(acc, prof, &Professions[prof]->primarySkill);

}

//iomatix fixed up that shit! ;)
int Professions_Add_That_Amount_SkillPoints(int level) //that may be useful for disps !! iomatix
{
	int p;

	p = lmd_skillpoints_perlevel.integer; //starting 
	if (level > MASTER_LEVEL)//additional points bonus for mastery level!
	{
		p += floor((float)(lmd_skillpoints_perlevel.integer / 3));
		if (level % 3 == 0) p += 1; //add the missing point every 3 levels (part of bonus).
	}

	return p;
}
//iomatix:
int Professions_TotalSkillPoints(Account_t *acc) {

	if (!acc)return 0;
	int p;

	int level = Accounts_Prof_GetLevel(acc);
	int prof = Accounts_Prof_GetProfession(acc);
	//additional points for new game plus!
	int nwg = Accounts_GetNewGamePlus_count(acc)*lmd_skillpoints_perlevel.integer;
	p = level * Professions_Add_That_Amount_SkillPoints(level) + nwg;
	return p;
}
int Professions_TotalSkillPoints_Basic(int level) {
	int p;
	p = level * Professions_Add_That_Amount_SkillPoints(level);
	return p;
}

int Professions_SkillCost(profSkill_t *skill, int level) {
	//If we have a min level, dont count it
	if (level <= skill->levels.min) return 0;
	level -= skill->levels.min;
	if (skill->points.type == SPT_TRIANGULAR) return 1 + (level * (level + 1) * 2);
	else if (skill->points.type == SPT_LINEAR) return level;
	else if (skill->points.type == SPT_LINEAR_2) return level * 2;
	else if (skill->points.type == SPT_LINEAR_5) return level * 5;
	else if (skill->points.type == SPT_LINEAR_10) return level * 10;
	else if (skill->points.type == SPT_LINEAR_12) return level * 12;
	return 0;

	////linear
	//return skill->points.cost + (skill->points.multiplier * level);
}

int Professions_UsedSkillPoints(Account_t *acc, int prof, profSkill_t *skill) {
	//Count upward from the current location and total up all skill points.

	int i, sum = 0;

	if (!acc) {
		return 0;
	}

	if (skill == NULL) skill = &Professions[prof]->primarySkill;

	if (skill->getValue) {
		int level = skill->getValue(acc, skill) - skill->levels.min;
		sum = Professions_SkillCost(skill, level);
	}

	for (i = 0; i < skill->subSkills.count; i++) {
		sum += Professions_UsedSkillPoints(acc, prof, &skill->subSkills.skill[i]);
	}
	return sum;
}

int Professions_AvailableSkillPoints(Account_t *acc, int prof, profSkill_t *skill, profSkill_t **parent) {
	int level = 0;

	if (!acc) {
		return 0;
	}

	//Trace backwards from the given skill to find the next point group.
	// TODO: Support reuse of a single skill def on multiple parents?
	while (skill = skill->parent) {
		if (skill->subSkills.pointGroup) {
			//point groups must have an associated skill.
			assert(skill->getValue);
			if (skill->getValue) {
				level = skill->getValue(acc, skill);
			}
			break;
		}
		else if (!skill->parent) {
			level = Accounts_Prof_GetLevel(acc);
			break;
		}
	}

	if (parent)
		*parent = skill;

	return Professions_TotalSkillPoints(acc) - Professions_UsedSkillPoints(acc, prof, skill);
}

int recallDroppedCredits(gentity_t *ent);
void Profession_Reset(gentity_t *ent)
{
	PlayerAcc_SetScore(ent, 10);
	PlayerAcc_SetExperience(ent, 0); //reset experience points.

	recallDroppedCredits(ent);

	Professions_SetDefaultSkills(ent->client->pers.Lmd.account, PlayerAcc_Prof_GetProfession(ent));


	ent->client->ps.fd.forceDoInit = qtrue;
	ent->flags &= ~FL_GODMODE;
	if (ent->client->sess.sessionTeam != TEAM_SPECTATOR) {
		ent->client->ps.persistant[PERS_SCORE]++;
		ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
		player_die(ent, ent, ent, 100000, MOD_SUICIDE);
	}
}
qboolean Professions_ChooseProf(gentity_t *ent, int prof) {
	int playerProfession = PlayerAcc_Prof_GetProfession(ent);
	int myLevel = PlayerAcc_Prof_GetLevel(ent);
	gentity_t *t = NULL;
	int flags = PlayerAcc_GetFlags(ent);

	if (!ent->client->pers.Lmd.account) {
		if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3You need to be registered to do this. Type '\\help register' for more information.");
		else trap_SendServerCommand(ent->s.number, "chat \"^3You need to be registered to do this. Type '\\help register' for more information.\"");
		return qfalse;
	}

	if (prof < 0 || prof >= NUM_PROFESSIONS) {
		if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3That is not a valid profession.");
		else trap_SendServerCommand(ent->s.number, "chat \"^1That is not a valid profession.\"");
		return qfalse;
	}

	if (prof == playerProfession) {
		if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3Your profession was not changed.");
		else
			trap_SendServerCommand(ent->s.number, "chat \"^3Your profession was not changed. ^1You can not switch to the same profession.\"");
		return qfalse;
	}

	//formula
	int cost_skillpoints = lmd_profession_fee.integer + lmd_skillpoint_cost.integer * Professions_UsedSkillPoints(ent->client->pers.Lmd.account, playerProfession, NULL) / 10; //reset for 1/10 of the cost
	if (flags & ACCFLAGS_NOPROFCRLOSS) {
		PlayerAcc_AddFlags(ent, -ACCFLAGS_NOPROFCRLOSS);
		if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3Your free profession change has been used up.");
		else
			trap_SendServerCommand(ent->s.number, "chat \"^3Your free profession change has been used up.\"");
	}
	else {
		if (PlayerAcc_GetCredits(ent) <= cost_skillpoints)
		{
			if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3You've paid all your credits.");
			else
				trap_SendServerCommand(ent->s.number, "chat \"^3You've paid all your credits.\"");
			PlayerAcc_SetCredits(ent, 0);


		}
		else
		{



			if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3You've paid ^2%i CR", cost_skillpoints));
			else
				trap_SendServerCommand(ent->s.number, va("chat \"^3You've paid ^2%i CR\"", cost_skillpoints));
			PlayerAcc_SetCredits(ent, PlayerAcc_GetCredits(ent) - cost_skillpoints);

		}


	}
	//iomatix:

	PlayerAcc_Prof_SetProfession(ent, prof);

	Profession_Reset(ent);



	if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3Your profession is now: ^2%s", Professions[prof]->name)); else
		trap_SendServerCommand(ent->s.number, va("chat \"^3Your profession is now: ^2%s\"", Professions[prof]->name));
	return qtrue;
}

void Profession_UpdateSkillEffects(gentity_t *ent, int prof) {
	if (prof == PROF_JEDI) WP_InitForcePowers(ent);
}

qboolean Lmd_Prof_SkillIsLeveled(Account_t *acc, int prof, profSkill_t *skill) {
	if (skill->getValue) {
		int value = skill->getValue(acc, skill);
		if (value > skill->levels.min) {
			return qtrue;
		}
	}

	if (skill->subSkills.count > 0) {
		int i;
		for (i = 0; i < skill->subSkills.count; i++) {
			if (Lmd_Prof_SkillIsLeveled(acc, prof, &skill->subSkills.skill[i]))
				return qtrue;
		}
	}
	return qfalse;
}

qboolean Lmd_Prof_SkillIsAchieveable(Account_t *acc, int prof, profSkill_t *skill, profSkill_t **blocker) {
	profSkill_t *parent;
	//track down the tree to find any skills that block our path from being achived.
	while ((parent = skill->parent)) {
		int i;
		for (i = 0; i < parent->subSkills.count; i++) {
			if (&parent->subSkills.skill[i] == skill) continue;
			if (parent->subSkills.skill[i].unique & skill->unique && Lmd_Prof_SkillIsLeveled(acc, prof, &parent->subSkills.skill[i])) {
				if (blocker) *blocker = &parent->subSkills.skill[i];

				//iomatix level is bigger than MASTERLEVEL for Jedi and Sith
				if ((parent->subSkills.skill[i].name == "Jedi" || parent->subSkills.skill[i].name == "Sith") && Accounts_Prof_GetLevel(acc) >= MASTER_LEVEL) continue;


				return qfalse;

			}
		}
		skill = parent;
	}
	return qtrue;
}

void Cmd_SkillSelect_List(gentity_t *ent, int prof, profSkill_t *parent) {
	int i;
	char *s;
	char c;
	profSkill_t *group = NULL;
	int points;
	int level;
	int cost;
	profSkill_t *skill;
	profSkill_t *blocker = NULL;
	qboolean allBlocked = qfalse;

	Account_t *acc = ent->client->pers.Lmd.account;

	if (!acc) {
		return;
	}


	//Pass a child of the parent, to get the points available inside of it.
	points = Professions_AvailableSkillPoints(acc, prof, &parent->subSkills.skill[0], &group);

	if (Lmd_Prof_SkillIsAchieveable(acc, prof, parent, &blocker) == qfalse) {
		allBlocked = qtrue;
	}



	if (allBlocked)
		s = va("^3(^1Blocked by ^2%s^3)", blocker->name);
	else if (parent->getValue && parent->levels.max > parent->levels.min) {
		// Skill has levels
		level = parent->getValue(acc, parent);
		s = va("^3(^2%i^3 out of ^2%i^3)", level, parent->levels.max);
	}
	else {
		s = "";
	}

	Disp(ent, va("^3Skills for ^%c%s^3 %s", (blocker) ? '1' : '2', parent->name, s));
	//trap_SendServerCommand(ent->s.number, va("chat \"^3Skills for ^%c%s^3 %s\"", (blocker) ? '1' : '2', parent->name, s));


	for (i = 0; i < parent->subSkills.count; i++) {
		skill = &parent->subSkills.skill[i];

		if (allBlocked) {
			c = '1';
			s = "";
		}
		else if (Lmd_Prof_SkillIsAchieveable(acc, prof, skill, &blocker) == qfalse) {
			c = '1';
			s = va("^3(^1Blocked by ^2%s^3)", blocker->name);
		}
		else if (skill->getValue && skill->levels.max > skill->levels.min) { // if(skill->index >= 0) {
			level = skill->getValue(acc, skill);
			cost = Professions_SkillCost(skill, level);
			if (level >= skill->levels.max)
				c = '3';
			else if (points < cost)
				c = '1';
			else
				c = '2';

			s = va("^3(^2%i^3 out of ^2%i^3)", level, skill->levels.max);
		}
		else {
			c = '2';
			s = "";
		}

		Disp(ent, va("^%c%-20s %s", c, skill->name, s));
		//trap_SendServerCommand(ent->s.number, va("chat \"^%c%-20s %s\"", c, skill->name, s));
	}

	if (group) {
		if (group->setValue)
			// TODO: Not garenteed that this is the case.
			s = va(" ^3for use inside the ^2%s^3 category.\nLeveling up the ^2%s^3 skill will give you more points for these skills", group->name, group->name);
		else
			s = va(" ^3for use inside the ^2%s^3 category", group->name);
	}
	else
		s = "";

	if (points > 0) {
		if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3You have ^2%i^3 skill point%s available%s.", points, (points != 1) ? "s" : "", s)); else
			trap_SendServerCommand(ent->s.number, va("chat \"^3You have ^2%i^3 skill point%s available%s.\"", points, (points != 1) ? "s" : "", s));
	}
	else
	{
		if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3You have no skill points available%s.", s)); else
			trap_SendServerCommand(ent->s.number, va("chat \"^3You have no skill points available%s.\"", s));
	}
}

void Cmd_SkillSelect_Level(gentity_t *ent, int prof, profSkill_t *skill, qboolean down) {
	//iomatix:
	Account_t *acc = ent->client->pers.Lmd.account;
	int profession_level = Accounts_Prof_GetLevel(acc);

	if (!acc) {
		return;
	}

	if (!skill->getValue || !skill->setValue || skill->levels.max <= skill->levels.min) {
		if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3This skill cannot be leveled."); else
			trap_SendServerCommand(ent->s.number, "chat \"^3This skill cannot be leveled.\"");
		return;
	}

	profSkill_t *blocker;
	if (Lmd_Prof_SkillIsAchieveable(acc, prof, skill, &blocker) == qfalse) {
		if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3You cannot level this skill while ^2%s^3 is leveled.", blocker->name)); else
			trap_SendServerCommand(ent->s.number, va("chat \"^1You cannot level this skill while ^2%s^1 is leveled.\"", blocker->name));
		if (blocker->name == "Sith" || blocker->name == "Jedi") {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3Unlocked at ^2%i^3 profession level.", MASTER_LEVEL)); else
				trap_SendServerCommand(ent->s.number, va("chat \"^3Unlocked at ^2%i^3 profession level.\"", MASTER_LEVEL));
		}
		return;
	}

	int level = skill->getValue(acc, skill);
	if (down) {
		if (!skill->levels.canRemove) {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3This skill cannot be leveled down."); else
				trap_SendServerCommand(ent->s.number, "chat \"^1This skill cannot be leveled down.\"");
			return;
		}
		if (level <= skill->levels.min) {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3This skill is already at its lowest level."); else
				trap_SendServerCommand(ent->s.number, "chat \"^3This skill is already at its lowest level.\"");
			return;
		}
		level--;
		/*
		   if (skill->setValue(acc, skill, level))
		   {
			   Profession_UpdateSkillEffects(ent, prof);
			   Disp(ent, "^3Skill is leveled down.");
		   } //free but not refundable
		   */
		return;
	}
	else {
		if (level >= skill->levels.max) {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3This skill is already at its highest level."); else
				trap_SendServerCommand(ent->s.number, "chat \"^3This skill is already at its highest level.\"");
			return;
		}


		//iomatix re-designe
		int nextLevel = level + 1;

		if (lmd_mastery_level_skills_level.integer > 0)
		{
			if (nextLevel > lmd_mastery_level_skills_level.integer && profession_level < MASTER_LEVEL)
			{
				if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^1Unlocked at ^2%i^3 profession level. You need to reach mastery level before increasing this skill.", MASTER_LEVEL)); else
					trap_SendServerCommand(ent->s.number, va("chat \"^1Unlocked at ^2%i^1 profession level. ^3You need to reach ^6Mastery Level ^3before increasing this skill.\"", MASTER_LEVEL));
				return;
			}

		}
		//iomatix:
		int cost = Professions_SkillCost(skill, nextLevel) - Professions_SkillCost(skill, level);
		int points = Professions_AvailableSkillPoints(acc, prof, skill, NULL);

		int Cr_cost = Accounts_GetCredits(acc) - lmd_skillpoint_cost.integer*cost; //the value is ready to set
		if (points < cost) {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^1Not enough Points.\n^3You need ^2%i^3 Point%s more to level up this skill.", cost - points, (cost - points == 1) ? "" : "s")); else
				trap_SendServerCommand(ent->s.number, va("chat \"^1Not enough Points. ^3You need ^2%i^3 Point%s more to level up this skill.\"", cost - points, (cost - points == 1) ? "" : "s"));
			return;
		}
		if (Cr_cost < 0)
		{
			if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^1Not enough Credits.\n^3You need ^3%i^3 Credits more to level up this skill.", -Cr_cost)); else
				trap_SendServerCommand(ent->s.number, va("chat \"^1Not enough Credits. ^3You need ^3%i^3 Credits more to level up this skill.\"", lmd_skillpoint_cost.integer*cost, Cr_cost));
			return;
		}
		//fixed by iomatix.
		//if(!skill) Disp(ent, ("^1MISSING SKILL!")); //debug

		level++;
		nextLevel++;
		Accounts_SetCredits(acc, Cr_cost);
		Disp(ent, va("^3You've paid ^2%i CR^3. ^2%i CR ^3has left.", lmd_skillpoint_cost.integer*cost, Cr_cost));
		//trap_SendServerCommand(ent->s.number, va("chat \"^3You've paid ^2%i CR^3. ^2%i CR ^3has left.\"", lmd_skillpoint_cost.integer*cost, Cr_cost));
		if (level >= skill->levels.max) {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3This skill is now at its highest level."); else
				trap_SendServerCommand(ent->s.number, "chat \"^3This skill is now at its highest level.\"");
		}
		else {
			//iomatix
			points -= cost;
			cost = Professions_SkillCost(skill, nextLevel) - Professions_SkillCost(skill, level);
			if (points >= cost) {
				Disp(ent, va("^3You can increase this skill again.  It will cost ^2%i^3 points, leaving you with ^2%i^3 point%s.",
					cost, points - cost, (points - cost == 1) ? "" : "s"));
				const char **descr = skill->levelDescriptions; //desc
				for (int i = 0; i < level; i++) {
					if (descr == NULL)
						break;
					descr++;
				}
				if (*descr != NULL) Disp(ent, va("^3Next level: ^8%s", *descr));
				//trap_SendServerCommand(ent->s.number, va("chat \"^3Next level: ^8%s.\"", *descr));
			}
			//else Disp(ent, "^2You do not have enough points to increase this skill."); iomatix: we don't need that it's only disturbing!!! who the hell get idea to disp it is good deal??? 
		}
	}

	//check 
	if (skill->setValue(acc, skill, level)) {
		if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3The ^2%s^3 skill is now at level ^2%i^3.", skill->name, level)); else
			trap_SendServerCommand(ent->s.number, va("chat \"^3The ^2%s^3 skill is now at level ^2%i^3.\"", skill->name, level));
		const char **descr_now = skill->levelDescriptions; //desc
		for (int i = 0; i < level - 1; i++) {
			if (descr_now == NULL)
				break;
			descr_now++;
		}
		if (*descr_now != NULL) Disp(ent, va("^3%s upgraded: ^2%s", skill->name, *descr_now));
		//trap_SendServerCommand(ent->s.number, va("chat \"^3%s upgraded: ^2%s\"", skill->name, *descr_now));

		Profession_UpdateSkillEffects(ent, prof);
	}
	else Disp(ent, ("^1Something gone wrong!")); //debug
}

void Cmd_SkillSelect(gentity_t *ent, int prof, profSkill_t *skill, int depth) {
	//depth is the current argument depth that matches our skill
	//skills jetpack: 1
	//skills jetpack fuel: 2
	char arg[MAX_STRING_CHARS];
	Account_t *acc = ent->client->pers.Lmd.account;
	if (!acc) {
		return;
	}

	trap_Argv(depth + 1, arg, sizeof(arg));
	if (arg[0]) {
		if (Q_stricmp(arg, "up") == 0) {
			Cmd_SkillSelect_Level(ent, prof, skill, qfalse);
		}
		else if (Q_stricmp(arg, "down") == 0) {
			Cmd_SkillSelect_Level(ent, prof, skill, qtrue);
		}
		else if (skill->subSkills.count > 0) {
			int index = -1, i;
			int arglen = strlen(arg);
			for (i = 0; i < skill->subSkills.count; i++) {
				if (Q_stricmpn(skill->subSkills.skill[i].name, arg, arglen) == 0) {
					if (index == -1)
						index = i;
					else {
						index = -2;
						break;
					}
				}
			}
			if (index == -1) {
				if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3\'^2%s^3\' does not match any skills.", arg)); else
					trap_SendServerCommand(ent->s.number, va("chat \"^3\'^2%s^3\' does not match any skills.\"", arg));
			}
			else if (index == -2) {
				if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3Multiple skills match \'^2%s^3\'.", arg)); else
					trap_SendServerCommand(ent->s.number, va("chat \"^3Multiple skills match \'^2%s^3\'.\"", arg));
			}
			else
				Cmd_SkillSelect(ent, prof, &skill->subSkills.skill[index], depth + 1);
			return;
		}
	}
	else {
		profSkill_t *blocker;
		int i;
		int level = 0;
		char *cmd = ConcatArgs(0);

		if (skill->getValue) {
			level = skill->getValue(acc, skill);
		}

		Disp(ent, va("^2%s", skill->name));
		//trap_SendServerCommand(ent->s.number, va("chat \"^2%s\"", skill->name));
		if (skill->subSkills.count > 0) {
			//Display the help text before the subskill list.
			Disp(ent, va("^5%s", skill->baseDescription));
			//trap_SendServerCommand(ent->s.number, va("chat \"^5%s\"", skill->baseDescription));
			Cmd_SkillSelect_List(ent, prof, skill);
			Disp(ent, va("^3Use ^2/%s <skill>^3 to manipulate skills within this skill.", cmd));
			//trap_SendServerCommand(ent->s.number, va("chat \"^3Use ^2/%s <skill>^3 to manipulate skills within this skill.\"", cmd));
		}
		else {
			if (Lmd_Prof_SkillIsAchieveable(acc, prof, skill, &blocker) == qfalse) {
				if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3You cannot level this skill while ^2%s^3 is leveled.", blocker->name)); else
					trap_SendServerCommand(ent->s.number, va("chat \"^1You cannot level this skill while ^2%s^1 is leveled.\"", blocker->name));
				if (blocker->name == "Sith" || blocker->name == "Jedi") {
					if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3Unlocked at ^2%i^3 profession level.", MASTER_LEVEL)); else
						trap_SendServerCommand(ent->s.number, va("chat \"^3Unlocked at ^6%i^3 profession level.\"", MASTER_LEVEL));
				}
			}
			else if (skill->getValue) {
				//Only show if it was not already shown by SkillSelect_List
				Disp(ent, va("^3Level ^2%i^3 out of ^2%i^3", level, skill->levels.max));
				//trap_SendServerCommand(ent->s.number, va("chat \"^3Level ^2%i^3 out of ^2%i^3\"", level, skill->levels.max));
			}
			Disp(ent, va("^5%s", skill->baseDescription));
			//trap_SendServerCommand(ent->s.number, va("chat \"^5%s\"", skill->baseDescription));
		}

		if (level < skill->levels.max) {
			const char **descr = skill->levelDescriptions;
			for (i = 0; i < level; i++) {
				if (descr == NULL)
					break;
				descr++;
			}
			if (*descr != NULL)
				Disp(ent, va("^2Next level: ^5%s", *descr));
			//trap_SendServerCommand(ent->s.number, va("chat \"^2Next level: ^5%s.\"", *descr));
		}

		if (skill->setValue && skill->levels.max > skill->levels.min) {
			int points = Professions_AvailableSkillPoints(acc, prof, skill, NULL);
			int cost = Professions_SkillCost(skill, level + 1) - Professions_SkillCost(skill, level);
			int cost_cr = cost * lmd_skillpoint_cost.integer;  //cost credits
			int cr_player = Accounts_GetCredits(acc);
			if (level < skill->levels.max) {
				if (points >= level + 1 && cost_cr <= cr_player) {
					Disp(ent, va("^3Use ^2/%s up^3 to increase the ^2%s^3 skill.", cmd, skill->name));
					if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3The cost is: ^2%i^3CR. Leaving you with ^2%i^3CR.", cost_cr, cr_player - cost_cr)); else
						trap_SendServerCommand(ent->s.number, va("chat \"^3The Cost is ^2%i^3 point%s and ^2%i CR^3.\"", cost, (cost == 1) ? "" : "s", cost_cr));

				}
				else {
					if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3You do not have enough points or credits to increase the ^2%s^3 skill.", skill->name)); else
						trap_SendServerCommand(ent->s.number, va("chat \"^1You do not have enough points or credits to increase the ^2%s^1 skill.\"", skill->name));
				}
			}
			else
			{
				if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3The ^2%s^3 skill is at its maximum level", skill->name)); else
					trap_SendServerCommand(ent->s.number, va("chat \"^3The ^2%s^3 skill is at its maximum level.\"", skill->name));
			}
			if (skill->levels.canRemove) {
				if (level > skill->levels.min) //trap_SendServerCommand(ent->s.number, va("chat \"^3Use ^2/%s down^3 to decrease the ^2%s^3 skill.  You will regain ^2%i^3 point%s, bringing you up to ^2%i^3 point%s total.\"", 
					//cmd, skill->name, cost, (cost == 1) ? "" : "s", points + cost, (points + cost == 1) ? "" : "s"));
					Disp(ent, va("^3Use ^2/%s down^3 to decrease the ^2%s^3 skill.  You will regain ^2%i^3 point%s, bringing you up to ^2%i^3 point%s total.",
						cmd, skill->name, cost, (cost == 1) ? "" : "s", points + cost, (points + cost == 1) ? "" : "s"));

				else
				{
					if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3The ^2%s^3 skill is at its minimum level.", skill->name)); else
						trap_SendServerCommand(ent->s.number, va("chat \"^3The ^2%s^3 skill is at its minimum level.\"", skill->name));
				}
			}
		}
	}
}

extern vmCvar_t g_maxForceLevel;
void Cmd_SkillSelect_f(gentity_t *ent, int iArg) {
	//Ufo:
	if (ent->client->ps.duelInProgress) {
		if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3You cannot select skills at this time."); else
			trap_SendServerCommand(ent->s.number, "chat \"^1You cannot select skills at this time. ^3Duel in progress...\"");
		return;
	}

	int prof = PlayerAcc_Prof_GetProfession(ent);

	if (!Professions[prof]->primarySkill.subSkills.count) {
		if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3This profession has no skills."); else
			trap_SendServerCommand(ent->s.number, "chat \"3This profession has no skills.\"");
		return;
	}
	Disp(ent, "^4===========================================");
	Cmd_SkillSelect(ent, prof, &Professions[prof]->primarySkill, 0);
	Disp(ent, "^4===========================================");
}

void Cmd_ResetSkills_f(gentity_t *ent, int iArg) {
	Account_t *acc = ent->client->pers.Lmd.account;

	int myCredits = PlayerAcc_GetCredits(ent);
	int prof = PlayerAcc_Prof_GetProfession(ent);
	int used;

	if (!acc) {
		return;
	}

	char arg[MAX_TOKEN_CHARS];
	trap_Argv(1, arg, sizeof(arg));

	used = Professions_UsedSkillPoints(acc, prof, &Professions[prof]->primarySkill);
	if (Q_stricmp("confirm", arg) == 0)
	{





		if (Professions[prof]->primarySkill.subSkills.count == 0) {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3This profession has no skills."); else
				trap_SendServerCommand(ent->s.number, "chat \"^3This profession has no skills.\"");
			return;
		}



		if (used == 0) {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3All your skills are already at their lowest level."); else
				trap_SendServerCommand(ent->s.number, "chat \"^3All your skills are already at their lowest level.\"");
			return;
		}

		int cost = used * lmd_skillpoint_cost.integer / 10;

		if (myCredits < cost) {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3The cost to reset your skills is ^2CR %i^3.", cost)); else
				trap_SendServerCommand(ent->s.number, va("chat \"^3The cost to reset your skills is ^2CR %i^3.\"", cost));
			return;
		}
		if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3Paid ^2%i CR ^3to reset your skills.", cost)); else
			trap_SendServerCommand(ent->s.number, va("chat \"^3Paid ^2%i CR ^3to reset your skills.\"", cost));
		PlayerAcc_SetCredits(ent, myCredits - cost);
		Accounts_Prof_ClearData(ent->client->pers.Lmd.account);
		Professions_SetDefaultSkills(ent->client->pers.Lmd.account, prof);
		Profession_UpdateSkillEffects(ent, prof);

		if (lmd_old_commands_disp.integer == 1)Disp(ent, "^2Your skills have been reseted!"); else
			trap_SendServerCommand(ent->s.number, "chat \"^2Your skills have been reseted!\"");
	}
	else
	{
		int cost = used * lmd_skillpoint_cost.integer / 10;
		if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3Skill reset will cost ^2%iCR.\n^3Type ^8resetskills confirm ^3to reset your skills.", cost));
		else {
			trap_SendServerCommand(ent->s.number, va("chat \"^3Skill reset will cost ^2%iCR.\"", cost));
			trap_SendServerCommand(ent->s.number, "chat \"^3Type ^8resetskills confirm ^3to reset your skills.\"");
		}
	}
}


//iomatix:
int Professions_LevelCost_EXP(int prof, int level) {
	int nextlevel = level + 1;
	//450 
	int cost;
	if (lmd_exp_for_level.integer <= 1) lmd_exp_for_level.integer = 450; //default

	if (nextlevel < 16) cost = lmd_exp_for_level.integer * level * nextlevel / 7.5f;
	else if (nextlevel < MASTER_LEVEL)	cost = lmd_exp_for_level.integer * level * nextlevel / 7.3f;
	else cost = lmd_exp_for_level.integer * level * nextlevel / 6.426f;									//harder formula for players above 40 lvl

	return cost;
}

int Professions_LevelCost(int prof, int level, int time) {
	int nextlevel = level + 1;
	int cost = (int)(LEVEL_COST * (nextlevel * (nextlevel + 1)));

	// Allow negative?
	if (LEVEL_TIME_REDUCE != 0) {
		if (LEVEL_TIME_THRESH > 0 && time > LEVEL_TIME_THRESH)
			time = LEVEL_TIME_THRESH;
		cost -= (int)floor((float)time / LEVEL_TIME_REDUCE);
	}

	if (cost < LEVEL_COST * nextlevel * 2) {
		cost = LEVEL_COST * nextlevel * 2;
	}
	return cost;
}

typedef struct Cmd_BuyLevel_Confirm_Data_s {
	int cost;
	int level;
}Cmd_BuyLevel_Confirm_Data_t;

void Cmd_BuyLevel_Confirm(gentity_t *ent, void *dataptr) {
	Cmd_BuyLevel_Confirm_Data_t *data = (Cmd_BuyLevel_Confirm_Data_t *)dataptr;
	int newCr = PlayerAcc_GetCredits(ent) - data->cost;
	if (newCr < 0) {
		if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3You no longer have enough credits to level up. You need ^2%i^3 more to reach a next profession level.", -newCr)); else
			trap_SendServerCommand(ent->s.number, va("chat \"^3You no longer have enough credits to level up. You need ^2%i^3 more to reach a next profession level.\"", -newCr));
		return;
	}

}

void Experience_Level_Up(gentity_t *ent)
{
	int playerLevel = PlayerAcc_Prof_GetLevel(ent);
	int myExp = PlayerAcc_GetExperience(ent), cost;
	int flags = PlayerAcc_GetFlags(ent);
	int prof = PlayerAcc_Prof_GetProfession(ent);

	if (prof == PROF_ADMIN) return;
	if (playerLevel <= 0) return;
	if (playerLevel >= Professions[prof]->primarySkill.levels.max) return;

	if (flags & ACCFLAGS_NOPROFCRLOSS) PlayerAcc_AddFlags(ent, -ACCFLAGS_NOPROFCRLOSS);
	cost = Professions_LevelCost_EXP(prof, playerLevel);
	int resEXP = PlayerAcc_GetExperience(ent);
	if (resEXP <= 0) {
		return;
	}


	resEXP = resEXP - cost; // new cost
	if (resEXP < 0) {
		Disp(ent, va("^5%i ^3/ ^2%i ^3EXP\n", PlayerAcc_GetExperience(ent), cost));
		Disp(ent, va("^3You need ^5%i EXP ^3more.\n", -resEXP));
		return;
	}

	//checks are completed

	PlayerAcc_SetExperience(ent, resEXP);
	//iomatix:
	playerLevel++;
	PlayerAcc_Prof_SetLevel(ent, playerLevel);
	//credit boxes reward
	if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3Got a ^2Credit Box ^3for reaching a new level!"); else
		trap_SendServerCommand(ent->s.number, "chat \"^3Got a Credit Box for reaching a new level!\"");
	if (playerLevel >= 30 && playerLevel < 60) {
		PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 1);
		if (playerLevel % 5 == 0)PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 1);
		if (playerLevel % 10 == 0)PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 1);
	}
	else if (playerLevel < 100) {
		PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 1);
		if (playerLevel % 5 == 0)PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 1);
		if (playerLevel % 10 == 0)PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 2);
	}
	else {
		PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 2);
		if (playerLevel % 5 == 0)PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 1);
		if (playerLevel % 10 == 0)PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 1);
	}

	//credit boxes special levels bonuses:
	if (playerLevel == 40) {
		PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 15);
		if (lmd_old_commands_disp.integer == 1)Disp(ent, "^5You've got a special bonus for reaching the ^6Mastery Level^5!"); else
			trap_SendServerCommand(ent->s.number, "chat \"^5You've got a special bonus for reaching the ^6Mastery Level^5!\"");
	}
	else if (playerLevel == 60)PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 10);
	else if (playerLevel == 100)PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 20);
	else if (playerLevel == 120) {
		PlayerAcc_SetLootboxes(ent, PlayerAcc_GetLootboxes(ent) + 50);
		if (lmd_old_commands_disp.integer == 1)Disp(ent, "^5You've got a special bonus for reaching ^3level Cap!\n^2Don't hesitate to try a ^3New Game Plus ^2mode.");
		else {
			trap_SendServerCommand(ent->s.number, "chat \"^5You've got a special bonus for reaching ^3The Level Cap!\"");
			trap_SendServerCommand(ent->s.number, "chat \"^2Don't hesitate to try a ^3New Game Plus ^2mode.\"");
		}
	}
	/////


	int NewSkillPoints_value = Professions_Add_That_Amount_SkillPoints(playerLevel);




	cost = Professions_LevelCost_EXP(prof, playerLevel);
	Disp(ent, va("^5%i ^3/ ^2%i ^3EXP\n", resEXP, cost));
	char *playername = PlayerAcc_GetName(ent);
	char *msg_other = va("^1%s ^3becomes more powerful. One's level is ^1%i^3 now.", playername, playerLevel);
	G_LogPrintf("Level up: %s is %i now.\n", playername, playerLevel);
	char *msg_line_1 = "^5Congratulation! Level Increased!";
	char *msg_line_2 = va("^3Your level is ^2%i^3.", playerLevel);
	char *msg_line_3 = va("^2%i ^3skill points recived.", NewSkillPoints_value);



	char *msg = va("%s\n%s\n%s", msg_line_1, msg_line_2, msg_line_3);

	trap_SendServerCommand(-1, va("print \"\n%s\n\"", msg_other));
	if (lmd_old_commands_disp.integer != 1) trap_SendServerCommand_ToAll(ent->s.number, va("chat \"%s\"", msg_other));

	WP_InitForcePowers(ent);


	G_Sound(ent, CHAN_AUTO, G_SoundIndex("sound/interface/secret_area.wav"));
	trap_SendServerCommand(ent->s.number, va("cp \"%s\"", msg));
	if (lmd_old_commands_disp.integer == 1)Disp(ent, msg); else {
		trap_SendServerCommand(ent->s.number, va("chat \"%s\"", msg_line_1));
		trap_SendServerCommand(ent->s.number, va("chat \"%s\"", msg_line_2));
		trap_SendServerCommand(ent->s.number, va("chat \"%s\"", msg_line_3));
	}
	return;

}

void Cmd_BuyLevel_f(gentity_t *ent, int iArg) {
	if (lmd_is_buy_level.integer == 0) {
		int prof = PlayerAcc_Prof_GetProfession(ent);
		if (lmd_old_commands_disp.integer == 1)Disp(ent, "^1You can not buy a level on this server. Gain experience to level up instead."); else
			trap_SendServerCommand(ent->s.number, "chat \"^1You can not buy a level on this server. Gain experience to level up instead.\"");
		if (Auths_AccHasAdmin(ent->client->pers.Lmd.account)) {
			Disp(ent, "^2You are able to change the setup inside of the server.cfg file by adding this line:\n^5set lmd_is_buy_level 1");
		}
	}
	else {
		int playerLevel = PlayerAcc_Prof_GetLevel(ent);
		int myCreds = PlayerAcc_GetCredits(ent), cost;
		int flags = PlayerAcc_GetFlags(ent);
		int prof = PlayerAcc_Prof_GetProfession(ent);
		char arg[MAX_STRING_CHARS];
		trap_Argv(1, arg, sizeof(arg));
		if (prof == PROF_ADMIN) {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3The god profession has no levels."); else
				trap_SendServerCommand(ent->s.number, "chat \"^3The god profession has no levels.\"");
			return;
		}

		if (playerLevel <= 0) {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, "^3You are not logged in."); else
				trap_SendServerCommand(ent->s.number, "chat \"^1You are not logged in.\"");
			return;
		}

		if (playerLevel >= Professions[prof]->primarySkill.levels.max) {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3You have reached the maximum level %i.", Professions[prof]->primarySkill.levels.max)); else
				trap_SendServerCommand(ent->s.number, va("chat \"^3You have reached the maximum level.\"", Professions[prof]->primarySkill.levels.max));
			return;
		}


		if (flags & ACCFLAGS_NOPROFCRLOSS) {
			PlayerAcc_AddFlags(ent, -ACCFLAGS_NOPROFCRLOSS);
		}

		cost = Professions_LevelCost(prof, playerLevel, Time_Now() - Accounts_Prof_GetLastLevelup(ent->client->pers.Lmd.account));
		cost += (Professions_LevelCost_EXP(prof, playerLevel) - PlayerAcc_GetExperience(ent)) / 4;

		int resCr = PlayerAcc_GetCredits(ent) - cost;
		if (resCr < 0) {
			if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3The next level is ^2%i^3 and costs ^2CR %i^3. You need ^2%i^3 more credit%s.", playerLevel + 1, cost, -resCr, (resCr != -1) ? "s" : "")); else
				trap_SendServerCommand(ent->s.number, va("chat \"^3The next level is ^2%i^3 and costs ^2CR %i^3. You need ^2%i^3 more credit%s.\"", playerLevel + 1, cost, -resCr, (resCr != -1) ? "s" : ""));
			return;
		}

		if (Q_stricmp("confirm", arg) == 0) {
			PlayerAcc_SetCredits(ent, resCr);
			//iomatix:

			PlayerAcc_SetExperience(ent, 1 + (Professions_LevelCost_EXP(prof, playerLevel) - PlayerAcc_GetExperience(ent)));
			Experience_Level_Up(ent);



			return;
		}

		Disp(ent, va(
			CT_B"The next level costs "CT_V"%i"CT_B" credits.  Leveling up will leave you with "CT_V"%i"CT_B" credits.\n"
			"Use \'"CT_C"BuyLevel confirm"CT_B"\' to level up .",
			cost,
			resCr));
	}
}

void Profession_DisplayProfs(gentity_t *ent) {
	int prof = PlayerAcc_Prof_GetProfession(ent);
	int i;
	if (lmd_old_commands_disp.integer == 1)Disp(ent, va("^3Your current profession is: ^2%s", Professions[prof]->name)); else
		trap_SendServerCommand(ent->s.number, va("chat \"^3Your current profession is: ^2%s\"", Professions[prof]->name));
	Disp(ent, "^3Available professions are:");
	for (i = 0; i < NUM_PROFESSIONS; i++) {
		if ((i == PROF_ADMIN && !Auths_PlayerHasAuthFlag(ent, AUTH_GODPROF)) || i == PROF_BOT) {
			continue;
		}
		Disp(ent, va("^3%s", Professions[i]->name));
	}
}

void Cmd_Profession_f(gentity_t *ent, int iArg) {
	char arg[MAX_TOKEN_CHARS];
	trap_Argv(1, arg, sizeof(arg));
	if (arg[0]) {
		int i;
		int prof = -1;
		int arglen = strlen(arg);
		for (i = 0; i < NUM_PROFESSIONS; i++) {
			if ((i == PROF_ADMIN && !Auths_PlayerHasAuthFlag(ent, AUTH_GODPROF)) || i == PROF_BOT) {
				continue;
			}
			if (Q_stricmpn(arg, Professions[i]->name, arglen) == 0) {
				if (prof >= 0) {
					prof = -1;
					break;
				}
				prof = i;
			}
		}
		if (prof >= 0) {
			Professions_ChooseProf(ent, prof);
			return;
		}
		Disp(ent, "^3Unknown profession.");
	}
	Profession_DisplayProfs(ent);
}

void Cmd_Cortosis_f(gentity_t *ent, int iArg);
void Cmd_Flame_f(gentity_t *ent, int iArg);
void Cmd_Ionlysaber_f(gentity_t *ent, int iArg);
void Cmd_MercWeapon_f(gentity_t *ent, int iArg);
void Cmd_Ysalamiri_f(gentity_t *ent, int iArg);
//iomatix:




void Cmd_NewGameP_f(gentity_t *ent, int iArg) {
	char arg[MAX_TOKEN_CHARS];
	trap_Argv(1, arg, sizeof(arg));
	if (Q_stricmp("start", arg) == 0)
	{
		int p_level = PlayerAcc_Prof_GetLevel(ent);
		if (p_level <= 1)
		{
			if (lmd_old_commands_disp.integer == 1)Disp(ent, "^1Your profession level is too small to start the new game.");
			else trap_SendServerCommand(ent->s.number, "chat \"^1Your profession level is too small to start the new game.\"");
			return;
		}


		Disp(ent, "^2New Game Plus started.");
		if (p_level > 1) {
			int ng_points = 0;
			PlayerAcc_Prof_SetLevel(ent, 1);
			Profession_Reset(ent);

			if (p_level >= 120)ng_points = 6;
			else if (p_level >= 115)ng_points = 5;
			else if (p_level >= 100)ng_points = 4;
			else if (p_level >= 85)ng_points = 3;
			else if (p_level >= 66)ng_points = 2;
			else if (p_level >= 40)ng_points = 1;
			if (ng_points > 0) {
				if (lmd_old_commands_disp.integer == 1) Disp(ent, "^3The New Game Plus Level is increased by ^2%i. ^3You've got additional skillpoints."); else
					trap_SendServerCommand(ent->s.number, va("chat \"^3The New Game Plus Level is increased by ^2%i. ^3You've got additional skillpoints.\"", ng_points));

			}
			else {
				if (lmd_old_commands_disp.integer == 1) Disp(ent, "^3The New Game Plus Level isn't increased. You must reach at least ^6Mastery Level.");
				else {
					trap_SendServerCommand(ent->s.number, "chat \"^3The New Game Plus Level isn't increased. You must reach at least ^6Mastery Level.\"");
					trap_SendServerCommand_ToAll(ent->s.number, va("chat \"%s ^8started a New Game.\"", PlayerAcc_GetName(ent)));
				}

			}

			PlayerAcc_SetNewGamePlus_count(ent, PlayerAcc_GetNewGamePlus_count(ent) + ng_points);
			WP_InitForcePowers(ent);

		}

	}
	else if (Q_stricmp("reset", arg) == 0) {
		int ng_points = PlayerAcc_GetNewGamePlus_count(ent);
		if (ng_points != 0)
		{
			PlayerAcc_SetNewGamePlus_count(ent, 0);
			if (lmd_old_commands_disp.integer == 1) Disp(ent, "^3Your New Game Plus progress is reseted!");
			else trap_SendServerCommand(ent->s.number, "chat \"^3Your New Game Plus progress is reseted!\"");

		}
		else
		{
			if (lmd_old_commands_disp.integer == 1) Disp(ent, "^1Your New Game Plus Level is^5 0 ^1already.");
			else trap_SendServerCommand(ent->s.number, "chat \"^1Your New Game Plus Level is^5 0 ^1already.\"");

		}
	}
	else {

		Disp(ent, "^3WARNING: ^1The command will reset your progress for the active profession!\n ^1You can reset the newgameplus progress by ^3newgame reset ^1command, it's irreversible!");
		if (lmd_old_commands_disp.integer == 1) Disp(ent, "^3Type ^5newgame start ^3to start New Game Plus mode.\n To gain benefits from NGP your current profession must be at least at ^640 level^3.");
		else trap_SendServerCommand(ent->s.number, "chat \"^3Type ^5newgame start ^3to start New Game Plus mode. To gain benefits from NGP your current profession must be at least at ^640 level^3.\"");
	}

}



/////
cmdEntry_t professionCommandEntries[] = {
{ "buylevel","Buys a level in your current profession if [cost] is enough to buy the next level. Otherwise your current level, and the cost to buy the next level will be displayed.", Cmd_BuyLevel_f, 0, qfalse, 1, 129, 0, 0 },
{ "cortosis", "Equips an armor that turns off hostile lightsabers and lowers incoming splash damage. Prevents usability of heavy splash weapons.", Cmd_Cortosis_f, 0, qfalse, 0, 64, ~(1 << GT_FFA), PROF_MERC },
{ "flame", "Shoots out a spew of flames.", Cmd_Flame_f, 0, qfalse, 1, 257, 0, PROF_MERC },
{ "ionlysaber", "You can't use forcepowers other than heal or drain - but you're also immune to them. Greatly reduces received splash damage.", Cmd_Ionlysaber_f, 0, qfalse, 0, 64, ~(1 << GT_FFA), PROF_JEDI },
{ "profession", "Choose a profession. ^1You will start from level one and lost part of credits if you choose a new profession.", Cmd_Profession_f, 0, qfalse, 1, 256, 0, 0 },
{ "resetskills", "Reset your skills. ^1This costs money! ^3if no argument is provided the cost will be displayed.", Cmd_ResetSkills_f, 0, qfalse, 2, 257, 0, 0 },
{ "skills", "View and raise your profession skills. You can only raise skill levels if you have unallocated skill points. If no argument is provided, your current skill levels will be listed.", Cmd_SkillSelect_f, 0, qfalse, 1, 257,0, 0 },
{ "weapons", "Select or unselect a weapon.", Cmd_MercWeapon_f, 0, qfalse, 1, 257, 0, PROF_MERC },
{ "newgame", "Start a New Game. Gain unique benefits with New Game Plus mode after reaching the Mastery Level.", Cmd_NewGameP_f, 0, qfalse, 1, 129, 0, 0 },
#ifndef LMD_EXPERIMENTAL
{ "ysalamiri","Use your Ysalamiri.  You can use the 'challenge to duel' button instead of this command.", Cmd_Ysalamiri_f, 0, qfalse, 0, 257,0, PROF_MERC },
#endif
{ NULL },
};


/*
TODO:
	Grapple launcher should be moved into a new file, as it may not be merc specific.

Merc flame should leave behind burning oil that lasts a second or so, both on players and the map itself.
	Subskills for range, spread, and burn time.  Increase fuel usage with these?

Possibly:
	If a merc runs out of force (via sith drain), make them move slower (apply rage recovery time).
	Give merc a small speed boost that uses forcepower.
	Skill to increase forcepower: "endurance".

Unlikely:
	Mercs can pick up jedi holocrons which gives a one time protection against the power.
		Once used, the holocron pops out and bounces a few times before disappearing (or shattering like glass).
*/


#include "g_local.h"
#include "Lmd_Data.h"
#include "Lmd_Accounts_Data.h"
#include "Lmd_Prof_Core.h"


//iomatix: Auto-code generation, it's simplier to add new skills, skip that part and use STD_SKILLS_FUNCS(name) where name must match with other structures!
//iomatix: automata
#define STD_SKILLS_GET(name) int Lmd_Prof_Merc_GetSkill_##name(AccountPtr_t accPtr, profSkill_t *skill) { \
				Account_t *acc = (Account_t*)accPtr; \
				mercFields_t *data;	\
				if(!IS_A_MERC(acc)) return 0; \
				data = ACCFIELDDATA(acc);\
				if(!data) return 0; \
				return data->skills.##name; \
		} 

#define IS_A_MERC(acc) (Accounts_Prof_GetProfession(acc) == PROF_MERC)
#define STD_SKILLS_CANSET(name) qboolean Lmd_Prof_Merc_CanSetSkill_##name(AccountPtr_t accPtr, profSkill_t *skill, int value) { \
				Account_t *acc = (Account_t*)accPtr; \
				mercFields_t *data; \
				if(!(acc)) return qfalse; \
				if(!IS_A_MERC(acc)) return qfalse; \
				data = ACCFIELDDATA(acc); \
				if(!data) return qfalse; \
				if(value < skill->levels.min || value > skill->levels.max) return qfalse; \
				return qtrue; \
		}

#define STD_SKILLS_SET(name) qboolean Lmd_Prof_Merc_SetSkill_##name(AccountPtr_t accPtr, profSkill_t *skill, int value) { \
				Account_t *acc = (Account_t*)accPtr; \
				mercFields_t *data; \
				if(!Lmd_Prof_Merc_CanSetSkill_##name(acc, skill, value)) return qfalse; \
				data = ACCFIELDDATA(acc); \
				data->skills.##name = value; \
				Lmd_Accounts_Modify(acc); \
				return qtrue; \
		}

#define STD_SKILLS_FUNCS(name) STD_SKILLS_GET(name) \
STD_SKILLS_CANSET(name) \
STD_SKILLS_SET(name)

typedef struct mercFields_s{
	//int Merc[SK_MERC_NUM_SKILLS];
	struct {
		int melee;
		int weapon;
		int ammo;
		int forceresist;
		int armor;
		int fuel;
		int flame;
		int binoculars;
		int ysalamiri;
		int hp_maxs;
		int sd_maxs;
		int lethality;
		int rifle_master;
		int perfect_aim;
	} skills;
	int weaponselect;
}mercFields_t;
#define	MERCFIELDOFS(x) ((int)&(((mercFields_t *)0)->x))

#define ACCFIELDDATA(acc) (mercFields_t*)Accounts_Prof_GetFieldData(acc)
#define FIELDDATA(ent) (mercFields_t*)Accounts_Prof_GetFieldData(ent->client->pers.Lmd.account)

#define IS_A_MERC(acc) (Accounts_Prof_GetProfession(acc) == PROF_MERC)


#define MercFields_Base(_m) \
	_m##_AUTO(melee, MERCFIELDOFS(skills.melee), F_INT) \
	_m##_AUTO(weapon, MERCFIELDOFS(skills.weapon), F_INT) \
	_m##_AUTO(ammo, MERCFIELDOFS(skills.ammo), F_INT) \
	_m##_AUTO(forceresist, MERCFIELDOFS(skills.forceresist), F_INT) \
	_m##_AUTO(armor, MERCFIELDOFS(skills.armor), F_INT) \
	_m##_AUTO(fuel, MERCFIELDOFS(skills.fuel), F_INT) \
	_m##_AUTO(flame, MERCFIELDOFS(skills.flame), F_INT) \
	_m##_AUTO(binoculars, MERCFIELDOFS(skills.binoculars), F_INT) \
	_m##_AUTO(ysalamiri, MERCFIELDOFS(skills.ysalamiri), F_INT) \
	_m##_AUTO(hp_maxs, MERCFIELDOFS(skills.hp_maxs), F_INT) \
	_m##_AUTO(sd_maxs, MERCFIELDOFS(skills.sd_maxs), F_INT) \
	_m##_AUTO(lethality, MERCFIELDOFS(skills.lethality), F_INT) \
	_m##_AUTO(rifle_master, MERCFIELDOFS(skills.rifle_master), F_INT) \
	_m##_AUTO(perfect_aim, MERCFIELDOFS(skills.perfect_aim), F_INT) \
	_m##_AUTO(weaponselect, MERCFIELDOFS(weaponselect), F_INT)

MercFields_Base(DEFINE_FIELD_PRE)

DATAFIELDS_BEGIN(MercFields)
MercFields_Base(DEFINE_FIELD_LIST)
DATAFIELDS_END

const int MercFields_Count = DATAFIELDS_COUNT(MercFields);
//iomatix:
//20


//skills:
const char *mercSkill_hp_maxs_Descr[] = {
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+13 HP",
	"+25 HP",
	NULL
};
//+272 HP


STD_SKILLS_FUNCS(hp_maxs)

profSkill_t mercSkill_hp_maxs = {
	"Vitality",
	"Increase total Health capacity.",
	mercSkill_hp_maxs_Descr,

	0,
	SkillLevels_20,
	SkillPoints_Linear_10,

	Lmd_Prof_Merc_GetSkill_hp_maxs,
	Lmd_Prof_Merc_CanSetSkill_hp_maxs,
	Lmd_Prof_Merc_SetSkill_hp_maxs,
};
////////////
const char *mercSkill_sd_maxs_Descr[] = {
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+5 Shield",
	"+8 Shield",
	NULL
};

STD_SKILLS_FUNCS(sd_maxs)

profSkill_t mercSkill_sd_maxs = {
	"Defense",
	"Increase total Shield capacity.",
	mercSkill_sd_maxs_Descr,

	0,
	SkillLevels_20,
	SkillPoints_Linear_10,

	Lmd_Prof_Merc_GetSkill_sd_maxs,
	Lmd_Prof_Merc_CanSetSkill_sd_maxs,
	Lmd_Prof_Merc_SetSkill_sd_maxs,
};
///////////
const char *mercSkill_lethality_Descr[] = {
	"5 percent of the Lethality",
	"10 percent of the Lethality",
	"15 percent of the Lethality",
	"20 percent of the Lethality",
	"25 percent of the Lethality",
	NULL
};

STD_SKILLS_FUNCS(lethality)

profSkill_t mercSkill_Lethality = {
	"Lethality",
	"Your attacks pierces through the shields.",
	mercSkill_lethality_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Linear_12,

	Lmd_Prof_Merc_GetSkill_lethality,
	Lmd_Prof_Merc_CanSetSkill_lethality,
	Lmd_Prof_Merc_SetSkill_lethality,
};
//////////
const char *mercSkill_rifle_master_Descr[] = {
	"5 percent of the additional damage using rifles.",
	"10 percent of the additional damage using rifles.",
	"15 percent of the additional damage using rifles.",
	"20 percent of the additional damage using rifles.",
	NULL
};

STD_SKILLS_FUNCS(rifle_master)

profSkill_t mercSkill_rifle_master = {
	"Master of the Rifles",
	"Increases your damage dealt with non-explosive weapons.",
	mercSkill_rifle_master_Descr,

	0,
	SkillLevels_4,
	SkillPoints_Linear_12,

	Lmd_Prof_Merc_GetSkill_rifle_master,
	Lmd_Prof_Merc_CanSetSkill_rifle_master,
	Lmd_Prof_Merc_SetSkill_rifle_master,
};

/////// too op? 4,8,12,16,20,24%? nerf?
const char *mercSkill_perfect_aim_Descr[] = {
	"4 percent chance for additional 30 percent critical damage.",
	"8 percent chance for additional 60 percent of critical damage.",
	"12 percent chance for additional 90 percent of critical damage.",
	"16 percent chance for additional 120 percent of critical damage.",
	"20 percent chance for additional 150 percent of critical damage.",
	"24 percent chance for additional 180 percent of critical damage.",
	NULL
};

STD_SKILLS_FUNCS(perfect_aim)

profSkill_t mercSkill_perfect_aim = {
	"Perfect Aim",
	"Get chance for additional critical damage dealt with non-explosive weapons.",
	mercSkill_perfect_aim_Descr,

	0,
	SkillLevels_6,
	SkillPoints_Linear_12,

	Lmd_Prof_Merc_GetSkill_perfect_aim,
	Lmd_Prof_Merc_CanSetSkill_perfect_aim,
	Lmd_Prof_Merc_SetSkill_perfect_aim,
};

//functions for passive skills:
//GetSkill
int Lmd_Prof_Merc_Getperfect_aimSkill(Account_t *acc) {
	if (!acc) {
		return 0;
	}
	return mercSkill_perfect_aim.getValue(acc, &mercSkill_perfect_aim);
}
int Lmd_Prof_Merc_Getrifle_masterSkill(Account_t *acc) {
	if (!acc) {
		return 0;
	}
	return mercSkill_rifle_master.getValue(acc, &mercSkill_rifle_master);
}
int Lmd_Prof_Merc_GetLethalitySkill(Account_t *acc) {
	if (!acc) {
		return 0;
	}
	return mercSkill_Lethality.getValue(acc, &mercSkill_Lethality);
}
/////Functions:
int Get_Merc_hp_maxs_value(gentity_t *ent)
{
	int value = 55; //starting health

	value += floor(PlayerAcc_Prof_GetLevel(ent) * 2.55); //scale health
	int skillHP = Lmd_Prof_Merc_GetSkill_hp_maxs(ent->client->pers.Lmd.account, &mercSkill_hp_maxs);
	if (skillHP > 0){
		value += 13 * skillHP;
		if (skillHP == mercSkill_hp_maxs.levels.max) value += 12; //bonus for maxed (13+12=25)
	}
	return value;

}

int Get_Merc_sd_maxs_value(gentity_t *ent)
{
	int value = 5; //starting shield

	int skillSD = Lmd_Prof_Merc_GetSkill_sd_maxs(ent->client->pers.Lmd.account, &mercSkill_sd_maxs);
	if (skillSD > 0) {
		value = 5 * skillSD;
		if (skillSD == mercSkill_sd_maxs.levels.max) value += 3; //bonus for maxed (5+3=8)
	}

	return value;


}
////
const char *mercSkill_MartialArts_Descr[] = {
	"Increased melee damage.",
	"Increased melee damage.  Learn to roll out of falls and under low overhangs.",
	"Increased melee damage.  Learn advanced melee techniques, such as grabs and grapples.",
	"Increased melee damage.",
	"Increased melee damage.  More accurately grab others for grapple techniques.",
	NULL
};

STD_SKILLS_FUNCS(melee)
profSkill_t mercSkill_MartialArts = {
	"MartialArts",
	"Higher martial arts skills yields more damage per strike and grapple moves.",
	mercSkill_MartialArts_Descr,
	
	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Merc_GetSkill_melee,
	Lmd_Prof_Merc_CanSetSkill_melee,
	Lmd_Prof_Merc_SetSkill_melee
};

int Lmd_Prof_Merc_GetMeleeSkill(Account_t *acc) {
	if (!acc) {
		return 0;
	}
	return mercSkill_MartialArts.getValue(acc, &mercSkill_MartialArts);
}

const char *mercSkill_Weapons_Descr[] = {
	"Gain 4 weapon credits.  Gain access to the E11 Blaster and Stun Baton.",
	"Gain 11 weapon credits.  Gain access to Gadgets and the Bowcaster.",
	"Gain 17 weapon credits.  Gain access to the Disruptor.",
	"Gain 20 weapon credits.  Gain access to the Flechette.",
	"Gain 24 weapon credits.  Gain access to the Repeater.",
	"Gain 30 weapon credits.",
	"Gain 33 weapon credits.  Gain access to the Heavy Weapons.",
	"Gain 36 weapon credits.",
	NULL
};

STD_SKILLS_FUNCS(weapon)

profSkill_t mercSkill_Weapons = {
	"Weapons",
	"Get access to more advanced weaponry.",
	mercSkill_Weapons_Descr,

	0,
	SkillLevels_8,
	SkillPoints_Default,

	Lmd_Prof_Merc_GetSkill_weapon,
	Lmd_Prof_Merc_CanSetSkill_weapon,
	Lmd_Prof_Merc_SetSkill_weapon,
};



const char *mercSkill_Ammo_Descr[] = {
	"Increase maximum ammo capacity to 120 percent.",
	"Increase maximum ammo capacity to 140 percent.",
	"Increase maximum ammo capacity to 160 percent.",
	"Increase maximum ammo capacity to 180 percent.",
	"Double the maximum ammo capacity.",
	NULL
};
STD_SKILLS_FUNCS(ammo)
profSkill_t mercSkill_Ammo = {
	"Ammo",
	"Bring more ammo into the field.",
	mercSkill_Ammo_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Merc_GetSkill_ammo,
	Lmd_Prof_Merc_CanSetSkill_ammo,
	Lmd_Prof_Merc_SetSkill_ammo,
};

int Lmd_Prof_Merc_GetAmmoSkill(Account_t *acc) {
	if (!acc) {
		return 0;
	}
	return mercSkill_Ammo.getValue(acc, &mercSkill_Ammo);
}


const char *mercSkill_Shield_Descr[] = {
	"Spawn with an extra 15 percent shield charge.",
	"Spawn with an extra 30 percent shield charge.",
	"Spawn with an extra 45 percent shield charge.",
	"Spawn with an extra 60 percent shield charge.",
	"Spawn with an extra 75 percent shield charge.",
	NULL
};
STD_SKILLS_FUNCS(armor)

profSkill_t mercSkill_Shield = {
	"Shield",
	"Bring a higher shield charge into the field.",
	mercSkill_Shield_Descr,
	
	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Merc_GetSkill_armor,
	Lmd_Prof_Merc_CanSetSkill_armor,
	Lmd_Prof_Merc_SetSkill_armor,
};



const char *mercSkill_ForceResistance_Descr[] = {
	"Increased Force Push and Pull resistance.",
	"Increased Force Push and Pull resistance.",
	"Increased Force Push and Pull resistance.",
	"Increased Force Push and Pull resistance.",
	"Never loose your weapons to Force Pull.",
	NULL
};
STD_SKILLS_FUNCS(forceresist)
profSkill_t mercSkill_ForceResistance = {
	"ForceResist",
	"Resist the effects of force powers.",
	mercSkill_ForceResistance_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Merc_GetSkill_forceresist,
	Lmd_Prof_Merc_CanSetSkill_forceresist,
	Lmd_Prof_Merc_SetSkill_forceresist
};

int Lmd_Prof_Merc_GetForceResistanceSkill(Account_t *acc) {
	if (!acc) {
		return 0;
	}
	return mercSkill_ForceResistance.getValue(acc, &mercSkill_ForceResistance);
}

const char *mercSkill_Fuel_Descr[] = {
#ifdef LMD_NEW_JETPACK
	"Burn fuel at 200 points per second.  Regenerate fuel at 12 points every second.",
	"Burn fuel at 100 points per second.  Regenerate fuel at 14 points every second.",
	"Burn fuel at 80 points per second.  Regenerate fuel at 17 points every second.",
	"Burn fuel at 75 points per second.  Regenerate fuel at 25 points every second.",
	"Burn fuel at 65 points per second.  Regenerate fuel at 50 points every second.",
#else
	"Burn fuel at 10 points per second.  Regenerate fuel at 12 points every second.",
	"Burn fuel at 5 points per second.  Regenerate fuel at 14 points every second.",
	"Burn fuel at 3 points per second.  Regenerate fuel at 17 points every second.",
	"Burn fuel at 2.5 points per second.  Regenerate fuel at 25 points every second.",
	"Burn fuel at 1.25 points per second.  Regenerate fuel at 50 points every second.",
#endif
	NULL
};
STD_SKILLS_FUNCS(fuel)
profSkill_t mercSkill_Fuel = {
	"Fuel",
	"Increase your fuel reserves for Jetpack and Flame Burst.",
	mercSkill_Fuel_Descr,
	
	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Merc_GetSkill_fuel,
	Lmd_Prof_Merc_CanSetSkill_fuel,
	Lmd_Prof_Merc_SetSkill_fuel
};



const char *mercSkill_FlameBurst_Descr[] = {
	"Deal 16 points of damage to your target.",
	"Deal 22 points of damage to your target.",
	"Deal 28 points of damage to your target.",
	"Deal 34 points of damage to your target.",
	"Deal 40 points of damage to your target.",
	NULL
};

STD_SKILLS_FUNCS(flame)

profSkill_t mercSkill_FlameBurst = {
	"FlameBurst",
	"Shoot burning fuel onto your target.",
	mercSkill_FlameBurst_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Merc_GetSkill_flame,
	Lmd_Prof_Merc_CanSetSkill_flame,
	Lmd_Prof_Merc_SetSkill_flame
};


const char *mercSkill_StashRange_Descr[] = {
	"Detect a nearby money stash.",
	"Increase the binocular's range by a factor of 2 over the normal.",
	"Increase the binocular's range by a factor of 3 over the normal.",
	"Increase the binocular's range by a factor of 4 over the normal.",
	"Increase the binocular's range by a factor of 6 over the normal.",
	"Increase the binocular's range by a factor of 8 over the normal.",
	NULL
};

STD_SKILLS_FUNCS(binoculars)

profSkill_t mercSkill_StashRange = {
	"StashRange",
	"Use binoculars to locate the money stash.  The binoculars will beep when a stash is nearby, and beep more rapidly as you aim at it.",
	mercSkill_StashRange_Descr,
	//SK_MERC_BINOCULARS,
	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Merc_GetSkill_binoculars,
	Lmd_Prof_Merc_CanSetSkill_binoculars,
	Lmd_Prof_Merc_SetSkill_binoculars,
};

int Lmd_Prof_Merc_GetStashRangeSkill(Account_t *acc) {
	if (!acc) {
		return 0;
	}
	return mercSkill_StashRange.getValue(acc, &mercSkill_StashRange);
}

const char *mercSkill_Ysalamiri_Descr[] = {
	"",
	"",
	"",
	"",
	"",
	"",
	NULL
};

STD_SKILLS_FUNCS(ysalamiri)

profSkill_t mercSkill_Ysalamiri = {
	"Ysalamiri",
	"Create a blast of ysalamiri to counter force powers.",
	mercSkill_Ysalamiri_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Merc_GetSkill_ysalamiri,
	Lmd_Prof_Merc_SetSkill_ysalamiri,
	Lmd_Prof_Merc_SetSkill_ysalamiri
};

int Lmd_Prof_Merc_GetYsalamiriSkill(Account_t *acc) {
	if (!acc) {
		return 0;
	}
	return mercSkill_Ysalamiri.getValue(acc, &mercSkill_Ysalamiri);
}



profSkill_t mercSkills[] = {
	mercSkill_MartialArts,
	mercSkill_Weapons,
	mercSkill_Ammo,
	mercSkill_Shield,
	mercSkill_ForceResistance,
	mercSkill_Fuel,
	mercSkill_FlameBurst,
	mercSkill_StashRange,
	mercSkill_Ysalamiri,
	mercSkill_hp_maxs,
	mercSkill_sd_maxs,
	mercSkill_Lethality,
	mercSkill_rifle_master,
	mercSkill_perfect_aim,
};
const unsigned int mercSkillCount = sizeof(mercSkills) / sizeof(profSkill_t);




int merc_stash_range[6] = {
	512,
	2  * 512,
	3  * 512,
	4  * 512,
	6  * 512,
	8  * 512
};

typedef struct mercItemTable_s{
	char *name;
	int points;
	int num;
}mercItemTable_t;


/*
Weapons sorted by points (with weapon skill boundries included)
==Level 5=== (20)
Repeater	15
==Level 4=== (17)
Flechette	14
==Level 3=== (14)
Disruptor	10
==Level 2=== (9)
Bowcaster	5
Thermals	4
==Level 1=== (4)
E11			3
Stun		1

==Level 5=== (20)
Repeater | Bowcaster
Repeater | Thermals | Stun
Repeater | E11 | Stun | +1
Flechette | Bowcaster | +1
Flechette | Thermals | Stun +1
Flechette | E11 | Stun | +2
Disruptor | Bowcaster | Thermals | Stun
Disruptor | Bowcaster | E11 | Stun | +1
Disruptor | Thermals | E11 | Stun | +2
Bowcaster | Thermals | E11 | Stun | +7

==Level 4=== (17)
Flechette | E11
Flechette | Stun | +2
Disruptor | Bowcaster | Stun | +1
Disruptor | Thermals | E11
Disruptor | E11 | Stun | +3
Bowcaster | Thermals | E11 | Stun | +4

==Level 3=== (14)
Disruptor | Thermals
Disruptor | E11 | +1
Bowcaster | Thermals | E11 | +2

==Level 2=== (9)
Bowcaster | Thermals
Bowcaster | E11 | +1
Thermals | E11 | +2

==Level 1=== (4)
E11 | +1

*/

const mercItemTable_t mercWeaponTable[] = {
#ifndef LMD_NEW_JETPACK
{ "Stun baton", 1, WP_STUN_BATON },
#endif
{ "Pistol", 1, WP_BRYAR_PISTOL},
{ "Blaster", 2, WP_BLASTER },
{ "Bowcaster", 4, WP_BOWCASTER },
{ "Thermal Detonators", 5, WP_THERMAL },
{ "Detonation Packs",10,WP_DET_PACK },
{ "Laser Trip Mines", 12, WP_TRIP_MINE },
{ "Destructive EMP-2", 10, WP_DEMP2 },
{ "Disruptor", 10, WP_DISRUPTOR },
{ "Flechette", 14, WP_FLECHETTE },
{ "Repeater", 15, WP_REPEATER },
{ "Rocket Launcher",23,WP_ROCKET_LAUNCHER },
{ "Concussion",28,WP_CONCUSSION },
};

const unsigned int numMercWeapons = sizeof(mercWeaponTable) / sizeof(mercItemTable_t);

#ifdef LMD_NEW_JETPACK
const int mercWeaponTableMax[] = {
	0,
	1,
	3,
	4,
	5,
	6,
};
#else
const int mercWeaponTableMax[] = {
	1,
	3,
	7,
	9,
	10,
	11,
	11,
	13,
	13,
};
#endif

int mercWeaponPoints[] = {
	1,
	5,
	12,
	18,
	21,
	25,
	31,
	34,
	37
};


int Merc_TotalWeaponPoints(gentity_t *ent){
	mercFields_t *entry = FIELDDATA(ent);
	return mercWeaponPoints[entry->skills.weapon];
}

int Merc_UsedWeaponPoints(gentity_t *ent){
	mercFields_t *entry = FIELDDATA(ent);
	int sum = 0;
	int i;
	for(i = 0;i<numMercWeapons;i++){
		if(entry->weaponselect & (1 << mercWeaponTable[i].num))
			sum += mercWeaponTable[i].points;
	}
	return sum;
}

float Merc_SpeedFactor(gentity_t *ent) {
	//Ufo: slow down only in certain conditions
	return (ent->client->ps.weapon > WP_BOWCASTER || ent->client->jetPackOn) ? 0.8 : 1.0;
}

int MaxAmmo(gentity_t *ent, int ammo);
void Merc_GiveStartingWeapons(gentity_t *ent){
	mercFields_t *entry = FIELDDATA(ent);
	int i;
	int highestWp = WP_BRYAR_PISTOL;
	//sanity checking here
	int usedPoints;
	int weaponSkillLvl = entry->skills.weapon;
	int totalPoints;
	int weaponMask = 0;
	int oldMask = entry->weaponselect;

	//Clean invalid weapons.  Nasty...
	for(i = 0; i < mercWeaponTableMax[weaponSkillLvl]; i++) {
		if(oldMask & (1 << mercWeaponTable[i].num))
			weaponMask |= (1 << mercWeaponTable[i].num);
	}

	if(weaponMask != oldMask) {
		oldMask = entry->weaponselect = weaponMask;
		PlayerAcc_Prof_SetModified(ent);
	}

	usedPoints = Merc_UsedWeaponPoints(ent);
	totalPoints = Merc_TotalWeaponPoints(ent);

	if(usedPoints > totalPoints){
		for(i = mercWeaponTableMax[weaponSkillLvl] - 1; i >= 0; i--){
			if(weaponMask & (1 << mercWeaponTable[i].num)) {
				weaponMask &= ~(1 << mercWeaponTable[i].num);				
				usedPoints -= mercWeaponTable[i].points;
				if(usedPoints <= totalPoints)
					break;
			}
		}
	}

	if(weaponMask != oldMask) {
		oldMask = entry->weaponselect = weaponMask;
		PlayerAcc_Prof_SetModified(ent);
	}

	for(i = 0; i < mercWeaponTableMax[weaponSkillLvl]; i++){
		if(weaponMask & (1 << mercWeaponTable[i].num)){
			ent->client->ps.stats[STAT_WEAPONS] |= (1 << mercWeaponTable[i].num);
			Add_Ammo(ent, weaponData[mercWeaponTable[i].num].ammoIndex, Q3_INFINITE);
			highestWp = i;
		}
	}
	ent->client->ps.weapon = mercWeaponTable[highestWp].num;


}

void Merc_DisplayWeapons(gentity_t *ent){
	mercFields_t *entry = FIELDDATA(ent);
	char disp[MAX_STRING_CHARS] = "";
	int i;
	int weaponSkill = entry->skills.weapon;
	int weaponMask = entry->weaponselect;
	int points = Merc_TotalWeaponPoints(ent) - Merc_UsedWeaponPoints(ent);
	char c;
	Q_strncpyz(disp, "^3Weapon               ^2Points\n", sizeof(disp));
	for(i = 0; i < mercWeaponTableMax[weaponSkill]; i++){
		if(weaponMask & (1 << mercWeaponTable[i].num)){
			c = '2';
		}
		else if(mercWeaponTable[i].points > points) {
			c = '1';
		}
		else {
			c = '5';
		}
		Q_strcat(disp, sizeof(disp), va("^%c%-20s ^2%i\n", c, mercWeaponTable[i].name, mercWeaponTable[i].points));
	}
	if(points < 0) //we will sanity check and correct it in Merc_GiveStartingWeapons when the player spawns.
		points = 0;
	Q_strcat(disp, sizeof(disp), va("^3You have ^2%i^3 weapon credit%s left.", points, (points>1||points==0)?"s":""));
	Disp(ent, disp);
}
void Merc_ChooseWeapon(gentity_t *ent, char *weapon){
	mercFields_t *entry = FIELDDATA(ent);
	int weaponSkill = entry->skills.weapon;
	int weaponMask = entry->weaponselect;
	int wpIndex = -1, wpMask;
	int points = Merc_TotalWeaponPoints(ent) - Merc_UsedWeaponPoints(ent);
	int i;
	int len = strlen(weapon);
	char compare[MAX_STRING_CHARS];
	for(i = 0; i < mercWeaponTableMax[weaponSkill]; i++) {
		Q_strncpyz(compare, mercWeaponTable[i].name, sizeof(compare));
		Q_CleanStr2(compare);
		if(Q_stricmpn(compare, weapon, len) == 0) {
			if(wpIndex == -1) {
				wpIndex = i;
			}
			else {
				//More than one match
				wpIndex = -2;
				break;
			}
		}
	}
	if(wpIndex == -1) {
		Disp(ent, "^3Weapon not found.");
		return;
	}
	else if(wpIndex == -2) {
		Disp(ent, "^3More than one weapon matches that name.");
		return;
	}
	wpMask = (1 << mercWeaponTable[wpIndex].num);
	if(weaponMask & wpMask){
		weaponMask &= ~wpMask;
		Disp(ent, va("^2%s^3 has been unselected.", mercWeaponTable[wpIndex].name));
	}
	else{
		if(mercWeaponTable[wpIndex].points > points){
			Disp(ent, "^3You do not have enough points to get this weapon.  Try unselecting other weapons or increasing your ^2Weapons^3 skill.");
			return;
		}
		weaponMask |= wpMask;
		Disp(ent, va("^2%s^3 has been selected.  You will have this weapon next time you respawn.", mercWeaponTable[wpIndex].name));
	}
	entry->weaponselect = weaponMask;
	PlayerAcc_Prof_SetModified(ent);
}

void Cmd_MercWeapon_f(gentity_t *ent, int iArg){
	mercFields_t *entry = FIELDDATA(ent);
	char arg[MAX_STRING_CHARS];
	int argc = trap_Argc();

	if(entry->skills.weapon <= 0){
		Disp(ent, "^3You must have at least level 1 of the ^2Weapons^3 skill to use this.");
		return;
	}

	if(argc > 1){
		trap_Argv(1, arg, sizeof(arg));
		Q_strlwr(arg);
		Merc_ChooseWeapon(ent, arg);
	}
	else
		Merc_DisplayWeapons(ent);
	return;
}

#ifdef LMD_EXPERIMENTAL
void MercFlame_Missile_Think(gentity_t *ent) {
	ent->nextthink = level.time + 100;
	if(ent->painDebounceTime < level.time) {
		G_PlayEffectID(G_EffectIndex("env/fire_wall"), ent->r.currentOrigin, vec3_origin);
		ent->painDebounceTime = level.time + 100;
	}
	if(ent->genericValue1 < level.time)
		G_FreeEntity(ent);
}
#endif

void UpdateClientRenderBolts(gentity_t *self, vec3_t renderOrigin, vec3_t renderAngles);
void Prof_Merc_Flame(gentity_t *ent){
	mercFields_t *entry = FIELDDATA(ent);
	vec3_t forward;
	vec3_t center, mins, maxs, dir, ent_org, size, v;
	trace_t tr;
	gentity_t *traceEnt;
	float radius, dot, dist;
	//gentity_t *entityList[MAX_GENTITIES];
	int iEntityList[MAX_GENTITIES];
	int e, numListedEntities, i;
	int lvl = entry->skills.flame;

	if(ent->client->sess.sessionTeam == TEAM_SPECTATOR)
		return;

	if(lvl <= 0)
		return;

	if(lvl > 5)
		lvl = 5; //sanity check

	if(ent->client->ps.m_iVehicleNum > 0 && ent->client->ps.m_iVehicleNum < ENTITYNUM_NONE)
		return;


	if(ent->client->Lmd.profSpecial[0] > level.time)
		return;

	//Dont do this if we are busy
	if(ent->client->ps.forceHandExtend != HANDEXTEND_NONE || ent->client->ps.weaponTime > 0)
		return;
		
	//Ufo:
	if (ent->client->Lmd.restrict & 16)
		return;
	
	if (ent->client->pers.Lmd.persistantFlags & SPF_CORTOSIS && ent->client->ps.fd.forceGripBeingGripped < level.time)
		return;

	ent->client->Lmd.profSpecial[0] = level.time + 3000 - (lvl * 400);

	if(ent->client->ps.jetpackFuel < 25)
		return;
	ent->client->ps.jetpackFuel -= 25;
	ent->client->ps.eFlags &= ~EF_INVULNERABLE;
	ent->client->invulnerableTimer = 0;

	AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
	VectorNormalize( forward );

	VectorCopy( ent->client->ps.origin, center );
	//Ufo: we will have a longer radius only if we're being gripped
	if (ent->client->ps.fd.forceGripBeingGripped > level.time)
		radius = 175;
	else
		radius = 125;
	for( i = 0 ; i < 3 ; i++ ){
		mins[i] = center[i] - radius;
		maxs[i] = center[i] + radius;
	}

	UpdateClientRenderBolts(ent, ent->client->ps.origin, ent->client->ps.viewangles);
	
	G_PlayEffectID(G_EffectIndex("env/flame_jet"), ent->client->renderInfo.handRPoint, ent->client->ps.viewangles);
	ent->client->ps.forceHandExtend = HANDEXTEND_FORCEPUSH;
	ent->client->ps.forceHandExtendTime = level.time + 1000;

	numListedEntities = trap_EntitiesInBox(mins, maxs, iEntityList, MAX_GENTITIES);


	for ( e = 0 ; e < numListedEntities ; e++ ){
		traceEnt = &g_entities[iEntityList[e]];

		if(traceEnt == ent || !traceEnt->inuse || !traceEnt->takedamage || traceEnt->health <= 0 )//no torturing corpses
			continue;
		if ( !g_friendlyFire.integer && OnSameTeam(ent, traceEnt))
			continue;
		// find the distance from the edge of the bounding box
		for( i = 0 ; i < 3 ; i++ ){
			if(center[i] < traceEnt->r.absmin[i])
				v[i] = traceEnt->r.absmin[i] - center[i];
			else if(center[i] > traceEnt->r.absmax[i])
				v[i] = center[i] - traceEnt->r.absmax[i];
			else
				v[i] = 0;
		}

		VectorSubtract( traceEnt->r.absmax, traceEnt->r.absmin, size );
		VectorMA( traceEnt->r.absmin, 0.5, size, ent_org );

		//see if they're in front of me
		//must be within the forward cone
		VectorSubtract( ent_org, center, dir );
		VectorNormalize( dir );
		//too wide of an angle
		if ( (dot = DotProduct( dir, forward )) < 0.7 )
		//if ( (dot = DotProduct( dir, forward )) < 0.5 )
			continue;

		//must be close enough
		dist = VectorLength( v );
		if ( dist >= radius )
			continue;

		//in PVS?
		if(!traceEnt->r.bmodel && !trap_InPVS( ent_org, ent->client->ps.origin))
			continue;

		//Now check and see if we can actually hit it
		trap_Trace( &tr, ent->client->ps.origin, vec3_origin, vec3_origin, ent_org, ent->s.number, MASK_SHOT );
		if(tr.fraction < 1.0f && tr.entityNum != traceEnt->s.number)
			continue;

		// ok, we are within the radius, add us to the incoming list

		G_PlayEffectID(G_EffectIndex("env/fire_wall"), traceEnt->r.currentOrigin, vec3_origin);                       
		G_Damage(traceEnt, ent, ent, ent->client->ps.viewangles, NULL, 10 + (lvl * 6), /*DAMAGE_NO_ARMOR|*/DAMAGE_NO_KNOCKBACK, MOD_LAVA);

		//escape a force grip if we're in one
		if(traceEnt->client && traceEnt->client->ps.fd.forceGripEntityNum != ENTITYNUM_NONE){
			if(lvl >= traceEnt->client->ps.fd.forcePowerLevel[FP_GRIP]){
				if(g_entities[traceEnt->client->ps.fd.forceGripEntityNum].client)
					g_entities[traceEnt->client->ps.fd.forceGripEntityNum].client->ps.fd.forceGripBeingGripped = 0;
				traceEnt->client->ps.fd.forceGripUseTime = level.time + 2500; //Ufo: was 1000 //since we just broke out of it..
				WP_ForcePowerStop(traceEnt, FP_GRIP);
			}
		}
	}
}

void Cmd_Flame_f(gentity_t *ent, int iArg){
	if (!ent->client->pers.Lmd.account) {
		return;
	}
	if (Lmd_Prof_Merc_GetSkill_flame(ent->client->pers.Lmd.account, &mercSkill_FlameBurst) <= 0) {
		Disp(ent, "^3You must have at least level 1 Flame Burst skill to use this.");
	}
	else{
		//Ufo:
		//Disp(ent, "^3You should be using a key bound to \'+force_lightning\' instead of the flame command...");
		Prof_Merc_Flame(ent);
	}
}


void YsalamiriBlast (vec3_t origin, int lvl){
	int i,count;
	float radius = 128 + (lvl - 1) * 128;
	gentity_t *ent_list[MAX_GENTITIES];
	trace_t tr;

	count = G_RadiusList(origin, radius, NULL, qtrue, ent_list, qfalse);
	for (i = 0; i < count; i++) {
		if (!ent_list[i]->client) {
			continue;
		}
		if (duelInProgress(&ent_list[i]->client->ps)) {
			continue;
		}
		if (ent_list[i]->flags & FL_GODMODE) {
			continue;
		}

		trap_Trace(&tr, origin, NULL, NULL, ent_list[i]->client->ps.origin, -1, MASK_PLAYERSOLID);
		if (tr.entityNum != ent_list[i]->s.number) {
			continue;
		}

		if (ent_list[i]->client->ps.powerups[PW_YSALAMIRI] <
			level.time + lvl  * 5000) {
				ent_list[i]->client->ps.powerups[PW_YSALAMIRI] =
					level.time + lvl  * 5000;
		}
	}
	G_PlayEffectID(G_EffectIndex("env/quake_smaller"), origin, vec3_origin);
	G_SoundAtLoc(origin, CHAN_AUTO, G_SoundIndex("sound/effects/thud"));
}

void Cmd_Ysalamiri_f (gentity_t *ent, int iArg){
	mercFields_t *entry = FIELDDATA(ent);
	int lvl;
	if (ent->client->ps.groundEntityNum == ENTITYNUM_NONE) {
		return;
	}
	if (ent->client->ps.legsAnim == BOTH_MEDITATE 
		&& ent->client->ps.torsoAnim == BOTH_MEDITATE) {
			return;
	}
	
	//Ufo:
	if (ent->client->Lmd.restrict & 16)
		return;
	
	if (!(ent->client->ps.stats[STAT_HOLDABLE_ITEMS] & (1<<HI_YSALAMIRI))) {
		Disp(ent, "^3You do not have an Ysalamiri.");
		return;
	}
	ent->client->ps.stats[STAT_HOLDABLE_ITEMS] &= ~(1<<HI_YSALAMIRI);
	lvl = entry->skills.ysalamiri;
	if(lvl <= 0) {
		Disp(ent, "^3You do not have the Ysalamiri skill.");
		return;
	}

	if (ent->client->ps.powerups[PW_YSALAMIRI] > level.time) {
		Disp(ent, "^3You must wait for your Ysalamiri effects to wear off before using another blast.");
		return;
	}

	// TODO: Because this is time dependant, this might cause people to stick to lower levels.
	// Need to rework this.
	ent->client->ps.powerups[PW_YSALAMIRI] = level.time + (lvl + 1) * 5000;
	//Ufo:
	if(lvl > 2 && !(ent->client->pers.Lmd.persistantFlags & SPF_CORTOSIS)){
		trap_UnlinkEntity(ent);
		YsalamiriBlast(ent->client->ps.origin, lvl - 2);
		trap_LinkEntity(ent);
	}
}

//Ufo:
void Cmd_Cortosis_f (gentity_t *ent, int iArg)
{
	if (ent->s.m_iVehicleNum){
		return;
	}

	if (ent->client->ps.weaponTime > 0 
		|| ent->client->ps.weapon > WP_BOWCASTER
		|| ent->client->ps.m_iVehicleNum
		|| ent->client->pers.Lmd.persistantFlags & SPF_CORTOSIS
		|| ent->client->ps.forceHandExtend != HANDEXTEND_NONE){
			return;
	}
	if(ent->client->Lmd.moneyStash){
		return;
	}

	ent->client->ps.trueNonJedi = qtrue;
	ent->client->ps.trueJedi = qfalse;
	ent->client->pers.Lmd.persistantFlags |= SPF_CORTOSIS;
	Disp (ent, "CORTOSIS activated (it can be deactivated by using /kill command)\n");
}

#ifdef LMD_NEW_JETPACK
int Merc_JetpackDefuelBurst(gentity_t *ent) {
	return 5;
}
#endif

int Merc_JetpackRefuel(gentity_t *ent) {
#ifndef LMD_NEW_JETPACK
	//Ufo: Merc's fuel skill now affects refueling rather than defueling
	if(!ent->client->pers.Lmd.account)
		return 150;
	mercFields_t *entry = (mercFields_t*)Accounts_Prof_GetFieldData(ent->client->pers.Lmd.account);
	return 150 - (entry->skills.fuel * 15);
	//return 150;
#else

	if(!ent->client->pers.Lmd.account)
		return 100;
	mercFields_t *entry = (mercFields_t*)Accounts_Prof_GetFieldData(ent->client->pers.Lmd.account);
//#ifdef LMD_EXPERIMENTAL
	int i = trap_Cvar_VariableIntegerValue("lmd_db_mercjprefuel");
	if(i > 0)
		return i;
//#endif
	switch (entry->skills.fuel) {
		default:
			return 100;
		case 1:
			return 80;
		case 2:
			return 70;
		case 3:
			return 60;
		case 4:
			return 40;
		case 5:
			return 20;
	}
#endif
}

int Merc_JetpackDefuel(gentity_t *ent) {
	if(!ent->client->pers.Lmd.account)
		return 5;
	mercFields_t *entry = (mercFields_t*)Accounts_Prof_GetFieldData(ent->client->pers.Lmd.account);
#ifdef LMD_NEW_JETPACK
	int i = trap_Cvar_VariableIntegerValue("lmd_db_mercjpdefuel");
	if(i > 0)
		return i;
	switch (entry->skills.fuel) {
		default:
			return 10;
		case 1:
			return 15;
		case 2:
			return 15;
		case 3:
			return 20;
		case 4:
			return 20;
		case 5:
			return 25;
	}

#else

	switch (entry->skills.fuel) {
		default:
			return 200 /5;
		case 1:
			return 200 / 2;
		case 2:
			return 200;
		case 3:
			return 200 * 1.5;
		case 4:
			return 200 * 2;
		case 5:
			return 200 * 4;
	}

#endif
}





void Merc_Unhook (gentity_t *ent);
#define GRAPPLE_REDRAW_TIME 150

//how much longer to draw the grapple over the redraw time
#define GRAPPLE_REDRAW_OVERSHOOT 10 

void Merc_DrawHook(gentity_t *ent){
	vec3_t pos;
	gentity_t *hook = ent->client->hook;
	gentity_t *te;
	int safety = 0;

	if(!hook) {
		Com_Printf("info: No Hook, but trying to draw\n");
		return;
	}
	else if(Q_stricmp("grappleProj", hook->classname) == 0) {
		//Running projectile.
		return;
	}
	if (!hook || Q_stricmp("grapple", hook->classname)){
		Com_Printf("info: Invalid hook\n");
		ent->client->hook = NULL;
		return;
	}

	if (ent->client->hook->genericValue15 > level.time) {
		return;
	}


	ent->client->hook->genericValue15 = level.time + GRAPPLE_REDRAW_TIME;

	UpdateClientRenderBolts(ent, ent->client->ps.origin, ent->s.angles);
	VectorCopy(ent->client->renderInfo.torsoPoint, pos);
	VectorMA(pos, 0.1, ent->client->ps.velocity, pos); //Try to predict from velocity.

	do {
		if (!hook->inuse) {
			Merc_Unhook(ent);
			return;
		}

		if (hook->nextTrain == hook) {
			Com_Printf("info: grapple has itself as next!\n");
			hook->parent = NULL;
		}

		if (safety++ >= 3) {
			break;
		}

		te = G_TempEntity(hook->s.origin, EV_TESTLINE);
		if(!te){
			Merc_Unhook(ent);
			return;
		}
		te->parent = hook;
		VectorCopy(hook->s.origin, te->s.origin);
		SnapVector( te->s.origin ); // save network bandwidth
		G_SetOrigin( te, te->s.origin );
		SnapVector(pos);
		VectorCopy(pos, te->s.origin2);

		te->s.time2 = GRAPPLE_REDRAW_TIME + GRAPPLE_REDRAW_OVERSHOOT;
		te->s.weapon = 0x000020;
		trap_LinkEntity(te);
		VectorCopy (hook->s.origin, pos);
		hook = hook->nextTrain;

	} while (hook);
}

void Merc_Unhook (gentity_t *ent){
	gentity_t *hook;
	if(!ent->client->hook)
		return;
	if (Q_stricmp("grappleProj", ent->client->hook->classname) == 0) {
		//Are in projectile mode, clear it.
		G_FreeEntity(ent->client->hook);
		ent->client->hook = NULL;
	}
	while ((hook = ent->client->hook) ) {
		ent->client->hook = ent->client->hook->nextTrain;
		hook->neverFree = qfalse;
		G_FreeEntity(hook);
	}
}

void Merc_CreateGrappleNode(gentity_t *player, vec3_t position) {
	gentity_t *node = G_Spawn();
	if (!node) {
		Merc_Unhook(player);
		return;
	}
	//RoboPhred
	node->neverFree = qtrue;

	VectorCopy(position, node->s.origin);
	node->classname = "grapple";
	node->r.contents = 0;
	if(player->client->hook && Q_stricmp("grapple", player->client->hook->classname) == 0) {
		node->genericValue15 = player->client->hook->genericValue15;
	}
	else {
		node->genericValue11 = 0;
		node->genericValue15 = 0;
	}
	node->genericValue11 = -1;

	if (node->nextTrain) {
		node->genericValue4 = node->parent->genericValue4 + 1;
		node->genericValue3 = node->parent->genericValue3 + (int)Distance(node->s.origin, node->parent->s.origin);
	}
	else {
		node->genericValue4 = 0;
		node->genericValue3 = 0;
	}

	trap_LinkEntity(node);

	node->parent = player;
	player->client->hook = node;

}

qboolean Merc_CheckHook (gentity_t *ent){
	//Ufo:
	if (!ent)
		return qfalse;
	gclient_t *client;
	client = ent->client;
	if (!client || !client->hook || !client->hook->inuse) {
		return qfalse;
	}
	if(Q_stricmp("grappleProj", client->hook->classname) == 0) {
		return qfalse;
	}
	if(Q_stricmp("grapple", client->hook->classname) != 0){
		Com_Printf("info: Invalid grapple\n");
		client->hook = NULL;
		return qfalse;
	}
	if (
#ifdef LMD_NEW_JETPACK
		PlayerAcc_Prof_GetProfession(ent) != PROF_MERC && 
#endif
		client->ps.weapon != WP_STUN_BATON ) {
			Merc_Unhook(ent);
			return qfalse;
	}

	if (Distance(client->ps.origin, client->hook->s.origin) + client->hook->genericValue3 > (2048 + 128)) {
		Merc_Unhook(ent);
		return qfalse;
	}

	vec3_t pos;
	trace_t tr;
	gentity_t *te = client->hook;
	if (te->genericValue11 != -1) {
		gentity_t *hte = &g_entities[te->genericValue11];
		if (!hte || !hte->inuse) {
			te->genericValue11 = -1;
			Merc_Unhook(ent);
			return qfalse;
		}
		else {
			VectorAdd(hte->s.pos.trBase, te->s.origin2, te->s.origin);
		}
	}
	if (te->nextTrain) {
		int safety = 0;

		while (te->nextTrain && safety++ < 20) {
			if (te->nextTrain == te) {
				Com_Printf("info: grapple has itself as next!\n");
				te->nextTrain = NULL;
				return qfalse;
			}
			if (te->nextTrain->genericValue11 != -1) {
				gentity_t *hte = &g_entities[te->nextTrain->genericValue11];
				if (!hte || !hte->inuse) {
					te->nextTrain->genericValue11 = -1;
					Merc_Unhook(ent);
					return qfalse;
				}
				else {
					VectorAdd(hte->s.pos.trBase, te->nextTrain->s.origin2, te->nextTrain->s.origin);
				}
			}

			trap_Trace(&tr, te->nextTrain->s.origin, NULL, NULL, te->s.origin, ent->s.number, ent->clipmask);
			if (tr.fraction < 0.99f && tr.entityNum != ENTITYNUM_WORLD && g_entities[tr.entityNum].s.pos.trType != TR_STATIONARY) {
				Merc_Unhook(ent);
				return qfalse;
			}
			te = te->nextTrain;
		}


		//delete node
		trap_Trace(&tr, client->hook->nextTrain->s.origin, NULL, NULL, client->hook->s.origin, ent->s.number, ent->clipmask);

		VectorSubtract(client->ps.origin, tr.endpos, pos);

		if (DotProduct(pos, tr.plane.normal) >= 0) {
			te = client->hook;
			te->nextTrain->genericValue15 = te->genericValue15;
			client->hook = client->hook->nextTrain;
			te->neverFree = qfalse;
			G_FreeEntity(te);
		}
	}

	//add node
	trap_Trace(&tr, client->renderInfo.eyePoint, NULL, NULL, client->hook->s.origin, ent->s.number, ent->clipmask);

	if (tr.fraction < 1.0f && Distance(tr.endpos, client->hook->s.origin) > 8) {
		if (client->hook->genericValue4 >= 10) {
			Merc_Unhook(ent);
			return qfalse;
		}

		Merc_CreateGrappleNode(ent, tr.endpos);
		
	}        
	return qtrue;
}

#ifdef LMD_EXPERIMENTAL
void Merc_HookProj_Think(gentity_t *hook) {
	gentity_t *parent = hook->parent;
	if(parent == NULL || parent->client->hook != hook || !parent->inuse || parent->client->pers.connected != CON_CONNECTED) {
		//Player was lost somehow.
		G_FreeEntity(hook);
		if(parent)
			parent->client->hook = NULL;
		return;
	}
	else if(Distance(hook->r.currentOrigin, parent->client->ps.origin) > 2048) {
		//Too far!
		G_FreeEntity(hook);
		parent->client->hook = NULL;
		return;
	}
	else {
		trace_t tr;
		int length = 2048;

		trap_Trace(&tr, parent->client->renderInfo.eyePoint, NULL, NULL, hook->r.currentOrigin, parent->s.number, hook->clipmask);
		if(tr.fraction == 1.0f) {
			//Didn't hit anything, check to see if we landed.
			//BG_EvaluateTrajectory( &hook->s.pos, level.time, newOrigin);
			vec3_t mins, maxs, thresh;
			VectorSet(thresh, 1.25, 1.25, 1.25);
			VectorCopy(hook->r.mins, mins);
			VectorCopy(hook->r.maxs, maxs);
			VectorSubtract(mins, thresh, mins);
			VectorAdd(maxs, thresh, maxs);
			trap_Trace(&tr, hook->r.currentOrigin, mins, maxs, hook->r.currentOrigin, parent->s.number, hook->clipmask);
		}
		if(tr.fraction != 1.0f &&
			!(tr.entityNum < MAX_CLIENTS || (tr.entityNum != ENTITYNUM_WORLD && g_entities[tr.entityNum].s.pos.trType != TR_STATIONARY) ||
			tr.surfaceFlags & SURF_SKY))
		{

			Merc_CreateGrappleNode(parent, tr.endpos);

			//If we hit a real valid entity, mark it so the grapple sticks to it.
			if (tr.entityNum != ENTITYNUM_WORLD) {
				//Parent's hook has been changed at this point.
				parent->client->hook->genericValue11 = tr.entityNum;
				VectorSubtract(tr.endpos, g_entities[tr.entityNum].s.pos.trBase, parent->client->hook->s.origin2);
			}

			G_FreeEntity(hook);
		}
	}

	if (hook->genericValue15 <= level.time) {
		vec3_t pos;
		gentity_t *te;
		hook->genericValue15 = level.time + GRAPPLE_REDRAW_TIME;

		UpdateClientRenderBolts(parent, parent->client->ps.origin, parent->s.angles);
		VectorCopy(parent->client->renderInfo.torsoPoint, pos);
		VectorMA(pos, 0.1, parent->client->ps.velocity, pos); //Try to predict from velocity.

		te = G_TempEntity(hook->s.origin, EV_TESTLINE);
		if(!te){
			Merc_Unhook(parent);
			return;
		}
		te->parent = hook;
		VectorCopy(hook->s.origin, te->s.origin);
		SnapVector(te->s.origin); // save network bandwidth
		G_SetOrigin(te, te->s.origin);
		SnapVector(te->s.origin);
		VectorCopy(pos, te->s.origin2);
		SnapVector(te->s.origin2);

		te->s.time2 = GRAPPLE_REDRAW_TIME + GRAPPLE_REDRAW_OVERSHOOT;
		te->s.weapon = 0x000020;
		trap_LinkEntity(te);
	}

	hook->nextthink = level.time + 100;
}
#endif

void Merc_FireGrapplingHook( gentity_t *ent, qboolean alt_fire ) {
	if (
#ifdef LMD_EXPERIMENTAL
		PlayerAcc_Prof_GetProfession(ent) != PROF_MERC && 
#endif
		(gameMode(GM_ALLWEAPONS) && (ent->client->ps.powerups[PW_REDFLAG] || ent->client->ps.powerups[PW_BLUEFLAG] || ent->client->ps.powerups[PW_NEUTRALFLAG]))) {
			return;
	}
	
	//Ufo:
	if (ent->client->Lmd.restrict & 4)
		return;

	if(ent->client->Lmd.grappleToggleTime > level.time)
		return;
	ent->client->Lmd.grappleToggleTime = level.time + 1000;

	//TODO: if not alt_fire, instantly grab whatever is a few feet in front.

#ifdef LMD_EXPERIMENTAL
	if (ent->client->hook && ent->client->hook->parent == ent
		&& Q_stricmp("grappleProj", ent->client->hook->classname) == 0) {
			//Already launched one, cancel it.
			G_FreeEntity(ent->client->hook);
			ent->client->hook = NULL;
			return; 
	}

	if (ent->client->hook) {
		//Already hooked, remove it.         
		Merc_Unhook(ent);
		if(!alt_fire)
			return;
	}

	trace_t tr;
	vec3_t muzzle, forward, right, up;
	AngleVectors(ent->client->ps.viewangles, forward, right, up);
	CalcMuzzlePoint ( ent, forward, right, up, muzzle );

	if(!alt_fire) {
		//Not using alt fire, fire a very short range instant grabbing hook
		VectorScale(forward, 100, muzzle);
		VectorAdd(ent->client->ps.origin, muzzle, muzzle);
		trap_Trace(&tr, ent->client->ps.origin, vec3_origin, vec3_origin, muzzle, ent->s.number, MASK_SHOT);
		if(tr.fraction == 1.0f)
			return;

		Merc_CreateGrappleNode(ent, tr.endpos);

		//If we hit a real valid entity, mark it so the grapple sticks to it.
		if (tr.entityNum != ENTITYNUM_WORLD) {
			//Parent's hook has been changed at this point.
			ent->client->hook->genericValue11 = tr.entityNum;
			VectorSubtract(tr.endpos, g_entities[tr.entityNum].s.pos.trBase, ent->client->hook->s.origin2);
		}
	}

	int count = ( level.time - ent->client->ps.weaponChargeTime ) / 100;
	ent->client->ps.weaponChargeTime = 0; //Lugormod glow bug fix

	if ( count < 1 )
	{
		count = 1;
	}
	else if ( count > 10 )
	{
		count = 10;
	}

	gentity_t *hook = G_Spawn();
	if (!hook) {
		return;
	}

	hook->s.modelindex = G_ModelIndex("models/items/forcegem.md3");
	hook->s.apos.trBase[ROLL] = 180;
	hook->s.apos.trBase[YAW] = ent->client->ps.viewangles[YAW];

	hook->classname = "grappleProj";
	hook->s.eType = ET_GENERAL;

	hook->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	hook->r.ownerNum = ent->s.number;
	VectorSet( hook->r.maxs, 2, 2, 2 );
	VectorSet( hook->r.mins, -2, -2, -2 );

	trap_Trace(&tr, ent->client->ps.origin, hook->r.mins, hook->r.maxs, muzzle, ent->s.number, MASK_PLAYERSOLID);

	G_SetOrigin( hook, tr.endpos );
	hook->physicsObject = qtrue;
	hook->s.pos.trType = TR_GRAVITY;
	VectorScale( forward, 100 * count, hook->s.pos.trDelta);
	hook->s.pos.trTime = level.time;
	hook->s.pos.trDelta[2] += 40.0f; //give a slight boost in the upward direction

	hook->clipmask = MASK_SHOT;
	hook->r.contents = CONTENTS_SOLID;

	hook->parent = ent;

	hook->think = Merc_HookProj_Think;
	hook->nextthink = level.time + 100;

	hook->genericValue3 = 0;
	hook->genericValue4 = 0;

	trap_LinkEntity(hook);

	ent->client->hook = hook;

#else

	if (alt_fire && ent->client->hook) {
		Merc_Unhook(ent);
		return;
	}

	trace_t tr;
	vec3_t fPos;
	int length = 2048;

	AngleVectors(ent->client->ps.viewangles, fPos, 0, 0);
	//VectorSet( mins, -0, -0, -0 );
	//VectorSet( maxs, 0, 0, 0 );

	fPos[0] = ent->client->renderInfo.eyePoint[0] + fPos[0]*length;
	fPos[1] = ent->client->renderInfo.eyePoint[1] + fPos[1]*length;
	fPos[2] = ent->client->renderInfo.eyePoint[2] + fPos[2]*length;

	trap_Trace(&tr,ent->client->renderInfo.eyePoint, NULL, 
		NULL, fPos, ent->s.number, ent->clipmask);
	if (tr.fraction == 1.0f 
		|| tr.entityNum < MAX_CLIENTS
		|| (tr.entityNum != ENTITYNUM_WORLD 
		&& g_entities[tr.entityNum].s.pos.trType != 
		TR_STATIONARY) 
		|| (tr.surfaceFlags & SURF_SKY) ) {
			return;
	}
	if (ent->client && ent->client->hook) {
		Merc_Unhook(ent);
	}

	ent->client->hook = G_Spawn();
	if (!ent->client->hook) {
		return;
	}
	if (tr.entityNum != ENTITYNUM_WORLD) {
		ent->client->hook->genericValue11 = tr.entityNum;
		VectorSubtract(tr.endpos, g_entities[tr.entityNum].s.pos.trBase, ent->client->hook->s.origin2);
	} else {
		ent->client->hook->genericValue11 = -1;
	}

	VectorCopy(tr.endpos, ent->client->hook->s.origin);
	ent->client->hook->parent = ent;
	ent->client->hook->classname = "grapple";


	//RoboPhred
	ent->neverFree = qtrue;
	trap_LinkEntity(ent->client->hook);
#endif
}

void Merc_Spawn(gentity_t *ent)
{
#ifdef LMD_NEW_JETPACK
	//Disp(ent, "\n^3============================================\n^3Prototype jetpack in testing.  Please give feedback.\n^3============================================\n");
#endif
	ent->client->ps.trueNonJedi = qtrue;
	ent->client->ps.trueJedi = qfalse;       

	ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_MELEE);
#ifdef LMD_NEW_JETPACK
	ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_STUN_BATON);
#endif
	Merc_GiveStartingWeapons(ent);
	ent->client->ps.stats[STAT_HOLDABLE_ITEMS] |= (1 << HI_BINOCULARS);
	ent->client->ps.stats[STAT_HOLDABLE_ITEM] = BG_GetItemIndexByTag(HI_BINOCULARS, IT_HOLDABLE);

	if (Lmd_Prof_Merc_GetSkill_fuel(ent->client->pers.Lmd.account, &mercSkill_Fuel) > 0) ent->client->ps.stats[STAT_HOLDABLE_ITEMS] |= (1 << HI_JETPACK); //iomatix: new mechanics, jetpack after upgrading the fuel.

				
	ent->client->ps.fd.forcePower = ent->client->ps.fd.forcePowerMax = 75;

	//iomatix:
	ent->client->pers.Lmd.killstreak = 0; //killstreak reset
	//HEALTH
	ent->client->pers.maxHealth = ent->client->ps.stats[STAT_MAX_HEALTH] =  Get_Merc_hp_maxs_value(ent); //iomatix HP skill

	//SHIELD:
	int armorSkill = Lmd_Prof_Merc_GetSkill_armor(ent->client->pers.Lmd.account, &mercSkill_Shield);
	if (armorSkill > 0) {
		//Will be capped to max on spawn finalize if needed.
		ent->client->ps.stats[STAT_ARMOR] += armorSkill * (ent->client->pers.maxHealth * 0.15f);
	}
	//iomatix:
	ent->client->ps.stats[STAT_ARMOR] += Get_Merc_sd_maxs_value(ent);
}

const char *mercProf_Descr[] = {
	NULL
};


profession_t mercProf = {
	"Mercenary",
	"merc",
{MercFields,MercFields_Count,sizeof(mercFields_t) },
	{
		"Mercenary",
		"One trained in weapons.  Mercenaries excel at attacking from a distance and avoid direct confrontations.\n"
		"As mecenaries level up, they can withstand more damage and improve their shields.",
	mercProf_Descr,
	1,
{ 1, 120 }, //LEVEL
{ SPT_NOCOST }, // The primary skill shouldn't factor into used skill points.

NULL,
NULL,
NULL,

{
	mercSkillCount,
	(profSkill_t *)mercSkills
}
	},
	Merc_Spawn
};

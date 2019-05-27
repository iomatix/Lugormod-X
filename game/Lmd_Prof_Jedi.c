
#include "g_local.h"
#include "Lmd_Accounts_Public.h"
#include "Lmd_Data.h"
#include "Lmd_Accounts_Data.h"
#include "Lmd_Prof_Core.h"

//Jedi skills.
enum {
	SK_JEDI_HEAL,
	SK_JEDI_LEVITATION,//hold/duration
	SK_JEDI_SPEED,//duration
	SK_JEDI_PUSH,//hold/duration
	SK_JEDI_PULL,//hold/duration
	SK_JEDI_TELEPATHY,//instant
	SK_JEDI_GRIP,//hold/duration
	SK_JEDI_LIGHTNING,//hold/duration
	SK_JEDI_RAGE,//duration
	SK_JEDI_PROTECT,
	SK_JEDI_ABSORB,
	SK_JEDI_TEAM_HEAL,
	SK_JEDI_TEAM_FORCE,
	SK_JEDI_DRAIN,
	SK_JEDI_SEE,
	SK_JEDI_SABER_OFFENSE,
	SK_JEDI_SABER_DEFENSE,
	SK_JEDI_SABERTHROW,
	//iomatix, passiv skills:
	SK_JEDI_hp_maxs,
	SK_JEDI_mp_maxs,
	SK_JEDI_overload,
	SK_JEDI_thousandcuts,
	///////////////////
	SK_JEDI_NUM_SKILLS
};
//iomatix automata:
#define STD_FORCEPOWER_GET(name, fp) int Lmd_Prof_Jedi_GetSkill_##name(AccountPtr_t accPtr, profSkill_t *skill) { \
											Account_t *acc = (Account_t*)accPtr; \
											jediFields_t *data; \
											if(!IS_A_JEDI(acc)) return 0; \
											data = ACCFIELDDATA(acc); \
											if(!data) return 0; \
											return data->Jedi[fp]; \
										} 

#define STD_FORCEPOWER_CANSET(name, fp) qboolean Lmd_Prof_Jedi_CanSetSkill_##name(AccountPtr_t accPtr, profSkill_t *skill, int value) { \
											Account_t *acc = (Account_t*)accPtr; \
											jediFields_t *data; \
											int side; \
											if(!IS_A_JEDI(acc)) return qfalse; \
											data = ACCFIELDDATA(acc); \
											if(!data) return qfalse; \
											if(value < skill->levels.min || value > skill->levels.max) return qfalse; \
											return qtrue; \
										}

#define STD_FORCEPOWER_SET(name, fp) qboolean Lmd_Prof_Jedi_SetSkill_##name(AccountPtr_t accPtr, profSkill_t *skill, int value) { \
										Account_t *acc = (Account_t*)accPtr; \
										jediFields_t *data; \
										if(!Lmd_Prof_Jedi_CanSetSkill_##name(acc, skill, value)) return qfalse; \
										data = ACCFIELDDATA(acc); \
										data->Jedi[fp] = value; \
										Lmd_Accounts_Modify(acc); \
										return qtrue; \
									}

#define STD_FORCEPOWER_FUNCS(name, fp) STD_FORCEPOWER_GET(name, fp) \
	STD_FORCEPOWER_CANSET(name, fp) \
	STD_FORCEPOWER_SET(name, fp)



typedef struct jediFields_s {
	int Jedi[SK_JEDI_NUM_SKILLS];
}jediFields_t;
#define	JEDIFIELDOFS(x) ((int)&(((jediFields_t *)0)->x))

#define IS_A_JEDI(acc) (Accounts_Prof_GetProfession(acc) == PROF_JEDI)

#define FIELDDATA(ent) (jediFields_t*)Accounts_Prof_GetFieldData(ent->client->pers.Lmd.account)
#define ACCFIELDDATA(acc) (jediFields_t*)Accounts_Prof_GetFieldData(acc)

#define JediFields_Base(_m) \
	_m##_AUTO(heal, JEDIFIELDOFS(Jedi[SK_JEDI_HEAL]), F_INT) \
	_m##_AUTO(levitate, JEDIFIELDOFS(Jedi[SK_JEDI_LEVITATION]), F_INT) \
	_m##_AUTO(speed, JEDIFIELDOFS(Jedi[SK_JEDI_SPEED]), F_INT) \
	_m##_AUTO(push, JEDIFIELDOFS(Jedi[SK_JEDI_PUSH]), F_INT) \
	_m##_AUTO(pull, JEDIFIELDOFS(Jedi[SK_JEDI_PULL]), F_INT) \
	_m##_AUTO(telepathy, JEDIFIELDOFS(Jedi[SK_JEDI_TELEPATHY]), F_INT) \
	_m##_AUTO(grip, JEDIFIELDOFS(Jedi[SK_JEDI_GRIP]), F_INT) \
	_m##_AUTO(lightning, JEDIFIELDOFS(Jedi[SK_JEDI_LIGHTNING]), F_INT) \
	_m##_AUTO(rage, JEDIFIELDOFS(Jedi[SK_JEDI_RAGE]), F_INT) \
	_m##_AUTO(protect, JEDIFIELDOFS(Jedi[SK_JEDI_PROTECT]), F_INT) \
	_m##_AUTO(absorb, JEDIFIELDOFS(Jedi[SK_JEDI_ABSORB]), F_INT) \
	_m##_AUTO(team_heal, JEDIFIELDOFS(Jedi[SK_JEDI_TEAM_HEAL]), F_INT) \
	_m##_AUTO(team_force, JEDIFIELDOFS(Jedi[SK_JEDI_TEAM_FORCE]), F_INT) \
	_m##_AUTO(drain, JEDIFIELDOFS(Jedi[SK_JEDI_DRAIN]), F_INT) \
	_m##_AUTO(see, JEDIFIELDOFS(Jedi[SK_JEDI_SEE]), F_INT) \
	_m##_AUTO(saber_offense, JEDIFIELDOFS(Jedi[SK_JEDI_SABER_OFFENSE]), F_INT) \
	_m##_AUTO(saber_defense, JEDIFIELDOFS(Jedi[SK_JEDI_SABER_DEFENSE]), F_INT) \
	_m##_AUTO(saber_throw, JEDIFIELDOFS(Jedi[SK_JEDI_SABERTHROW]), F_INT) \
    _m##_AUTO(hp_maxs, JEDIFIELDOFS(Jedi[SK_JEDI_hp_maxs]), F_INT) \
    _m##_AUTO(mp_maxs, JEDIFIELDOFS(Jedi[SK_JEDI_mp_maxs]), F_INT) \
    _m##_AUTO(overload, JEDIFIELDOFS(Jedi[SK_JEDI_overload]), F_INT) \
	_m##_AUTO(thousandcuts, JEDIFIELDOFS(Jedi[SK_JEDI_thousandcuts]), F_INT)

JediFields_Base(DEFINE_FIELD_PRE)

DATAFIELDS_BEGIN(JediFields)
JediFields_Base(DEFINE_FIELD_LIST)
DATAFIELDS_END

const int JediFields_Count = DATAFIELDS_COUNT(JediFields);


extern int forcePowerDarkLight[NUM_FORCE_POWERS];
//Ufo: redesigned so that having skills of both force sides appears as no side rather than jedi side due to heal skill having 0 index
int Jedi_GetAccSide(Account_t *acc) {
	jediFields_t *entry = (jediFields_t*)Accounts_Prof_GetFieldData(acc);
	int result = 0;
	int i;
	for (i = 0; i < SK_JEDI_NUM_SKILLS; i++) {
		if (entry->Jedi[i] > 0 && forcePowerDarkLight[i] != 0)
			result |= forcePowerDarkLight[i];
	}
	if (result == FORCE_LIGHTSIDE || result == FORCE_DARKSIDE)
		return result;
	else
		return 0;
}

int Jedi_GetSide(gentity_t *ent) {
	if (!ent->client->pers.Lmd.account)
		return 0;
	return Jedi_GetAccSide(ent->client->pers.Lmd.account);
}
/////iomatix: passive skills

//skills:
//will
const char *jediSkill_Passive_hp_maxs_Descr[] = {
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+10 HP",
	"+20 HP",
	NULL
};


STD_FORCEPOWER_FUNCS(hp_maxs, FP_PASSIVE_hp_maxs)

profSkill_t jediSkill_Passive_hp_maxs = {
	"Will",
	"Increase total Health capacity.",
	jediSkill_Passive_hp_maxs_Descr,

	0,
	SkillLevels_20,
	SkillPoints_Linear_10,

	Lmd_Prof_Jedi_GetSkill_hp_maxs,
	Lmd_Prof_Jedi_CanSetSkill_hp_maxs,
	Lmd_Prof_Jedi_SetSkill_hp_maxs
};
//Force
const char *jediSkill_Passive_mp_maxs_Descr[] = {
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+12 Force capacity",
	"+25 Force capacity",
	NULL
};


STD_FORCEPOWER_FUNCS(mp_maxs, FP_PASSIVE_mp_maxs)

profSkill_t jediSkill_Passive_mp_maxs = {
	"Force",
	"Increase the Force capacity.",
	jediSkill_Passive_mp_maxs_Descr,

	0,
	SkillLevels_20,
	SkillPoints_Linear_10,

	Lmd_Prof_Jedi_GetSkill_mp_maxs,
	Lmd_Prof_Jedi_CanSetSkill_mp_maxs,
	Lmd_Prof_Jedi_SetSkill_mp_maxs
};


//Overload
const char *jediSkill_Passive_overload_Descr[] = {
	"Additional 2 percent of force regeneration and force capacity",
	"Additional 5 percent of force regeneration and force capacity",
	"Additional 8 percent of force regeneration and force capacity",
	"Additional 10 percent of force regeneration and force capacity",
	NULL
};


STD_FORCEPOWER_FUNCS(overload, FP_PASSIVE_overload)

profSkill_t jediSkill_Passive_overload = {
	"Overload",
	"Regenerate more force and increase its base capacity.",
	jediSkill_Passive_overload_Descr,

	0,
	SkillLevels_8,
	SkillPoints_Linear_12,

	Lmd_Prof_Jedi_GetSkill_overload,
	Lmd_Prof_Jedi_CanSetSkill_overload,
	Lmd_Prof_Jedi_SetSkill_overload
};

const char *jediSkill_Passive_thousandcuts_Descr[] = {
	"5 percent of the additional saber damage.",
	"10 percent of the additional saber damage.",
	"15 percent of the additional saber damage.",
	"20 percent of the additional saber damage.",
	"5 percent of the base saber damage is converted to the lethality damage.",
	"10 percent of the base saber damage is converted to the lethality damage.",
	"15 percent of the base saber damage is converted to the lethality damage.",
	"20 percent of the base saber damage is converted to the lethality damage.",
	NULL
};

STD_FORCEPOWER_FUNCS(thousandcuts, FP_PASSIVE_thousandcuts)

profSkill_t jediSkill_Passive_thousandcuts = {
	"The Thousand Cuts",
	"Your saber attacks will become more deadly.",
	jediSkill_Passive_thousandcuts_Descr,

	0,
	SkillLevels_4,
	SkillPoints_Linear_12,

	Lmd_Prof_Jedi_GetSkill_thousandcuts,
	Lmd_Prof_Jedi_CanSetSkill_thousandcuts,
	Lmd_Prof_Jedi_SetSkill_thousandcuts,
};


profSkill_t jediSkill_Passive_Subskills[] = {
	jediSkill_Passive_hp_maxs,
	jediSkill_Passive_mp_maxs,
	jediSkill_Passive_overload,
	jediSkill_Passive_thousandcuts
};
const unsigned int jediPassiveCount = sizeof(jediSkill_Passive_Subskills) / sizeof(profSkill_t);
////////////////
////functions:
//functions for passive skills:
//GetSkill
int Lmd_Prof_Jedi_GetThousandCutsSkill(Account_t *acc) {
	if (!acc) {
		return 0;
	}
	return jediSkill_Passive_thousandcuts.getValue(acc, &jediSkill_Passive_thousandcuts);
}
//FUNCTIONS
int Get_Jedi_hp_maxs_value(gentity_t *ent) //Will (Health points)
{
	if (lmd_jedi_add_hp_level.integer == 0) { return 100; } //old lugor option

	int value = 40; //starting health
	
		
	value += floor(PlayerAcc_Prof_GetLevel(ent) * 1.7); //scale health
	

	int skillHP = Lmd_Prof_Jedi_GetSkill_hp_maxs(ent->client->pers.Lmd.account, &jediSkill_Passive_hp_maxs);
	if (skillHP > 0) {

		value += 10 * skillHP;
		if (skillHP == jediSkill_Passive_hp_maxs.levels.max) value += 10; //bonus for maxed (10+10=20)
	}

	return value;

}


//Overload + Force

int Get_Jedi_overload_value_cap(gentity_t *ent, int Amount) //returns % of total mana. Amount is based on overload skill level.
{

	if (!Lmd_Prof_Jedi_GetSkill_overload(ent->client->pers.Lmd.account, &jediSkill_Passive_overload)) return 0;

	int value = 0;
	int skillOverload = Lmd_Prof_Jedi_GetSkill_overload(ent->client->pers.Lmd.account, &jediSkill_Passive_overload);
	//2,5,8,10 %
	if (skillOverload == 1) value = Amount / 50; //2%
	else if (skillOverload == 2) value = Amount / 20; //5%
	else if (skillOverload == 3) value = Amount / 12; //+-8%
	else if (skillOverload == 4) value = Amount / 10; //10%
	return value;

}
int Get_Jedi_mp_maxs_value(gentity_t *ent)
{
	int value = 55; //starting force
	value += floor(PlayerAcc_Prof_GetLevel(ent) * 2.1); //Level bonus
	if (Lmd_Prof_Jedi_GetSkill_overload(ent->client->pers.Lmd.account, &jediSkill_Passive_overload)) value += Get_Jedi_overload_value_cap(ent, value); //be sure it adds only base Force Power //iomatix

																																					   //Passive skill bonus:
	int skillMP = Lmd_Prof_Jedi_GetSkill_mp_maxs(ent->client->pers.Lmd.account, &jediSkill_Passive_mp_maxs);
	if (skillMP > 0)
	{
		value += 10 * skillMP;
		if (skillMP == jediSkill_Passive_mp_maxs.levels.max) value += 13; //bonus for maxed (12+13=25)
	}

	return value;

}

int Get_Jedi_overload_value_reg(gentity_t *ent, int value) //returns % output for the skill
{
	if (!Lmd_Prof_Jedi_GetSkill_overload(ent->client->pers.Lmd.account, &jediSkill_Passive_overload)) return 0;

	int skillOverload = Lmd_Prof_Jedi_GetSkill_overload(ent->client->pers.Lmd.account, &jediSkill_Passive_overload);
	//2,5,8,10 %
	if (skillOverload == 1) value /= 50; //2%
	else if (skillOverload == 2) value /= Get_Jedi_mp_maxs_value(ent) / 20; //5%
	else if (skillOverload == 3) value /= Get_Jedi_mp_maxs_value(ent) / 12; //+-8%
	else if (skillOverload == 4) value /= Get_Jedi_mp_maxs_value(ent) / 10; //10%
	else value = 0;

	return value;

}


/////

////////////////////

const char *jediSkill_Neutral_Jump_Descr[] = {
	"Gain the force jump power.",
	"Jump higher.",
	"Jump higher.",
	"Jump higher.",
	"Jump higher.  Jump in the air.",
	NULL
};

STD_FORCEPOWER_FUNCS(Jump, FP_LEVITATION)

profSkill_t jediSkill_Neutral_Jump = {
	"Jump",
	"Use the force to reach higher places.",
	jediSkill_Neutral_Jump_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_Jump,
	Lmd_Prof_Jedi_CanSetSkill_Jump,
	Lmd_Prof_Jedi_SetSkill_Jump
};

const char *jediSkill_Neutral_Push_Descr[] = {
	"Gain the Push force power.  Push those that you target.",
	"Push anything within a small arc your aim.",
	"Push anything within a large arc of your aim.  Lower cooldown time.",
	"Push with more force than normal.  Lower cooldown time.",
	"Push with twice the normal force.  Lower cooldown time.",
	"",
	NULL
};

STD_FORCEPOWER_FUNCS(Push, FP_PUSH)

profSkill_t jediSkill_Neutral_Push = {
	"Push",
	"Push items and people away from you.  Deflect projectiles.  Break saber locks in your favor.",
	jediSkill_Neutral_Push_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_Push,
	Lmd_Prof_Jedi_CanSetSkill_Push,
	Lmd_Prof_Jedi_SetSkill_Push
};

const char *jediSkill_Neutral_Pull_Descr[] = {
	"Gain the Pull force power.  Push those that you target.",
	"Pull anything within a small arc your aim.",
	"Pull anything within a large arc of your aim.",
	"Pull with more force than normal.",
	"Pull with twice the normal force." ,
	NULL
};

STD_FORCEPOWER_FUNCS(Pull, FP_PULL)

profSkill_t jediSkill_Neutral_Pull = {
	"Pull",
	"Pull items, people, and projectiles to you.",
	jediSkill_Neutral_Pull_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_Pull,
	Lmd_Prof_Jedi_CanSetSkill_Pull,
	Lmd_Prof_Jedi_SetSkill_Pull
};

const char *jediSkill_Neutral_Speed_Descr[] = {
	"Gain the Speed force power.  Lasts 10 seconds.",
	"Increase the Speed duration to 15 seconds.",
	"Increase the Speed duration to 20 seconds.",
	"Increase the Speed duration to 25 seconds.",
	"Increase the Speed duration to 35 seconds.",
	NULL
};

STD_FORCEPOWER_FUNCS(Speed, FP_SPEED)

profSkill_t jediSkill_Neutral_Speed = {
	"Speed",
	"Gain a burst of speed.",
	jediSkill_Neutral_Speed_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_Speed,
	Lmd_Prof_Jedi_CanSetSkill_Speed,
	Lmd_Prof_Jedi_SetSkill_Speed
};


const char *jediSkill_Neutral_Seeing_Descr[] = {
	"Gain the Seeing force power.  Lasts 10 seconds",
	"Increase the Seeing duration to 20 seconds.",
	"Increase the Seeing duration to 30 seconds.  Sense the money stash with a blue beam.",
	"Increase the Seeing duration to 40 seconds.",
	"Increase the Seeing duration to 60 seconds.",
	NULL
};

STD_FORCEPOWER_FUNCS(Seeing, FP_SEE)

profSkill_t jediSkill_Neutral_Seeing = {
	"Seeing",
	"Locate others through barriers, and find the money stash.  Sense others using Mind Trick",
	jediSkill_Neutral_Seeing_Descr,

	0,
	//Ufo:
{ 0, 3 },
SkillPoints_Default,

Lmd_Prof_Jedi_GetSkill_Seeing,
Lmd_Prof_Jedi_CanSetSkill_Seeing,
Lmd_Prof_Jedi_SetSkill_Seeing
};

profSkill_t jediSkill_Neutral_Subskills[] = {
	jediSkill_Neutral_Jump,
	jediSkill_Neutral_Push,
	jediSkill_Neutral_Pull,
	jediSkill_Neutral_Speed,
	jediSkill_Neutral_Seeing
};
const unsigned int jediSkillNeutralCount = sizeof(jediSkill_Neutral_Subskills) / sizeof(profSkill_t);


const char *jediSkill_Light_Absorb_Descr[] = {
	"Gain the Absorb force power.  Become immune to the Lightning, Drain, Grip, Push, and Pull force powers.  Absorb 1/3 of the force energy.",
	"Absorb 2/3 of the force energy.",
	"Absorb all of the force energy.",
	"Absorb all of the force energy, and gain 1/3 more.",
	"Absorb all of the force energy, and gain 2/3 more.  Use Protect while absorbing.",
	NULL
};

STD_FORCEPOWER_FUNCS(Absorb, FP_ABSORB)


profSkill_t jediSkill_Light_Absorb = {
	"Absorb",
	"Absorb force powers used against you for 20 seconds.  Escape from the Grip force power.",
	jediSkill_Light_Absorb_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_Absorb,
	Lmd_Prof_Jedi_CanSetSkill_Absorb,
	Lmd_Prof_Jedi_SetSkill_Absorb
};


const char *jediSkill_Light_Heal_Descr[] = {
	"Gain the Heal force power.  Restore 5 health when used.",
	"Restore 10 health when used.",
	"Restore 25 health when used.",
	"Restore 35 health when used.",
	"Restore 50 health when used.",
	NULL
};

STD_FORCEPOWER_FUNCS(Heal, FP_HEAL)

profSkill_t jediSkill_Light_Heal = {
	"Heal",
	"Heal yourself.",
	jediSkill_Light_Heal_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_Heal,
	Lmd_Prof_Jedi_CanSetSkill_Heal,
	Lmd_Prof_Jedi_SetSkill_Heal
};


const char *jediSkill_Light_Protect_Descr[] = {
	"Gain the Protect force power.  Protect against 4/10 of the damage at 1 force point per damage point.",
	"Protect against 6/10 of the damage at 1 force point per 2 damage points.",
	"Protect against 8/10 of the damage at 1 force point per 4 damage points.",
	"Protect against 8.5/10 of the damage at 1 force point per 5 damage points.",
	"Protect against 9/10 of the damage at 1 force point per 7 damage points.",
	NULL
};

STD_FORCEPOWER_FUNCS(Protect, FP_PROTECT)

profSkill_t jediSkill_Light_Protect = {
	"Protect",
	"Protect yourself from damage.",
	jediSkill_Light_Protect_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_Protect,
	Lmd_Prof_Jedi_CanSetSkill_Protect,
	Lmd_Prof_Jedi_SetSkill_Protect
};

const char *jediSkill_Light_MindTrick_Descr[] = {
	"Gain the Mind Trick force power.  Become invisible to your target for 20 seconds.",
	"Become invisible to those in front of you for 25 seconds.",
	"Become invisible to those all around you for 30 seconds at double the usual distance.",
	"Invisibility time increased to 40 seconds at tripple the usual distance.",
	"Invisibility time increased to 50 seconds at four times the usual distance.",
	NULL
};

STD_FORCEPOWER_FUNCS(MindTrick, FP_TELEPATHY)

profSkill_t jediSkill_Light_MindTrick = {
	"MindTrick",
	"Trick others and become invisible to them.  You will become visible if you attack, or to anyone who uses Force Sense.",
	jediSkill_Light_MindTrick_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_MindTrick,
	Lmd_Prof_Jedi_CanSetSkill_MindTrick,
	Lmd_Prof_Jedi_SetSkill_MindTrick
};

const char *jediSkill_Light_TeamHeal_Descr[] = {
	"Make the skill more powerful!",
	"Make the skill more powerful!",
	"Make the skill more powerful!",
	"Make the skill more powerful!",
	"Make the skill more powerful!",
    NULL
};

STD_FORCEPOWER_FUNCS(TeamHeal, FP_TEAM_HEAL)

profSkill_t jediSkill_Light_TeamHeal = {
	"TeamHeal",
	"Heal others around you.  Control who you heal by adding them to \'/buddies\'.",
	jediSkill_Light_TeamHeal_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_TeamHeal,
	Lmd_Prof_Jedi_CanSetSkill_TeamHeal,
	Lmd_Prof_Jedi_SetSkill_TeamHeal
};

profSkill_t jediSkill_Light_Subskills[] = {
	jediSkill_Light_Absorb,
	jediSkill_Light_Heal,
	jediSkill_Light_Protect,
	jediSkill_Light_MindTrick,
	jediSkill_Light_TeamHeal
};
const unsigned int jediSkillLightCount = sizeof(jediSkill_Light_Subskills) / sizeof(profSkill_t);


const char *jediSkill_Dark_Grip_Descr[] = {
	"Gain the Grip force power.  Grip a target for 5 seconds.",
	"Slowly move the target around by aiming.  Do more damage when grip lasts for more than 2 seconds.",
	"Increases speed for moving the target.  Do twice as much damage when the grip lasts for more than 2 seconds.",
	"Grip others twice as far away than normal.  Mantain the grip when not looking at the player.",
	"Grip others four times farther than normal.",
	NULL
};

STD_FORCEPOWER_FUNCS(Grip, FP_GRIP)

profSkill_t jediSkill_Dark_Grip = {
	"Grip",
	"Paralyze a target and do more damage the longer they are held.",
	jediSkill_Dark_Grip_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_Grip,
	Lmd_Prof_Jedi_CanSetSkill_Grip,
	Lmd_Prof_Jedi_SetSkill_Grip
};


const char *jediSkill_Dark_Drain_Descr[] = {
	"Make the skill more powerful!",
	"Make the skill more powerful!",
	"Make the skill more powerful!",
	"Make the skill more powerful!",
	"Make the skill more powerful!",
	NULL
};

STD_FORCEPOWER_FUNCS(Drain, FP_DRAIN)

profSkill_t jediSkill_Dark_Drain = {
	"Drain",
	"Take force energy from others to heal yourself.",
	jediSkill_Dark_Drain_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_Drain,
	Lmd_Prof_Jedi_CanSetSkill_Drain,
	Lmd_Prof_Jedi_SetSkill_Drain
};

const char *jediSkill_Dark_Lightnig_Descr[] = {
	"Gain the Lightning force power.  Shoot a burst of electricity for half a second directly at the target.",
	"Shoot lightning continuously.  Take half as long to use the power again after stopping.",
	"Shoot lightning in a wide arc.",
	"Shoot lightning 1.5 times farther than usual.",
	"Shoot lightning twice as far as usual.",
	NULL
};

STD_FORCEPOWER_FUNCS(Lightning, FP_LIGHTNING)

profSkill_t jediSkill_Dark_Lightning = {
	"Lightning",
	"Shocks others with a damaging lightning burst.",
	jediSkill_Dark_Lightnig_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_Lightning,
	Lmd_Prof_Jedi_CanSetSkill_Lightning,
	Lmd_Prof_Jedi_SetSkill_Lightning
};

const char *jediSkill_Dark_Rage_Descr[] = {
	"Gain the Rage force power.  Do 1/5 more saber damage while active.  Health decreases at 40 points every 3 seconds.",
	"Do 2/5 more saber damage while active.    Health decreases at 20 points every 3 seconds.",
	"Do 3/5 more saber damage while active.  Health decreases at 40 points every 9 seconds.",
	"Do 4/5 more saber damage while active.  Health decreases at 10 points every 3 seconds.",
	"Do twice as much saber damage while active.  Health decreases at 4 points every 3 seconds.",
	NULL
};

STD_FORCEPOWER_FUNCS(Rage, FP_RAGE)

profSkill_t jediSkill_Dark_Rage = {
	"Rage",
	"Increase your speed damage and defense with the power of the force at the cost of degenerating health.  You need to recover for a short time afterwards.\n"
	"While using rage, you can resist death even when you have very low health.",
	jediSkill_Dark_Rage_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_Rage,
	Lmd_Prof_Jedi_CanSetSkill_Rage,
	Lmd_Prof_Jedi_SetSkill_Rage
};

const char *jediSkill_Dark_Energize_Descr[] = {
	"Make the skill more powerful!",
	"Make the skill more powerful!",
	"Make the skill more powerful!",
	"Make the skill more powerful!",
	"Make the skill more powerful!",
	NULL
};

STD_FORCEPOWER_FUNCS(Energize, FP_TEAM_FORCE)

profSkill_t jediSkill_Dark_Energize = {
	"Energize",
	"Recharge force energy of those around you.  Control who you recharge by adding them to \'/buddies\'.",
	jediSkill_Dark_Energize_Descr,

	0,
	SkillLevels_Default,
	SkillPoints_Default,

	Lmd_Prof_Jedi_GetSkill_Energize,
	Lmd_Prof_Jedi_CanSetSkill_Energize,
	Lmd_Prof_Jedi_SetSkill_Energize
};

profSkill_t jediSkill_Dark_Subskills[] = {
	jediSkill_Dark_Grip,
	jediSkill_Dark_Drain,
	jediSkill_Dark_Lightning,
	jediSkill_Dark_Rage,
	jediSkill_Dark_Energize
};
const unsigned int jediSkillDarkCount = sizeof(jediSkill_Dark_Subskills) / sizeof(profSkill_t);

const char *jediSkill_Saber_Attack_Descr[] = {
	"Train your offensive saber skills.",
	"Train your offensive saber skills.",
	"Train your offensive saber skills.",
	"Train your offensive saber skills.",
	NULL
};

STD_FORCEPOWER_FUNCS(Saber_Attack, FP_SABER_OFFENSE)

profSkill_t jediSkill_Saber_Attack = {
	"Attack",
	"Your skills with a saber.",
	jediSkill_Saber_Attack_Descr,

	0,
{ 1, 3 },
SkillPoints_Default,

Lmd_Prof_Jedi_GetSkill_Saber_Attack,
Lmd_Prof_Jedi_CanSetSkill_Saber_Attack,
Lmd_Prof_Jedi_SetSkill_Saber_Attack
};


const char *jediSkill_Saber_Defend_Descr[] = {
	"Train your deffensive saber skills.",
	"Train your deffensive saber skills.",
	"Train your deffensive saber skills.",
	"Train your deffensive saber skills.",
	NULL
};

STD_FORCEPOWER_FUNCS(Saber_Defend, FP_SABER_DEFENSE)

profSkill_t jediSkill_Saber_Defend = {
	"Defend",
	"Your ability to defend yourself and deflect projectiles with a saber.",
	jediSkill_Saber_Defend_Descr,

	0,
{ 1, 3 },
SkillPoints_Default,

Lmd_Prof_Jedi_GetSkill_Saber_Defend,
Lmd_Prof_Jedi_CanSetSkill_Saber_Defend,
Lmd_Prof_Jedi_SetSkill_Saber_Defend
};


const char *jediSkill_Saber_Throw_Descr[] = {
	"Train your throws.",
	"Train your throws.",
	"Train your throws.",
	"Train your throws.",
	NULL
};

STD_FORCEPOWER_FUNCS(Saber_Throw, FP_SABERTHROW)

profSkill_t jediSkill_Saber_Throw = {
	"Throw",
	"Your ability to throw the saber as a projectile.",
	jediSkill_Saber_Throw_Descr,

	0,
{ 0, 3 },
SkillPoints_Default,

Lmd_Prof_Jedi_GetSkill_Saber_Throw,
Lmd_Prof_Jedi_CanSetSkill_Saber_Throw,
Lmd_Prof_Jedi_SetSkill_Saber_Throw
};

profSkill_t jediSkill_Saber_Subskills[] = {
	jediSkill_Saber_Attack,
	jediSkill_Saber_Defend,
	jediSkill_Saber_Throw
};
const unsigned int jediSkillaberCount = sizeof(jediSkill_Saber_Subskills) / sizeof(profSkill_t);

const char *jediSkill_Neutral_Descr[] = {
	NULL
};
profSkill_t jediSkill_Neutral = {
	"Neutral",
	"Neutral force powers.",
	jediSkill_Neutral_Descr,

	0,

{ 0,0 },
SkillPoints_Default,

NULL,
NULL,
NULL,

{
	jediSkillNeutralCount,
	(profSkill_t *)jediSkill_Neutral_Subskills
},
};

const char *jediSkill_Jedi_Descr[] = {
	NULL
};

profSkill_t jediSkill_Jedi = {
	"Jedi",
	"Powers of the Jedi.",
	jediSkill_Jedi_Descr,

	1,
{ 0,0 },
SkillPoints_Default,

NULL,
NULL,
NULL,

{
	jediSkillLightCount,
	(profSkill_t *)jediSkill_Light_Subskills
},
};

const char *jediSkill_Sith_Descr[] = {
	NULL
};

profSkill_t jediSkill_Sith = {
	"Sith",
	"Powers of the Sith.",
	jediSkill_Sith_Descr,

	1,
{ 0,0 },
SkillPoints_Default,

NULL,
NULL,
NULL,

{
	jediSkillDarkCount, (profSkill_t *)jediSkill_Dark_Subskills
},
};

const char *jediSkill_Saber_Descr[] = {
	NULL
};

profSkill_t jediSkill_Saber = {
	"Saber",
	"Skills with a saber.",
	jediSkill_Saber_Descr,

	0,
{ 0,0 },
SkillPoints_Default,

NULL,
NULL,
NULL,

{
	jediSkillaberCount,
    (profSkill_t *)jediSkill_Saber_Subskills
}
};


//iomatix: passive branch
const char *jediSkill_Passive_Descr[] = {
	NULL
};

profSkill_t jediSkill_Passive = {
	"Passive",
	"List of the available passive skills:",
	 jediSkill_Passive_Descr,

	0,
{ 0,0 },
SkillPoints_Default,

NULL,
NULL,
NULL,
{ jediPassiveCount,(profSkill_t *) jediSkill_Passive_Subskills }
};

///
profSkill_t jediSkill[] = {
	jediSkill_Neutral,
	jediSkill_Jedi,
	jediSkill_Sith,
	jediSkill_Saber,
	jediSkill_Passive
};
const unsigned int jediSkillCount = sizeof(jediSkill) / sizeof(profSkill_t);



extern vmCvar_t g_forceRegenTime;
int Jedi_GetForceRegenDebounce(gentity_t *ent) {
	int level = PlayerAcc_Prof_GetLevel(ent);
	//if(level < 20)
	//	return g_forceRegenTime.integer;
	//level -= 20;
	//level / 20 = x / (g_forceRegenTime.integer * (2. / 3.))
	//(level * (g_forceRegenTime.integer * (2. / 3.))) = x * 20
	//((level * (g_forceRegenTime.integer * (2. / 3.))) / 20) = x
	//iomatix new regen formula
	return g_forceRegenTime.integer - floor((level * (g_forceRegenTime.integer / 8.)) / 120.); //120 is max level -> -13% regen cooldown on level 120
}

unsigned int Jedi_Count() {
	int i, count = 0;
	for (i = 0; i < MAX_CLIENTS; i++) {
		if (!g_entities[i].client || g_entities[i].client->pers.connected != CON_CONNECTED)
			continue;
		if (PlayerAcc_Prof_GetProfession((&g_entities[i])) == PROF_JEDI)
			count++;
	}
	return count;
}

//Ufo:
void Cmd_Ionlysaber_f(gentity_t *ent, int iArg)
{
	if (ent->s.m_iVehicleNum) {
		return;
	}

	if (ent->client->ps.weaponTime > 0
		|| ent->client->ps.weapon != WP_SABER
		|| ent->client->ps.m_iVehicleNum
		|| ent->client->pers.Lmd.persistantFlags & SPF_IONLYSABER
		|| duelInProgress(&ent->client->ps)
		|| ent->client->ps.forceHandExtend != HANDEXTEND_NONE
		|| BG_HasYsalamiri(g_gametype.integer, &ent->client->ps)) {
		return;
	}
	if (ent->client->Lmd.moneyStash) {
		return;
	}

	int i = 0;
	while (i < NUM_FORCE_POWERS)
	{
		if (ent->client->ps.fd.forcePowersActive & (1 << i))
		{
			WP_ForcePowerStop(ent, i);
		}
		i++;
	}

	ent->client->ps.trueNonJedi = qfalse;
	ent->client->ps.trueJedi = qtrue;
	ent->client->ps.weapon = WP_SABER;
	ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_SABER);
	ent->client->pers.Lmd.persistantFlags |= SPF_IONLYSABER;
	Disp(ent, "IONLYSABER activated (it can be deactivated by engaging in a duel or using /kill command)\n");
}

void Jedi_Spawn(gentity_t *ent) {
	ent->client->ps.trueNonJedi = qfalse;
	//iomatix:
	if (lmd_jedi_pickup_weapons.integer > 0) {
		ent->client->ps.trueJedi = qfalse;
	}
	else
	{
		ent->client->ps.trueJedi = qtrue;
		//not sure is it bug-free way but seems to be the only one as far as I looked for something more related with this for about 8 hours... ._.
	}
	//make sure they only use the saber
	ent->client->ps.weapon = WP_SABER;
	ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_SABER);

	//iomatix:
	ent->client->ps.fd.forcePowerMax =  Get_Jedi_mp_maxs_value(ent); 
	ent->client->ps.fd.forcePower = ent->client->ps.fd.forcePowerMax; //starting value
	ent->client->pers.maxHealth = ent->client->ps.stats[STAT_MAX_HEALTH] = Get_Jedi_hp_maxs_value(ent); //default formula
		


}

void Jedi_SetupForce(gentity_t *ent)
{
	int i;
	if (!ent->client->pers.Lmd.account)
		return;
	jediFields_t *entry = FIELDDATA(ent);

	for (i = 0; i < NUM_FORCE_POWERS; i++) {
		ent->client->ps.fd.forcePowerLevel[i] = entry->Jedi[i];
		if (ent->client->ps.fd.forcePowerLevel[i])
			ent->client->ps.fd.forcePowersKnown |= (1 << i);
		else
			ent->client->ps.fd.forcePowersKnown &= ~(1 << i);
	}
	ent->client->ps.fd.forceSide = Jedi_GetSide(ent);
}

const char *jediProf_Descr[] = {
	NULL
};

int Lmd_Prof_Jedi_GetLevel(Account_t *acc, profSkill_t *skill) {
	if (!IS_A_JEDI(acc)) {
		return 0;
	}
	return Accounts_Prof_GetLevel(acc);
}

qboolean Lmd_Prof_Jedi_CanSetLevel(Account_t *acc, profSkill_t *skill, int level) {
	if (!IS_A_JEDI(acc)) {
		return qfalse;
	}

	if (level < skill->levels.min || level > skill->levels.max) {
		return qfalse;
	}

	return qtrue;
}

qboolean Lmd_Prof_Jedi_SetLevel(Account_t *acc, profSkill_t *skill, int level) {
	if (!Lmd_Prof_Jedi_CanSetLevel(acc, skill, level)) {
		return qfalse;
	}

	Accounts_Prof_SetLevel(acc, level);
	return qtrue;
}

profession_t jediProf = {
	"Force User",
	"jedi",
{ JediFields, JediFields_Count, sizeof(jediFields_t) },
	{
		"ForceUser",
		"One who has learned the powers of the force.  A force user may specalize in light or dark powers, and excells at close quarter combat.\n"
		"Those proficient in the force increase its power as they increase in level.\n Force user is able to learn next specialization after reaching the Mastery Level. It is ^240 level of the current profession.",
	jediProf_Descr,

	1,
{ 1, 120 }, //LEVEL MAX!
{ SPT_NOCOST }, // The primary skill shouldn't factor into used skill points.

NULL,
NULL,
NULL,

{
	jediSkillCount,
	(profSkill_t *)jediSkill
}
	},
	Jedi_Spawn,
};

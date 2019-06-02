
#include "g_local.h"

#include "Lmd_Accounts_Core.h"
#include "Lmd_Accounts_Stats.h"
#include "Lmd_Commands_Auths.h"
#include "Lmd_Professions.h"
#include "Lmd_Checksum.h"
#include "Lmd_Time.h"
#include "Lmd_IPs.h"
#include "Lmd_Commands_Core.h"
#include "Lmd_Console.h"


Account_t *Accounts_New(char *username, char *name, char *password);

#define LEVEL_SCORE 45
#define MAX_LEVEL  120 // 40-> 120

void RenamePlayer(gentity_t *ent, char *Name) {
	char userinfo[MAX_INFO_STRING];
	int clientNum;
	if (!ent->client)
		return;
	if (!Name)
		return;
	if (strlen(Name) > MAX_NETNAME)
		return;
	clientNum = ent->s.number;
	trap_GetUserinfo(clientNum, userinfo, sizeof(userinfo));
	Info_SetValueForKey(userinfo, "name", Name);
	trap_SetUserinfo(clientNum, userinfo);
	ent->client->pers.netnameTime = 0;//
	ClientUserinfoChanged(clientNum);
}

void checkLevelUp(gentity_t *ent) {
	int myLevel = PlayerAcc_Prof_GetLevel(ent);
	int myTime = PlayerAcc_GetTime(ent);
	int levelUp = 0;
	if (myLevel && PlayerAcc_Prof_GetProfession(ent) != PROF_ADMIN) {
		qboolean lvlup = qfalse;
		while (myLevel * (myLevel+1) * LEVEL_SCORE <= PlayerAcc_GetExperience(ent) && myLevel * (myLevel + 1) * LEVEL_SCORE <= PlayerAcc_GetScore(ent) && myLevel * myLevel * 3600 * 2 <= myTime && myLevel < MAX_LEVEL) {
			levelUp++;
			myLevel++;
		}
		if (levelUp > 0) {
			int flags = PlayerAcc_GetFlags(ent);
			if (flags & ACCFLAGS_NOPROFCRLOSS) {
				PlayerAcc_AddFlags(ent, -ACCFLAGS_NOPROFCRLOSS);
				//Disp(ent, "^1Your free profession change has been canceled.");
				trap_SendServerCommand(ent->s.number, "chat \"^1Your free profession change has been canceled.\"");
			}
			//Disp(ent, va("You have gained %i level%s, you are now level %i.", levelUp, (levelUp>1) ? "s" : "", myLevel));
			trap_SendServerCommand(ent->s.number, va("chat \"^3You have gained ^5%i level%s, ^3you are now ^2level %i.\"", levelUp, (levelUp>1) ? "s" : "", myLevel));
			PlayerAcc_Prof_SetLevel(ent, myLevel);
			WP_InitForcePowers(ent);
		}
	}
}

void creditsByTime(gentity_t *ent, int playtime);
void updatePlayer(gentity_t *ent) {
	if (ent->client->pers.Lmd.account) {
		int prof = PlayerAcc_Prof_GetProfession(ent);
		if (prof == PROF_NONE || prof == PROF_BOT) checkLevelUp(ent);
		
	}

	//Ufo: admins often perform their tasks while spectating
	if (Auths_PlayerHasAdmin(ent) || (ent->client->sess.sessionTeam != TEAM_SPECTATOR && !(ent->client->pers.Lmd.jailTime > level.time))) {
		int playtime = (int)((level.time - ent->client->pers.Lmd.playTime) / 1000);
		// Add money by time
		if ((g_cmdDisable.integer & 256) && !Auths_PlayerHasAdmin(ent)) {
			creditsByTime(ent, playtime);
		}
		PlayerAcc_SetTime(ent, PlayerAcc_GetTime(ent) + playtime);
	}

	ent->client->pers.Lmd.playTime = level.time;
}


void Lmd_Accounts_LogAction(gentity_t *player, Account_t *acc, char* action) {
	char* username = Accounts_GetUsername(acc);
	G_LogPrintf("Player \"%s\" (%s) has %s.\n", player->client->pers.netname, username, action);
	Auths_DispAdmins(va(CT_B"Player \'"CT_N"%s"CT_B"\' ("CT_B_V"%s"CT_B") has %s.", player->client->pers.netname, username, action));
}


typedef struct hiscore_s {
	Account_t *acc;
	int val;
	//struct hiscore_s *next;
}hiscore_t;

void HiScore(gentity_t *ent, int field) {
	hiscore_t list[10];
	memset(list, 0, sizeof(list));
	int num_accounts = Accounts_Count();
	int i, i2;
	int valNew;
	int shiftIndex;
	Account_t *acc;
	for (i = 0; i<10; i++) {
		list[i].val = -Q3_INFINITE;
	}
	for (i = 0; i < num_accounts; i++) {
		acc = Accounts_Get(i);
		if (Accounts_Prof_GetProfession(acc) == PROF_ADMIN)
			continue;
		switch (field) {
		case 1:
			valNew = Accounts_GetTime(acc);
			break;
		case 2:
			valNew = Accounts_GetCredits(acc);
			break;
		case 3:
			valNew = Accounts_Prof_GetLevel(acc);
			break;
		case 4:
			valNew = Accounts_Stats_GetKills(acc);
			break;
		case 5:
			valNew = Accounts_Stats_GetStashes(acc);
			break;
		default:
			valNew = Accounts_GetScore(acc);
			break;
		}
		shiftIndex = -1;
		for (i2 = 0; i2 < 10; i2++) {
			if (list[i2].val < valNew) {
				shiftIndex = i2;
				break;
			}
		}
		if (shiftIndex < 0)
			continue;
		if (shiftIndex < 9) {
			//shift list array members from shiftIndex to 9 down.
			memcpy((list + shiftIndex + 1), list + shiftIndex, ((10 - shiftIndex) - 1) * sizeof(list[0]));
		}
		list[shiftIndex].acc = acc;
		list[shiftIndex].val = valNew;
	}

	char dstr[MAX_STRING_CHARS];
	char name[MAX_NAME_LENGTH];
	char *ptr;

	for (i = 0; i<10; i++) {
		if (!list[i].acc)
			break;
		ptr = Accounts_GetName(list[i].acc);
		if (!ptr)
			Q_strncpyz(name, "^3<^1ERROR^3>", sizeof(name));
		else
			Q_strncpyz(name, ptr, sizeof(name));
		Q_CleanStrC(name); //needed so credit amount lines up
		Q_strncpyz(dstr, va("^3%-32s", name), sizeof(dstr));
		switch (field) {
		case 1:
			Q_strcat(dstr, sizeof(dstr), va(" ^2%6i^3:^2%02i",
				Accounts_GetTime(list[i].acc) / 3600, (list[i].val / 60) % 60));
			break;
		default:
			Q_strcat(dstr, sizeof(dstr), va("  ^2%8i", list[i].val));
			break;
		}
		Disp(ent, dstr);
	}

}

//iomatix:
//Titles system 
char * GetTitle_system(gentity_t *ent)
{
	 int level = PlayerAcc_Prof_GetLevel(ent)+ PlayerAcc_GetNewGamePlus_count(ent); 
	 if (PlayerAcc_Prof_GetProfession(ent) == PROF_JEDI) {
		 //jedi titles
		 if (level < 10)return "Apprentice";
		 else if (level < 20)return "Disciple";
		 else if (level < 30)return "Adept";
		 else if (level < 40)
		 {
			 if(Jedi_GetSide(ent) == FORCE_DARKSIDE) return "Darth";
			 else if (Jedi_GetSide(ent) == FORCE_LIGHTSIDE) return "Knight";
			 else return "Nomad";

		 }
		 else if (level < 80)return "Lord";
		 else if (level < 100)return "Count";
		 else if (level < 130)return "Sage";
		 else if (level < 170)return "Master";
		 else if (level < 200)return "Luminary"; 
		 else return "Legend";
	 }
	 else if (PlayerAcc_Prof_GetProfession(ent) == PROF_MERC)
	 {//merc titles
		 if (level < 10)return "Freshmeat";
		 else if (level < 20)return "Rifleman";
		 else if (level < 30)return "Mercenary";
		 else if (level < 40)return "Specialist";
		 else if (level < 100)return "Bounty Hunter";
		 else if (level < 150)return "Assassin";
		 else if (level < 180)return "Commander";
		 else if (level < 230)return "Master of Hunt";
		 else return "Legend";


	 }
	 else return "Stranger";
	

}


extern int Professions_LevelCost_EXP(int prof, int playerLevel);
int Jedi_GetAccSide(Account_t *acc);
void GetStats(gentity_t *ent, Account_t *acc) {
	int prof, time, lvl, authrank;
	char *c;
	char *authlist;
	char *the_title = GetTitle_system(ent);
	char lastLogin[MAX_STRING_CHARS];

	char *secCode;

	if (!acc)
		return;

	prof = Accounts_Prof_GetProfession(acc);
	time = Accounts_GetTime(acc);
	lvl = Accounts_Prof_GetLevel(acc);
	authlist = Auths_QuickAuthList(acc);
	authrank = Auths_GetRank(acc);
	Time_ToHumanString(Accounts_GetLastLogin(acc), lastLogin, sizeof(lastLogin));

	secCode = Accounts_GetSeccode(acc);

	Disp(ent, va(
		"^8===== Account Information =====\n"
		"^3Id:            ^2%i\n"
		"^3Name:         ^8%s %s\n"
		"^3Username:      ^2%s\n"
		"^3Security code: ^2%s\n"
		"^3Time:          ^2%i^3:^2%02i\n"
		"%s",
		Accounts_GetId(acc),
		the_title,
		Accounts_GetName(acc),
		Accounts_GetUsername(acc),
		(secCode != NULL) ? secCode : "^1<none>",
		time / 3600, (time / 60) % 60, (Accounts_GetBountyReward(acc) > 0) ? va("^8Black List: ^1%i ^8CR", Accounts_GetBountyReward(acc)) : "^3You are not listed on ^8The Black List^3." ));
	  Disp(ent, va(
		"^8===== Progress Information =====\n"
		"^3Main Level:    ^2%i\n"
		"^3Credits:       ^2%i\n"
		"^3Experience:    ^5%i ^3/ ^2%i \n"
		"^3Force User Level:    ^2%i\n"
		"^3Mercenary Level:     ^2%i\n"
		"^3Credits Boxes:       ^2%i\n"
		"^3New Game Plus Level: ^2%i\n"
		"^3New Game Plus skill points: ^2%i\n"
		"^3Score:         ^2%i",
		lvl, Accounts_GetCredits(acc), Accounts_GetExperience(acc), Professions_LevelCost_EXP(PlayerAcc_Prof_GetProfession(ent),PlayerAcc_Prof_GetLevel(ent)), Accounts_GetLevel_jedi(acc),Accounts_GetLevel_merc(acc),Accounts_GetLootboxes(acc),Accounts_GetNewGamePlus_count(acc), Accounts_GetNewGamePlus_count(acc)*lmd_skillpoints_perlevel.integer, Accounts_GetScore(acc)));
	if (prof == PROF_NONE)
		c = "^2None";
	else if (prof == PROF_ADMIN)
		c = "^6God";
	else if (prof == PROF_BOT)
		c = "^1Bot";
	else if (prof == PROF_JEDI)
	{
		switch (Jedi_GetAccSide(acc))
		{
		case 0:
		default:
			//Ufo:
			c = "^5Force user";
			break;
		case 1:
			c = "^4Jedi";
			break;
		case 2:
			c = "^1Sith";
			break;
		}
	}
	else if (prof == PROF_MERC)
		c = "^3Merc";
	if (prof == PROF_NONE) Disp(ent, va("^3Next level up: ^2%i^3 points and ^2%i^3 hours", (lvl * (lvl+1) * LEVEL_SCORE), (lvl * lvl * 2)));
	Disp(ent, va(
		"^8===== Statistics =====\n"
		"^3Highest Streak: ^2x%i\n"
		"^3Logins:        ^2%i\n"
		"^3Last login:    ^2%s\n"
		"^3Kills:         ^2%i\n"
		"^3Deaths:        ^2%i\n"
		"^3Stashes:       ^2%i\n"
		"^3Total duels    ^2%i\n"
		"^3Duels won:     ^2%i\n"
		"^3Total shots:   ^2%i\n"
		"^3Shots hit:     ^2%i\n"
		"^3Profession:    ^2%s",
		Accounts_GetMaxKillstreak(acc),
		Accounts_GetLogins(acc),
		lastLogin,
		Accounts_Stats_GetKills(acc),
		Accounts_Stats_GetDeaths(acc),
		Accounts_Stats_GetStashes(acc), Accounts_Stats_GetDuels(acc), Accounts_Stats_GetDuelsWon(acc),
		Accounts_Stats_GetShots(acc), Accounts_Stats_GetHits(acc), c));
	if (authlist[0])
		Disp(ent, va(" ^1===== ADMIN =====\n^3Authfile(s):   ^2%s", authlist));
	if (authrank)
		Disp(ent, va("^3Authrank:      ^2%i", authrank));
	if (!ent) {
		IP_t ip;
		Accounts_GetLastIp(acc, ip);
		Disp(ent, va("^3Last IP:       ^2%s", Lmd_IPs_IPToString(ip)));
	}
}

void listAdmins(gentity_t *ent) {
	int i, count = 0;
	int max = Accounts_Count();
	Account_t *acc;
	for (i = 0; i < max; i++) {
		acc = Accounts_Get(i);
		if (Auths_AccHasAdmin(acc)) {
			Disp(ent, va("^3%-30s ^2%i %s", Accounts_GetUsername(acc), Auths_GetRank(acc), Auths_QuickAuthList(acc)));
			count++;
		}
	}
	Disp(ent, va(CT_V"%i"CT_B" nicks.", count));
}

void clearLevels(void) {
	int i, max = Accounts_Count();
	Account_t *acc;
	for (i = 0; i < max; i++) {
		acc = Accounts_Get(i);
		if (Auths_AccHasAdmin(acc))
			continue;
		Accounts_Prof_SetLevel(acc, 0);
	}
}

void clearCash(void) {
	int i, max = Accounts_Count();
	Account_t *acc;
	for (i = 0; i < max; i++) {
		acc = Accounts_Get(i);
		if (Auths_AccHasAdmin(acc))
			continue;
		Accounts_SetCredits(acc, 0);
	}
}

void clearScore(void) {
	int i, max = Accounts_Count();
	Account_t *acc;
	for (i = 0; i < max; i++) {
		acc = Accounts_Get(i);
		if (Auths_AccHasAdmin(acc))
			continue;
		Accounts_SetScore(acc, 0);
	}
}

void clearTime(void) {
	int i, max = Accounts_Count();
	Account_t *acc;
	for (i = 0; i < max; i++) {
		acc = Accounts_Get(i);
		if (Auths_AccHasAdmin(acc))
			continue;
		Accounts_SetTime(acc, 0);
	}
}

void clearAccounts(void) {
	int i, max = Accounts_Count();
	Account_t *acc;
	for (i = max - 1; i >= 0; i--) {
		acc = Accounts_Get(i);
		if (Auths_AccHasAdmin(acc))
			continue;
		Accounts_Delete(acc);
	}
}

void clearSkills(void) {
	int i, max = Accounts_Count();
	Account_t *acc;
	for (i = 0; i < max; i++) {
		acc = Accounts_Get(i);
		if (Auths_AccHasAdmin(acc))
			continue;
		Accounts_Prof_ClearData(acc);
	}
}

int recallDroppedCredits(gentity_t *ent);
void Inventory_Player_Logout(gentity_t *player);
void Lmd_Accounts_Player_Logout(gentity_t *ent) {
	Account_t *acc = ent->client->pers.Lmd.account;

	if (ent->client->pers.Lmd.jailTime > level.time)
		return;

	if (!acc) {
		if (Auths_PlayerHasTempAdmin(ent)) {
			Auths_RemoveTempAdmin(ent, NULL);
			//Disp(ent, "^3You have logged out of admin.");
			trap_SendServerCommand(ent->s.number, "chat \"^3You have logged out of admin.\"");
		}
		return;
	}

	recallDroppedCredits(ent);
	Inventory_Player_Logout(ent);
	updatePlayer(ent);

	ent->client->pers.Lmd.account = 0;
	ent->client->sess.Lmd.id = 0;

	Lmd_Accounts_LogAction(ent, acc, "logged out");

	RenamePlayer(ent, "Padawan");

	SetTeam(ent, "s");
	//Disp(ent, "^3You have logged out.");
	trap_SendServerCommand(ent->s.number, "chat \"^3You have ^2logged out^3.\"");
}

extern vmCvar_t lmd_startingcr;
int accountLiveTime(int level);
void Lmd_IPs_AddName(IP_t ip, char *name);
void Lmd_IPs_AddAccount(IP_t ip, int id);
void Inventory_Player_Login(gentity_t *player);
void PlayerGuide_Player_Login(gentity_t *ent);
qboolean Lmd_Accounts_Player_Login(gentity_t *ent, Account_t *acc) {
	char uinfo[MAX_INFO_STRING];
	int i;

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (g_entities[i].inuse && i != ent->s.number && g_entities[i].client->pers.Lmd.account == acc) {
			return qfalse;
		}
	}

	if (!Accounts_Prof_GetLevel(acc)) {
		Accounts_Prof_SetProfession(acc, PROF_NONE);
		Accounts_Prof_SetLevel(acc, 1);
		Accounts_SetScore(acc, 10);
		if (lmd_startingcr.integer > 0) {
			Accounts_SetCredits(acc, lmd_startingcr.integer);
			Accounts_AddFlags(acc, ACCFLAGS_NOPROFCRLOSS);
		}
	}



	Accounts_SetLogins(acc, Accounts_GetLogins(acc) + 1);

	ent->client->pers.Lmd.playTime = level.time;
	ent->client->pers.Lmd.account = acc;
	ent->client->sess.Lmd.id = Accounts_GetId(acc);

	Accounts_SetLastIp(acc, ent->client->sess.Lmd.ip);

	Accounts_SetLastLogin(acc, Time_Now());

	int prof = Accounts_Prof_GetProfession(acc);
	if (prof != PROF_NONE && prof != PROF_BOT) {
		ent->flags &= ~FL_GODMODE;
		if (ent->client->sess.sessionTeam != TEAM_SPECTATOR) {
			//ent->client->ps.persistant[PERS_SCORE]++;
			ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
			ClientSpawn(ent); //Ufo: why die if respawn is sufficient
							  //player_die(ent, ent, ent, 100000, MOD_SUICIDE);
		}
	}

	WP_InitForcePowers(ent);

	char *name = Accounts_GetName(acc);
	trap_GetUserinfo(ent->s.number, uinfo, sizeof(uinfo));
	Q_strncpyz(ent->client->pers.netname, name, sizeof(ent->client->pers.netname));
	Lmd_IPs_AddName(ent->client->sess.Lmd.ip, ent->client->pers.netname);
	Lmd_IPs_AddAccount(ent->client->sess.Lmd.ip, Accounts_GetId(acc));
	Info_SetValueForKey(uinfo, "name", name);
	trap_SetUserinfo(ent->s.number, uinfo);
	ClientUserinfoChanged(ent->s.number);

	Inventory_Player_Login(ent);
	PlayerGuide_Player_Login(ent);


	//Ufo: we don't want admins without proper flag to start with admin chatmode
	if (Auths_AccHasAdmin(acc) && Auths_AccHasAuthFlag(acc, AUTH_ADMINCHAT)) {
		ent->client->pers.Lmd.chatMode[1] = SAY_ADMINS;
	}


	Lmd_Accounts_LogAction(ent, acc, "logged in");

	return qtrue;
}

extern vmCvar_t lmd_loginsecurity;

void Lmd_Accounts_Player_CreateSecurityCode(gentity_t *ent, Account_t *acc) {
	char *code = Accounts_NewSeccode(acc);
	int flags = Accounts_GetFlags(acc);
	Disp(ent, va(
		"\n"
		"^3----IMPORTANT----\n"
		"^3Your new security code is:\n"
		"\n"
		"^2%s\n"
		"\n"
		"^3If you have security codes enabled, you will be asked for this code if you log in from a new ip.\n"
		"^3------------------\n"
		"\n",
		code));
	Disp(ent, va(
		"^3Your security code usage is currently %s^3.\n"
		"^3As Lugormod can not guarentee your password is safe, it is recommended you keep this on.\n"
		"^3You may enable or disable your security code by using the \'^2/seccode^3\' command.\n"
		"^3You may see your security code at any time by using the \'^2stats^3\' command.\n",
		(flags & ACCFLAGS_NOSECCODE) ? "^1disabled" : "^2enabled"));

	trap_SendServerCommand(ent->s.number, va("cp \""
		"^3Your security code is ^2%s^3\n"
		"^3This code is to ensure your account is not compromized if someone steals your password.\n"
		"^3More information has been written to the console."
		"\"", code));

	trap_SendServerCommand(ent->s.number, va("chat \"^3Your new security code is: ^2%s\"",code));
}

qboolean Lmd_Accounts_Player_TryLogin(gentity_t *ent, char *username, char *pass, char *secCode) {
	unsigned long int chk = Checksum(pass);
	int i;
	Account_t *acc;
	char *accSec;
	IP_t lastIP;
	int flags;

	if (!ent || !ent->client || !pass || duelInProgress(&ent->client->ps)) {
		//Disp(ent, "^3You cannot login at this time.");
		trap_SendServerCommand(ent->s.number, "chat \"^1You cannot login at this time.\"");
		return qfalse;
	}

	if (ent->client->pers.Lmd.account) {
		//Disp(ent, "^3You are currently logged in.");
		trap_SendServerCommand(ent->s.number, "chat \"^3You are currently logged in.\"");
		return qfalse;
	}

	acc = Accounts_GetByUsername(username);
	if (!acc) {
		//Disp(ent, "^1This username doesn't exist.");
		trap_SendServerCommand(ent->s.number, "chat \"^1This username doesn't exist.\"");
		return qfalse;
	}

	if (Accounts_GetPassword(acc) != chk) {
		//Disp(ent, "^1Invalid username or password.");
		trap_SendServerCommand(ent->s.number, "chat \"^1Invalid username or password.\"");
		return qfalse;
	}

	flags = Accounts_GetFlags(acc);

	accSec = Accounts_GetSeccode(acc);
	Accounts_GetLastIp(acc, lastIP);
	if (!(flags & ACCFLAGS_NOSECCODE)) {
		if (accSec && lastIP[0] && Lmd_IPs_CompareIP(ent->client->sess.Lmd.ip, lastIP) == qfalse) {
			if (!secCode[0]) {
				trap_SendServerCommand(ent->s.number, "chat \"^3Your ip does no match the last logged ip. Please enter your ^2security code ^3to login in.\"");
				Disp(ent, "^2Usage: ^3login <username> <password> [security_code] ");
				return qfalse;
			}
			else if (stricmp(accSec, secCode) != 0) {
				//Disp(ent, "^1Invalid security code.");
				trap_SendServerCommand(ent->s.number, "chat \"^1Invalid security code.\"");
				return qfalse;
			}
		}
	}

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (g_entities[i].inuse && i != ent->s.number && g_entities[i].client->pers.Lmd.account == acc) {
			Disp(ent, "^1Someone is using that account.");
			trap_SendServerCommand(i, va("cp \"^1WARNING\n^3Client ^1%i^3 (name ^2\'%s\'^3) is trying\n^3to log on to your account.\"",
				ent->s.number, ent->client->pers.netname));
			Com_Printf("info: %s (%i) is trying to log on to %s's account.\n", ent->client->pers.netname, ent->s.number, g_entities[i].client->pers.netname);
			return qfalse;
		}
	}

	if (Lmd_Accounts_Player_Login(ent, acc)) {
		if (!Auths_AccHasAdmin(acc)) {
			trap_SendServerCommand(ent->s.number, "chat \"^2Login successful.\"");
			Disp(ent, va("^3The account will expire in ^2%i^3 days if you do not login before that.", accountLiveTime(Accounts_Prof_GetLevel(acc))));
		}
		else {
			trap_SendServerCommand(ent->s.number, "chat \"^2Login successful.\"");
			Disp(ent, va("^3You are authenticated with ^2%s^3.", Auths_QuickAuthList(acc)));
		}
	}
	else {
		//Disp(ent, "^1Could not log in.");
		trap_SendServerCommand(ent->s.number, "chat \"^1Could not log in.\"");
	}

	if (lmd_loginsecurity.integer == 1 || (lmd_loginsecurity.integer == 2 && Auths_GetRank(acc) > 0)) {
		if (flags & ACCFLAGS_NOSECCODE || accSec == NULL) {
			Disp(ent, "^3This server requires you to have a security code. Your security code has been enabled.\n");
			Accounts_AddFlags(acc, -ACCFLAGS_NOSECCODE);
			if (!accSec) {
				Lmd_Accounts_Player_CreateSecurityCode(ent, acc);
			}
		}
	}


	return qtrue;
}

qboolean IsValidPlayerName(char *name, gentity_t *ent, qboolean isRegister, char **reason);

qboolean IsValidUsername(char *username, char **reason) {
	int i = 0, c = 0;
	char stripped[MAX_NETNAME];

	*reason = NULL;

	Q_strncpyz(stripped, username, sizeof(stripped));
	Q_CleanStr2(username);
	if (Q_stricmp(username, stripped) != 0 || strchr(username, ' ')) {
		*reason = "^3Username must only contain numbers and letters.";
		return qfalse;
	}
	while (username[i]) {
		if ((username[i] >= 'A' && username[i] <= 'Z') || (username[i] >= 'a' && username[i] <= 'z'))
			c++;
		i++;
	}
	if (c < 2) {
		*reason = "^3Your username must have at least one non-number character.";
		return qfalse;
	}
	else if (Accounts_GetByUsername(username)) {
		*reason = "^3That username is already taken.";
		return qfalse;
	}


	return qtrue;
}

qboolean Lmd_Accounts_Player_Register(gentity_t *ent, char *username, char *passwd) {
	char stripped[MAX_NETNAME];
	char *failReason;
	int i = 0, c = 0;

	Account_t *account;

	if (ent->client->pers.Lmd.account) {
		//Disp(ent, "^3You are already registered.");
		trap_SendServerCommand(ent->s.number, "chat \"^3You are already registered.\"");
		return qfalse;
	}

	if (!IsValidPlayerName(username, ent, qtrue, &failReason)) {
		//trap_SendServerCommand(ent->s.number, "chat \"^1Invalid player name.\"");
		Disp(ent, va("^1You cannot register this player name.\n^3%s", failReason));
		trap_SendServerCommand(ent->s.number, va("chat \"^1%s\"", failReason));
		return qfalse;
	}

	if (!IsValidUsername(username, &failReason)) {
		//trap_SendServerCommand(ent->s.number, "chat \"^1Invalid user name.\"");
		Disp(ent, va("^1You cannot register this username.\n^3%s", failReason));
		trap_SendServerCommand(ent->s.number, va("chat \"^1%s\"", failReason));
		return qfalse;
	}


	if (Accounts_GetByName(username)) {
		trap_SendServerCommand(ent->s.number, "chat \"^1The name is already taken.\"");
		return qfalse;
	}


	account = Accounts_New(username, username, passwd); //username/playername/pass
	if (!account) {
		Disp(ent, "^1Could not create a new account. There may be a problem with the server.");
		return qfalse;
	}

	RenamePlayer(ent, username); //setting new name up
	trap_SendServerCommand(ent->s.number, va("chat \"^2Account %s^2 is registered.\"",username));
	Disp(ent, va(
		"%s^2 is now registered. Now known as ^2%s\n"
		"^3From now on, type '^2\\login %s [password]^3' to use your account.\n"
		"^1Be sure to remember your username and password, or you will not be able to log back in and use the name you have registered.",
		ent->client->pers.netname, username, username));

	ClientUserinfoChanged(ent->s.number);

	Lmd_Accounts_LogAction(ent, account, "registered");


	if (lmd_loginsecurity.integer == 0) {
		Accounts_AddFlags(account, ACCFLAGS_NOSECCODE);
	}
	else if (lmd_loginsecurity.integer == 1) {
		// Required for all.
		Lmd_Accounts_Player_CreateSecurityCode(ent, account);
	}

	Lmd_Accounts_Player_Login(ent, account);

	return qtrue;
}

qboolean Lmd_Accounts_Bot_Login(gentity_t *ent) {
	int i;
	Account_t *acc = Accounts_GetByName(ent->client->pers.netname);

	//Check for a bot using a regular player's name
	//if(acc && Accounts_Prof_GetProfession(acc) != PROF_BOT) return qfalse; //return false, we cannot login into an account.
	//iomatix: now bot could use custom skill's tree and professions.

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (i == ent->s.number || !g_entities[i].inuse || !g_entities[i].client)
			continue;
		//no login if sharing an account.
		if (acc && g_entities[i].client->pers.Lmd.account == acc)
			return qfalse;
		//no login if sharing a name.
		if (!Q_stricmpname(g_entities[i].client->pers.netname, ent->client->pers.netname))
			return qfalse;
	}

	//if dosen't exist create a new account.
	if (!acc) {
		char newstr[MAX_STRING_CHARS];
		Q_strncpyz(newstr, ent->client->pers.netname, sizeof(newstr));
		Q_CleanStr2(newstr);
		acc = Accounts_New(newstr, ent->client->pers.netname, NULL);
		if (!acc)
			return qfalse;
	}

	if (Accounts_GetScore(acc) < 10) //Bots always start with at least 10
		Accounts_SetScore(acc, 10);

	return Lmd_Accounts_Player_Login(ent, acc);
}

qboolean IsValidName(char *name) {
	char cmpName[MAX_NETNAME];
	int c = 0;
	int i = 0;

	if (!name || !name[0])
		return qfalse;

	if (strlen(name) >= MAX_NETNAME)
		return qfalse;

	if (strchr(name, '\n') != NULL || strchr(name, '\r') != NULL)
		return qfalse;

	Q_strncpyz(cmpName, name, sizeof(cmpName));
	Q_CleanStr2(cmpName);

	while (cmpName[i] && i < MAX_NAME_LENGTH) {
		if (Q_isprint(cmpName[i]) == qfalse) {
			return qfalse;
		}

		if ((cmpName[i] >= 'a' && cmpName[i] <= 'z') || (cmpName[i] >= 'A' && cmpName[i] <= 'Z')) {
			c++;
		}

		// Must be a valid infostring
		if (cmpName[i] == '\"' || cmpName[i] == ';' || cmpName[i] == '\\') {
			return qfalse;
		}

		i++;
	}

	if (c < 3) {
		return qfalse;
	}

	return qtrue;
}

qboolean IsValidPlayerName(char *name, gentity_t *ent, qboolean isRegister, char** reason) {
	char cmpName[MAX_NETNAME];

	*reason = NULL;

	if (ent && ent->r.svFlags & SVF_BOT) {
		return qtrue;
	}

	if (IsValidName(name) == qfalse) {
		return qfalse;
	}

	if (Q_stricmpname(name, "Padawan") == 0) {
		if (isRegister || (ent && ent->client->pers.Lmd.account)) {
			*reason = "Registered accounts cannot use the name \'Padawan\'";
			return qfalse;
		}

		return qtrue; //just deal with it
	}

	Q_strncpyz(cmpName, name, sizeof(cmpName));
	Q_CleanStr2(cmpName);

	if (!((g_cmdDisable.integer * 2) & (1 << 1))) {
		Account_t *acc = Accounts_GetByName(cmpName);
		if (acc || G_GetBotInfoByName(cmpName)) {
			if (!ent || ent->client->pers.Lmd.account != acc) {
				*reason = "This name is already in use";
				return qfalse;
			}
		}
	}

	return qtrue;
}

qboolean IsValidPlayerName(char *name, gentity_t *ent, qboolean isRegister) {
	char *unused;
	return IsValidPlayerName(name, ent, isRegister, &unused);
}

void Cmd_ChPasswd_f(gentity_t *ent, int iArg) {
	if (trap_Argc() < 2) {
		Disp(ent, "^3Usage: chpasswd ^2<password>");
		return;
	}
	char *passwd = ConcatArgs(1);
	PlayerAcc_SetPassword(ent, passwd);
	//Disp(ent, "Password changed.");
	trap_SendServerCommand(ent->s.number, "chat \"^2Password changed.\"");
}

void GetStats(gentity_t *ent, Account_t *acc);
void Cmd_GetStats_f(gentity_t *ent, int iArg) {
	GetStats(ent, ent->client->pers.Lmd.account);
}

qboolean DropCredits(gentity_t *ent, int amount);
gentity_t* AimAnyTarget(gentity_t *ent, int length);
void Cmd_Credits_f(gentity_t *ent, int iArg) {
	int ownCreds = PlayerAcc_GetCredits(ent), numCreds;
	char sArg[MAX_TOKEN_CHARS];

	if (!ent->client->pers.Lmd.account) {
		//Disp(ent, "^3You must be logged in to use this.");
		trap_SendServerCommand(ent->s.number, "chat \"^1You must be logged in to use this command.\"");
		return;
	}

	if (iArg == 0) {
		//Disp(ent, va("^3You have ^2CR %i^3.", ownCreds));
		trap_SendServerCommand(ent->s.number, va("chat \"^3You have ^2CR %i^3.\"",ownCreds));
		return;
	}

	if (trap_Argc() < 2) {
		//iomatix
		trap_SendServerCommand(ent->s.number, "chat \"^1Not enough arguments.\"");
		//Disp(ent, "^3Not enough arguments.");
		return;
	}

	//RoboPhred
	if (ent->client->sess.spectatorState != SPECTATOR_NOT) {
		//Disp(ent, "^3You cannot do this while spectating.");
		trap_SendServerCommand(ent->s.number, "chat \"^1You cannot do this while spectating.\"");
		return;
	}

	if (ent->health <= 0) {
		//Disp(ent, "^3You must be alive to use this command.");
		trap_SendServerCommand(ent->s.number, "chat \"^1You must be alive to use this command.\"");
		return;
	}

	trap_Argv(1, sArg, sizeof(sArg));
	numCreds = atoi(sArg);
	if (numCreds < 1) {
		//Disp(ent, "^3Invalid value, credits must be >= 0");
		trap_SendServerCommand(ent->s.number, "chat \"^1Invalid value, must be at least 1.\"");
		return;
	}

	if (numCreds > ownCreds) {
		//Disp(ent, va("^3You cannot afford ^1CR %i^3.", numCreds));
		trap_SendServerCommand(ent->s.number, va("chat \"^1You cannot afford ^1CR %i^3.\"",numCreds));
		return;
	}
	if (iArg == 1) {
		gentity_t *payEnt = AimAnyTarget(ent, 64);
		if (!payEnt) {
			//Disp(ent, "^3Nothing targeted");
			trap_SendServerCommand(ent->s.number, "chat \"^1Nothing targeted.\"");
			return;
		}
		if (!payEnt->client) {
			if (!(payEnt->flags&FL_PAY) && !payEnt->pay) {
				//Disp(ent, "^3This is not a payable entity");
				trap_SendServerCommand(ent->s.number, "chat \"^1This is not a payable entity.\"");
				return;
			}
			else if (payEnt->flags & FL_INACTIVE || !payEnt->use) {
				//Disp(ent, "^3This entity is inactive");
				trap_SendServerCommand(ent->s.number, "chat \"^1This entity is inactive.\"");
				return;
			}
		}

		if (payEnt->client) {
			if (!payEnt->client->pers.Lmd.account) {
				//Disp(ent, "^3This player is not logged in.");
				trap_SendServerCommand(ent->s.number, "chat \"^1This player is not logged in.\"");
				return;
			}
			PlayerAcc_SetCredits(ent, ownCreds - numCreds);

			//Disp(ent, va("^3You paid ^2CR %i^3 to ^7%s.", numCreds, payEnt->client->pers.netname));
			trap_SendServerCommand(ent->s.number, va("chat \"^3You paid ^2CR %i^3 to ^7%s.\"", numCreds, payEnt->client->pers.netname));
			GiveCredits(payEnt, numCreds, va("from ^7%s", ent->client->pers.netname));
		}
		else {
			if (payEnt->pay)
			{
				if (payEnt->pay(payEnt, numCreds, ent) == qtrue)
					PlayerAcc_SetCredits(ent, ownCreds - numCreds);
			}
			else
			{
				payEnt->genericValue5 = numCreds;
				payEnt->use(payEnt, ent, ent);
				// Reset ownCreds, as the entity might manipulate credits
				ownCreds = PlayerAcc_GetCredits(ent);
				PlayerAcc_SetCredits(ent, ownCreds - numCreds);
			}
		}
	}
	else if (iArg == 3) {
		if (ent->s.m_iVehicleNum || ent->client->sess.sessionTeam == TEAM_SPECTATOR ||
			ent->client->pers.Lmd.jailTime > level.time) {
			return; 
		}
		DropCredits(ent, numCreds);
	}
}

qboolean Lmd_Accounts_Player_Register(gentity_t *ent, char *username, char *passwd);
void Cmd_Register_f(gentity_t *ent, int iArg) {
	char username[MAX_STRING_CHARS]; //size checked in registerAccount()
	char *passwd;
	if (trap_Argc() < 3) {
		if(trap_Argc() < 2)trap_SendServerCommand(ent->s.number, "chat \"^3Register a new account.\"");		
		Disp(ent, "^3You have to choose a username and password.\n\\^2register <username> <password>\n^3Your username will become your in-game character name.");
		return;
	}
	trap_Argv(1, username, sizeof(username));
	passwd = ConcatArgs(2);
	Lmd_Accounts_Player_Register(ent, username, passwd);
}

void Cmd_Seccode_f(gentity_t *ent, int iArg) {
	int argc = trap_Argc();

	if (!ent->client->pers.Lmd.account) {
		//Disp(ent, "^3You must be logged in to use this.");
		trap_SendServerCommand(ent->s.number, "chat \"^1You must be logged in to use this.\"");
		return;
	}

	qboolean codeEnabled = (PlayerAcc_GetFlags(ent) & ACCFLAGS_NOSECCODE) == 0;

	if (argc < 2) {
		char *code = Accounts_GetSeccode(ent->client->pers.Lmd.account);
		Disp(ent, va("^3Your security code is ^2%s^3\n"
			"^3Your security code is %s^3.\n"
			"^3Use \'^2/seccode new^3\' to create a new security code, or \'^2/seccode toggle^3\' to %s it.",
			code,
			codeEnabled ? "^2Enabled" : "^1Disabled",
			codeEnabled ? "disable" : "enable"));
		trap_SendServerCommand(ent->s.number, va("chat \"^3Your security code: ^2%s.\"",code));
		return;
	}

	char cmd[MAX_STRING_CHARS];
	trap_Argv(1, cmd, sizeof(cmd));

	if (Q_stricmp(cmd, "new") == 0) {
		Lmd_Accounts_Player_CreateSecurityCode(ent, ent->client->pers.Lmd.account);
		return;
	}
	else if (Q_stricmp(cmd, "toggle") == 0) {
		if (codeEnabled) {
			if (lmd_loginsecurity.integer == 1) {
				//Disp(ent, "^3You cannot disable your security code. This server requires security codes for all users.");
				trap_SendServerCommand(ent->s.number, "chat \"^1You cannot disable your security code. This server requires the security code for each user.\"");
				return;
			}
			else if (Auths_GetRank(ent->client->pers.Lmd.account) > 0 && lmd_loginsecurity.integer >= 2) {
				//Disp(ent, "^3You cannot disable your security code.  This server requires security codes for all admins.");
				trap_SendServerCommand(ent->s.number, "chat \"^1You cannot disable your security code. This server requires the security code for all admins.\"");
				return;
			}

			//Disp(ent, "^3Your security code is now ^1disabled^3.");
			trap_SendServerCommand(ent->s.number, "chat \"^3Your security code is now ^1disabled^3.\"");
			PlayerAcc_AddFlags(ent, ACCFLAGS_NOSECCODE);
		}
		else {
			//Disp(ent, "^3Your security code is now ^2enabled^3.");		
			trap_SendServerCommand(ent->s.number, "chat \"^3Your security code is now ^2enabled^3.\"");			
			PlayerAcc_AddFlags(ent, -ACCFLAGS_NOSECCODE);
		}
	}
	else {
		Disp(ent, va("^1Unknown argument ^2%s", cmd));
	}
}

qboolean Lmd_Accounts_Player_TryLogin(gentity_t *ent, char *username, char *pass, char *secCode);
void Cmd_Login_f(gentity_t *ent, int iArg)
{
	char username[MAX_STRING_CHARS];
	char passwd[MAX_STRING_CHARS];
	char secCode[MAX_STRING_CHARS];
	if (trap_Argc() < 2) {
		trap_SendServerCommand(ent->s.number, "chat \"^3Login to your account.\"");
		Disp(ent, "^3Usage: login ^2<username> <password> [seccode]");
		return;
	}
	trap_Argv(1, username, sizeof(username));
	trap_Argv(2, passwd, sizeof(passwd));
	trap_Argv(3, secCode, sizeof(secCode));

	Lmd_Accounts_Player_TryLogin(ent, username, passwd, secCode);
}

void Lmd_Accounts_Player_Logout(gentity_t *ent);
void Cmd_Logout_f(gentity_t *ent, int iArg) {
	//Ufo: forbid logging out while dueling or frozen
	if (!ent || !ent->client || duelInProgress(&ent->client->ps) || ent->client->Lmd.flags & SNF_FREEZE) {
		//Disp(ent, "^3You cannot logout at this time.");
		trap_SendServerCommand(ent->s.number, "chat \"^1You cannot logout at this time.\"");
		return;
	}
	Lmd_Accounts_Player_Logout(ent);
}

void listWorthy(gentity_t *ent);
void Cmd_Worthy_f(gentity_t *ent, int iArg) {
	listWorthy(ent);
}

void Cmd_Inventory_f(gentity_t *ent, int iArg);
void Cmd_Property_f(gentity_t *ent, int iArg);
//iomatix:

void Cmd_aliasname_f(gentity_t *ent, int iArg) {
	char val[MAX_STRING_CHARS];
	char arg[MAX_TOKEN_CHARS];

	if (trap_Argc() < 2 ) {
		//Disp(ent, "^1The alias can not be empty.");
		trap_SendServerCommand(ent->s.number, "chat \"^1The alias can not be empty.\"");
		return;
	}else if(trap_Argc() > 2){
		//Disp(ent, "^1Too many arguments.");
		trap_SendServerCommand(ent->s.number, "chat \"^1Too many arguments.\"");
		return;
	}

	Account_t *thisacc;
	thisacc = ent->client->pers.Lmd.account;
	trap_Argv(0, arg, sizeof(arg));
	trap_Argv(1, val, sizeof(val));
	ClientCleanName((const char *)val, arg, sizeof(arg));
	if (!IsValidPlayerName(arg, NULL, qtrue)) {
		//Disp(ent, "^1That name is invalid or already in use.");
		trap_SendServerCommand(ent->s.number, "chat \"^1That name is invalid or already in use.\"");
		return;
	}
	Accounts_SetName(thisacc, arg);
	//Disp(ent, va("^2Your alias is %s now.",arg));
	trap_SendServerCommand(ent->s.number, va("chat \"^2Your alias is %s now.\"",arg));
	Lmd_Accounts_Player_Logout(ent);
	Lmd_Accounts_Player_Login(ent, thisacc);
	
}
int get_random(int min, int max)
{
	int tmp;
	if (max >= min)
		max -= min;
	else
	{
		tmp = min - max;
		min = max;
		max = tmp;
	}
	return max ? (rand() % max + min) : min;
}
int Open_Creditbox(gentity_t *ent) { //Get random value from the credit box
									 //Chances: 55% for 50-350CR, 25% for 360-1500CR, 15% for 1600-5500CR, 4% for 8000-20 000CR, 1% 20 000 - 175 000CR 
	int value = get_random(0, 101);

	if (value <= 55)
	{
		value = get_random(5, 35) * 10; //50-350 by 10
		Disp(ent, "^3This box is almost empty...");
	}
	else if (value <= 80) {
		value = get_random(36, 150) * 10; //360-1500 by 10
		Disp(ent, "^3This is small box...");
	}
	else if (value <= 95) {
		value = get_random(16, 55) * 100; //1600-5500 by 100
		Disp(ent, "^3This is medium box...");
	}
	else if (value <= 99) {
		value = get_random(16, 40) * 500; //8000-20000 by 500
		Disp(ent, "^3This is large box...");
	}
	else if (value <= 101) {
		value = get_random(20, 175) * 1000; //20 000-175 000 by 1000
		Disp(ent, "^3This box full of credits!");
	}
	return value;
}
void Cmd_Creditbox_f(gentity_t *ent, int iArg) {
	char arg[MAX_TOKEN_CHARS];
	trap_Argv(1, arg, sizeof(arg));
	if (Q_stricmp("open", arg) == 0)
	{
		int chests = PlayerAcc_GetLootboxes(ent);
		int credits_amount;
		if (chests > 0)
		{
			chests--;
			PlayerAcc_SetLootboxes(ent, chests);
			credits_amount = Open_Creditbox(ent);
			GiveCredits(ent, credits_amount, "from the Credit Box.");
			//Disp(ent, va("^2Recived ^3%i ^2Credits!", credits_amount));
			trap_SendServerCommand(ent->s.number, va("chat \"^2Recived ^3%i ^2Credits!\"", credits_amount));
		}else  //Disp(ent, "^1You have no Credit Boxes right now.");
		trap_SendServerCommand(ent->s.number, "chat \"^1You have no Credit Boxes right now.\"");

	}
	else {

		Disp(ent, "^3Type ^2creditbox open ^3to open one Credit Box.");
	}
}
cmdEntry_t accountCommandEntries[] = {
{ "alias","Change your display name. ^3The account name won't change so please login with your old username. ", Cmd_aliasname_f, 0, qfalse, 1, 256, 0, 0 },
{ "chpasswd","Change the password for your account.", Cmd_ChPasswd_f, 0, qfalse, 1, 1, 0, 0 },
{ "credits","Check your current wealth.", Cmd_Credits_f, 0, qfalse, 1, 128, ~(1 << GT_FFA), 0 },
{ "creditbox", "Open the Credit Box.", Cmd_Creditbox_f, 0, qfalse, 1, 128, 0, 0 },
{ "dropcr", "Drop credits.", Cmd_Credits_f, 3, qfalse, 1, 128, ~(1 << GT_FFA), 0 },
{ "inventory","View and use items in your inventory.", Cmd_Inventory_f, 0, qfalse, 1, 1, ~(1 << GT_FFA), 0 },
{ "login"," Login to use the name you registered with \\register.", Cmd_Login_f, 0, qfalse, 0, 1, 0, 0, qtrue },
{ "logout", "Logs out of your account.  If you are not in an account but have admin, you will loose admin status.", Cmd_Logout_f, 0, qfalse, 1, 0, 0, 0 },
{ "pay", "Give the player you are looking at CR <amount>.", Cmd_Credits_f, 1, qfalse, 1, 128, ~(1 << GT_FFA), 0 },
{ "property", "View your owned properties.  If you have the right rank, you can modify your property access here.", Cmd_Property_f, 0, qfalse, 1, 0, 0, 0 },
{ "register", "Register your account. ^5Your username will become your player name.", Cmd_Register_f, 0, qfalse, 0, 1, 0, 0, qtrue },
{ "seccode", "Show, edit, regenerate, or enable/disable your security code.", Cmd_Seccode_f, 0, qfalse, 1, 1, 0, 0 },
{ "stats","Shows some stats (you need to be registered)", Cmd_GetStats_f, 0, qfalse, 1, 1, 0, 0 },
{ "worthy", "List players on the server from whom you can score points in a duel.", Cmd_Worthy_f, 0, qfalse, 1, 1, ~(1 << GT_FFA), 0 },
{ NULL },
};
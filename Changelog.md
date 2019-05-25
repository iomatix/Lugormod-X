# Changelog:

## 3.0.1.0
- New Lugormod X version by iomatix release. ✔️
- Registration reworked. ✔️
- Bot's logging reworked. They are able to use professions now. ✔️
- Bot is trying to re-log on spawn when it's not logged. ✔️
- Profession selection is changed. ✔️
- Changed Maximum level up to 120. ✔️
- Admin is able to change level up to 255 via command now. Remember that, max buyable level is 120! ✔️
- After reaching 40 level Force user is able to learn new skill tree. ✔️
- Force Power adjusted depending on profession level. ✔️
- Skillreset command bug is fixed. ✔️
- Mercenary gets his jetpack after first fuel skill upgrade. ✔️
- Added lmd_jedi_pickup_weapons, set it to 1 to allow jedi class to pick up weapons and ammo from the map. Default 0 ✔️
- Added lmd_jedi_add_hp_level, set it to 1 to allow jedi class to get additional health-points per level, set it to 2 to use like mercenaries (may be op). Default 1 ✔️
- Added lmd_profession_fee variable to set the fee when changing the profession. Default 1350. ✔️
- Added lmd_skillpoints_perlevel variable that amount of skill points will be gived per each level up. Default 4. ✔️
- Reworked skillpoints checking system. Old one was totally bugged. ✔️
- Added more basic information in command-popup and repaired old messy ones for usable commands. ✔️

##TODO
- TODO: Player can spend skillpoints to upgrade their max hp/power/shield capacity. 50%
- TODO: Make MaxAmmo function for explosive materials.  

- TODO: Player after reaching level 120 is able to start over again from level 1 but with perma +25hp/+25force/+25shield boost. Repeatable. 
- TODO: Add lootboxes called now Credits Boxes. Chances: 80% for 25-200CR, 15% for 200-1000CR, 4% for 1000-10000CR, 1% for 10000-10
- TODO: Killing streak Bounty. After each lmd_bounty_streaks_by kills bounty is increasing. For killing bounty target player will recive Credit Boxes. new variable (lmd_bounty_streaks_by default "5")

- TODO: Add experience needed to level up. (Not tradable, add new cmd for admin to set experience.) It will block economic looses for servers. Level DOSEN'T need credits.
- TODO: lmd_exp_for_level /default 450
- TODO: Now skillpoints will cost credits to be spend (level(level+1)lmd_skillpoint_cost)  //default 150

- TODO: Lootbox for level up each 10 lvls %10
- TODO: Messages time control.
- TODO: Add drop item from inventory.
- TODO: Repair selling option trading system. Add message (motd) + try to console disp. If not create new buy/sell inv options.
- TODO: Messages motd for duel types.
- TODO: Lootboxes for killstreak


- TODO: Adding openjk API support
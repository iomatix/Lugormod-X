# The User Guide

### Important Note: Do not use commands on the chat but instead press `~` button on the keyboard to open the console window. Follow the directing inside the window.
### Small Note: There is no need to use full commands, it is possible to short most of them, E.g. profession -> prof, register -> reg, skill jedi heal -> sk j h, etc.

## How to start:

### Registration, Login, Passwords, Nicknames:
**To get access to most features on the server with Lugormod-X installed, the player must log in to his account first.**

- To create a new account, use `~` button to open the console window and then type the command **`register |username| |password|`**.

If the authentication process is successful keep your username and password from now because you will need them to log in to the account.
If the server uses a security code system by default, your 4-digit security code will be generated. Keep it save too because you can recover it only through contact with server's administration.
- The command **`seccode`** allows to check your security code, **`seccode new`** will generate a new one, **`seccode toggle`** will enable or disable it. If it is enabled, the code is necessary when you are going to log in using a new IP address.

- To log in to the account use the command **`login |username| |password|`** or **`login |username| |password| |seccode|`**, if the security code is enabled and you are trying to log in from a new IP address.

- To change the account's password use **`chpasswd |new_password|`** command.

You can **not** change your **username** but instead, it is possible to change your in-game name called nickname, netname, alias.
- To change the nickname use **`alias |new_name|`** command.

### Professions, Statistics, Skills:
**Before evolving character, upgrading skills and gaining levels, the player must choose his profession.**

**Changing the profession may cost credits!**

- To check the professions list type **`profession`**.  To choose a new profession use **`profession |name|`**. You can change your profession whenever you'd like to.

Force User: One who has learned the powers of the force. May specialize in light or dark powers. Draws a lightsaber. 
Mercenary: Trained in weapons. Excels at attacking from distance and avoids direct confrontations.

The player gains levels through gaining experience points or buying level with **`buylevel`** command.
Increasing profession level gives access to more skill points which are useful for upgrading skills.

**Upgrading and resetting skills costs credits!**

- To check account information, progress information, statistics, and more type **`stats`**.
- To check available skill points or skills themselves type **`skill`** to move through the trees and check more detailed information use **`skill |skill_name|`** or **`skill |tree| |skill_name|`**.
E.g. **`skill sith`** shows skills inside the sith tree, **`skill sith grip`** shows more detailed information about the Grip skill.
- To upgrade the skill type full path to the skill and **`up`** E.g **`skill sith grip up`**. 
The profession doesn't have to has got a skill tree. E.g. Mercenary ammo skill isn't inside any tree so upgrade path is just a **`skill ammo up`**.

**Note: If the skill has spacing in the name do not type full skill name, just a first word of the syntax.**

- To check the skill reset cost type *`resetskill`**.
- To reset all skills use **`resetskill confirm`** command.

Mastery Level is equal to 40 profession level after reaching this level player is able to upgrade skills above 3 and gains access to blocked skills trees.
Level Cap is equal to 120 after reaching this level the experience gain stops.

The player is able to gain more skill points through the New Game Plus mode after reaching at least the Mastery Level.
- **`newgame start`** resets current profession's progress in return increases New Game Plus Level which gives New Game Plus Skill Points.

### Credit-Boxes:
**Credit-Boxes keep credits inside.**
- type **`creditbox open`** to open one.

### The Black List - Bounties:
**The Bounty System is a complex system which allows players to create orders for the other players, and at the same time it is creating its own orders.**

Automated orders cannot be controlled. They give temporary chance for the Credit-Box, additional experience and credits but also updating the Black List.

- Type **`bounty |name| |credits|`** to place an order for the registered user |name| with the |credits|CR price to the Black List.
- Use **`blacklist`** to check the Black List or **`blacklist |name|`** to search |name| on the list.

## Most important commands you need to know:
- **`help`** and **`help |command|`** to show more detailed information about commands available.
- **`register |username| |password|`** to register a new account.
- **`login |username| |password|`**, **`login |username| |password| |seccode|`** to log in to an existing account.
- **`seccode`** to check account's security code.
- **`chpasswd |new_password|`** to change the account's password.
- **`alias |new_name|`** to change in-game name. The user name will not change so your login data remains the same.
- **`stats`** to check detailed account's information.
- **`profession`** and **`profession |name|`** to change the profession.
- **`skill`**, **`skill |name|`** and **`skill |name| up`** to check detailed information concerning profession's skills. **up** to upgrade the skill.
- **`resetskill`**, **`resetskill confirm`** to reset all active skills.
- **`newgame start`** resets current profession's progress. Above the Mastery Level in return increases New Game Plus Level which gives New Game Plus Skill Points. **`newgame reset`** resets the New Game Plus Level.
- **`creditbox open`** to open the Credit-Box.
- **`bounty |name| |credits|`** to place an order to the Black List.
- **`blacklist`** and **`blacklist |name|`** to check the Black List.

### [The Full Commands List](Commands.md)

### Credits:

**Copyright (C) 2019 iomatix.**

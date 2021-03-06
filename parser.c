#include "parser.h"
#include "spells.h"
#include "skills.h"
#include "keywords.h"
#include "items.h"

#include "unit.h"
#include "region.h"
#include "faction.h"

#include "rtl.h"

#include <quicklist.h>
#include <string.h>
#include <stdlib.h>

const char *skillnames[] = {
    "mining",
    "lumberjack",
    "quarrying",
    "horse training",
    "weaponsmith",
    "armorer",
    "building",
    "shipbuilding",
    "entertainment",
    "stealth",
    "observation",
    "tactics",
    "riding",
    "sword",
    "crossbow",
    "longbow",
    "magic",
};

const char *itemnames[MAXITEMS][2] = {
    { "iron", "iron" },
    { "wood", "wood" },
    { "stone", "stone" },
    { "horse", "horses" },
    { "sword", "swords" },
    { "crossbow", "crossbows" },
    { "longbow", "longbows" },
    { "chain mail", "chain mail" },
    { "plate armor", "plate armor" },
    { "Amulet of Darkness", "Amulets of Darkness" },
    { "Amulet of Death", "Amulets of Death" },
    { "Amulet of Healing", "Amulets of Healing" },
    { "Amulet of True Seeing", "Amulets of True Seeing" },
    { "Cloak of Invulnerability", "Cloaks of Invulnerability" },
    { "Ring of Invisibility", "Rings of Invisibility" },
    { "Ring of Power", "Rings of Power" },
    { "Runesword", "Runeswords" },
    { "Shieldstone", "Shieldstones" },
    { "Staff of Fire", "Staffs of Fire" },
    { "Staff of Lightning", "Staffs of Lightning" },
    { "Wand of Teleportation", "Wands of Teleportation" }
};

const char *spellnames[MAXSPELLS] = {
    "Black Wind",
    "Cause Fear",
    "Contaminate Water",
    "Dazzling Light",
    "Fireball",
    "Hand of Death",
    "Heal",
    "Inspire Courage",
    "Lightning Bolt",
    "Make Amulet of Darkness",
    "Make Amulet of Death",
    "Make Amulet of Healing",
    "Make Amulet of True Seeing",
    "Make Cloak of Invulnerability",
    "Make Ring of Invisibility",
    "Make Ring of Power",
    "Make Runesword",
    "Make Shieldstone",
    "Make Staff of Fire",
    "Make Staff of Lightning",
    "Make Wand of Teleportation",
    "Shield",
    "Sunfire",
    "Teleport",
};

int strpcmp(const void *s1, const void *s2)
{
    return _strcmpl(*(char **) s1, *(char **) s2);
}

keyword_t findkeyword(const char *s)
{
    const char **sp;

    if (!_strcmpl(s, "describe"))
        return K_DISPLAY;

    sp = (const char **)bsearch(&s, keywords, MAXKEYWORDS, sizeof s, strpcmp);
    if (sp == 0) {
        return MAXKEYWORDS;
    }
    return sp - keywords;
}

keyword_t igetkeyword(const char *s)
{
    return (keyword_t)findkeyword(igetstr(s));
}

keyword_t getkeyword(void)
{
    return (keyword_t)findkeyword(igetstr(0));
}

static const char * directions[MAXDIRECTIONS][3] = {
    { "north", "n", 0 }, { "south", "s", 0 },
    { "west", "w", 0 }, { "east", "e", 0 },
    { "mir", "m", 0 }, { "ydd", "y", 0 }
};

const char *direction_name(int d) {
    if (d >= 0 && d < MAXDIRECTIONS) {
        if (config.directions[d]) {
	   return config.directions[d][0];
	}
        return directions[d][0];
    }
    return 0;
}

int finddirection(const char *s) {
    int d;
    for (d = 0; d != MAXDIRECTIONS; ++d) {
       const char ** p = directions[d];
       if (config.directions[d]) p = (const char **)config.directions[d];
       while (*p) {
          if (!_strcmpl(s, *p++)) return d;
       }
    }
    return -1;
}

int findstr(const char **v, const char *s, int n)
{
    int i;

    for (i = 0; i != n; i++)
        if (!_strcmpl(v[i], s))
            return i;

    return -1;
}

int findskill(const char *s)
{
    if (!_strcmpl(s, "horse"))
        return SK_HORSE_TRAINING;
    if (!_strcmpl(s, "entertain"))
        return SK_ENTERTAINMENT;

    return findstr(skillnames, s, MAXSKILLS);
}

int getskill(void)
{
    return findskill(igetstr(0));
}

int finditem(const char *s)
{
    int i;

    if (!_strcmpl(s, "chain"))
        return I_CHAIN_MAIL;
    if (!_strcmpl(s, "plate"))
        return I_PLATE_ARMOR;

    for (i = 0; i != MAXITEMS; i++) {
        if (_strcmpl(itemnames[i][0], s)==0 || _strcmpl(itemnames[i][1], s)==0)
            return i;
    }
    return -1;
}

int getitem(void)
{
    return finditem(igetstr(0));
}

int findspell(const char *s)
{
    return findstr(spellnames, s, MAXSPELLS);
}

int getspell(void)
{
    return findspell(igetstr(0));
}

char *igetstr(const char *s1)
{
    int i;
    static const char *s;
    static char buf[256];

    if (s1)
        s = s1;
    while (*s == ' ')
        s++;
    i = 0;

    while (*s && *s != ' ') {
        buf[i] = *s;
        if (*s == '_')
            buf[i] = ' ';
        s++;
        i++;
    }

    buf[i] = 0;
    return buf;
}

unit *getnewunit(region * r, const faction * f)
{
    int n;
    unit *u;

    n = atoi(igetstr(0));

    if (n <= 0) {
        return 0;
    }
    for (u=r->units;u;u=u->next) {
        if (u->faction == f && u->alias == n) {
            return u;
        }
    }

    return 0;
}

unit *getunitg(region *r, const faction *f)
{
    const char *s;

    s = igetstr(0);

    if (!_strcmpl(s, "new")) {
        return getnewunit(r, f);
    }
    return findunitg(atoi(s));
}

int getunit(region * r, const faction *f, unit **uptr)
{
    int n;
    const char *s;
    unit *u;

    s = igetstr(0);

    if (!_strcmpl(s, "new")) {
        unit *u = getnewunit(r, f);
        if (u) {
            *uptr = u;
            return U_UNIT;
        } else {
            *uptr = 0;
            return U_NOTFOUND;
        }
    }

    if (!region_isocean(r) && !_strcmpl(s, "peasants")) {
        *uptr = 0;
        return U_PEASANTS;
    }

    n = atoi(s);

    if (n == 0) {
        *uptr = 0;
        return U_NONE;
    }

    for (u=r->units;u;u=u->next) {
        if (u->no == n && !u->isnew) {
            *uptr = u;
            return U_UNIT;
        }
    }
    *uptr = 0;
    return U_NOTFOUND;
}

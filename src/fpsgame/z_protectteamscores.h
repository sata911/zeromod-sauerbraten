#ifndef Z_PROTECTTEAMSCORES_H
#define Z_PROTECTTEAMSCORES_H

VAR(protectteamscores, 0, 0, 2);

static void z_pruneteaminfos(hashset<teaminfo> *ti)
{
    int oldnum = ti->numelems;
    enumerates(*ti, teaminfo, t,
               if(!t.frags) ti->remove(t.team);
    );
    if(ti->numelems < oldnum) return;
    enumerates(*ti, teaminfo, t,
               if(t.frags<=0) ti->remove(t.team);
    );
    if(ti->numelems < oldnum) return;
    ti->clear();
}

static int z_calcteamscore(hashset<teaminfo> *&ti, const char *team, int fragval)
{
    if(!ti) ti = new hashset<teaminfo>(1<<7);
    teaminfo *t = ti->access(team);
    if(!t)
    {
        if(ti->numelems>=64) z_pruneteaminfos(ti);
        t = &ti->operator[](team);
        copystring(t->team, team, sizeof(t->team));
        t->frags = 0;
    }
    t->frags += fragval;
    return t->frags;
}

void z_setteaminfos(hashset<teaminfo> *&dst, hashset<teaminfo> *src)
{
    if(!src) { DELETEP(dst); return; }
    if(dst) dst->clear();
    int count = 0;
    enumerates(*src, teaminfo, t,
        if(t.frags) { z_calcteamscore(dst, t.team, t.frags); count++; }
    );
    if(dst && !count) DELETEP(dst);
}

bool z_acceptfragval(clientinfo *ci, int fragval)
{
    switch(protectteamscores)
    {
        case 0: default: return true;
        case 1:
        {
            int teamscore = z_calcteamscore(ci->state.teaminfos, ci->team, fragval);
            return fragval > 0 ? (teamscore > 0) : (teamscore >= 0);
        }
        case 2: return fragval > 0 ? (ci->state.frags > 0) : (ci->state.frags >= 0);
    }
}

#endif // Z_PROTECTTEAMSCORES_H
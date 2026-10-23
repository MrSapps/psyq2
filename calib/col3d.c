#include "types.h"
int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll);
int TimesectLine(struct COLLHEADER *Map, int SpaceIndx, struct VECT *Loc, struct SVECT *V, int Scale);

int InitCol3dMove(struct COLLHEADER *Map, struct COL3DMOVE *Move3d, struct VECT *Location, short SpaceNum)
{
    Move3d->SpacePlane = Find3dSpaceIndxLink(Map, Location, SpaceNum, 1);
    if (Move3d->SpacePlane >= 0) {
        Move3d->SpaceTime = TimesectLine(Map, Move3d->SpacePlane, Location, &Move3d->Vel, Move3d->Scale);
        Move3d->Area = Map->Coll3dSpace[Move3d->SpacePlane].Area;
        Move3d->SpacePlane |= Map->ReturnPlaneNum << 11;
        return 1;
    }
    return 0;
}

int In3dSpaceIndx(struct COLLHEADER *Map, struct VECT *Loc, int Indx);

int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll)
{
    int Conx;
    int SpaceIndx;

    if (Main >= 0) {
        if (In3dSpaceIndx(Map, Loc, Main))
            return Main;
        for (Conx = Map->Coll3dSpace[Main].StartJoin; Conx < Map->Coll3dSpace[Main + 1].StartJoin; Conx++) {
            SpaceIndx = Map->Coll3dJoin[Conx].Conx & 0x7ff;
            if (In3dSpaceIndx(Map, Loc, SpaceIndx))
                return SpaceIndx;
        }
    }
    if (CheckAll) {
        for (SpaceIndx = 0; SpaceIndx < Map->TotalNumSpaces; SpaceIndx++)
            if (In3dSpaceIndx(Map, Loc, SpaceIndx))
                return SpaceIndx;
    }
    return -1;
}

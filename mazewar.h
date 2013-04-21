/* $Header: mazewar.h,v 1.7 88/08/25 09:59:51 kent Exp $ */

/*
 * mazewar.h - Definitions for MazeWar
 *
 * Author:	Christopher A. Kent
 * 		Western Research Laboratory
 * 		Digital Equipment Corporation
 * Date:	Wed Sep 24 1986
 */

/* Modified by Michael Greenwald for CS244B, Mar 1992,
   Greenwald@cs.stanford.edu */

/* Modified by Nicholas Dovidio for CS244B, Mar 2009,
 * ndovidio@stanford.edu
 * This version now uses the CS249a/b style of C++ coding.
 */

/***********************************************************
Copyright 1986 by Digital Equipment Corporation, Maynard, Massachusetts,

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital not be
used in advertising or publicity pertaining to disstribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifndef MAZEWAR_H
#define MAZEWAR_H


#include "fwk/NamedInterface.h"

#include "Nominal.h"
#include "Exception.h"
#include "packet.h"
#include <string>
#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>

/* fundamental constants */

#ifndef	TRUE
#define	TRUE		1
#define	FALSE		0
#endif	/* TRUE */

/* Timeouts(ms) */
#define JOIN_TIMEOUT 2000
#define MISSILE_SPEED 200
#define KILLED_TIMEOUT 5000

/* You can modify this if you want to */
#define	MAX_RATS	8
#define	MAX_TRACKED_MISSILE 2

/* network stuff */
/* Feel free to modify.  This is the simplest version we came up with */

/* A unique MAZEPORT will be assigned to your team by the TA */
#define	MAZEPORT	5023
/* The multicast group for Mazewar is 224.1.1.1 */
#define MAZEGROUP       0xe0010101
#define	MAZESERVICE	"mazewar244B"

/* The next two >must< be a power of two, because we subtract 1 from them
   to get a bitmask for random()
 */
#define	MAZEXMAX	32
#define	MAZEYMAX	16
#define	VECTORSIZE	55
#define	NAMESIZE	20
#define	NDIRECTION	4
#define	NORTH		0
#define	SOUTH		1
#define	EAST		2
#define	WEST		3
#define	NVIEW		4
#define	LEFT		0
#define	RIGHT		1
#define	REAR		2
#define	FRONT		3

/* types */

typedef	struct sockaddr_in			Sockaddr;
typedef bool	               		MazeRow[MAZEYMAX];
typedef	MazeRow						MazeType [MAZEXMAX];
typedef	MazeRow						*MazeTypePtr;
//typedef	short						Direction;
typedef	struct {
    short	x, y;
}		XYpoint;
typedef	struct {
    XYpoint	p1, p2;
}	XYpair;
typedef	struct {
    short	xcor, ycor;
} XY;
typedef	struct {
    unsigned short	bits[16];
}	BitCell;
typedef	char						RatName[NAMESIZE];

enum Phase{Join, Init, Play, Killed};

typedef struct {
    uint8_t seqMis;
    bool registeredKill;
    uint32_t victimId;
} infoMis;

typedef struct {
    timeval hitTime;
    uint8_t seqMis;
    uint32_t killerId;
} infoKill;

class Direction : public Ordinal<Direction, short> {
public:
    Direction(short num) : Ordinal<Direction, short>(num) {
        if(num < NORTH || num > NDIRECTION) {
            throw RangeException("Error: Unexpected value.\n");
        }
    }
};

class Loc : public Ordinal<Loc, short> {
public:
    Loc(short num) : Ordinal<Loc, short>(num) {
        if(num < 0) {
            throw RangeException("Error: Unexpected negative value.\n");
        }
    }
};

class Score : public Ordinal<Score, int> {
public:
    Score(int num) : Ordinal<Score, int>(num) {}
};


class RatIndexType : public Ordinal<RatIndexType, int> {
public:
    RatIndexType(int num) : Ordinal<RatIndexType, int>(num) {
        if(num < 0) {
            throw RangeException("Error: Unexpected negative value.\n");
        }
    }
};

class RatId : public Ordinal<RatId, uint32_t> {
public:
    RatId(uint32_t num) : Ordinal<RatId, uint32_t>(num) {
    }
};

class TokenId : public Ordinal<TokenId, long> {
public:
    TokenId(long num) : Ordinal<TokenId, long>(num) {}
};


class RatAppearance {

public:
    RatAppearance() :  x(1), y(1), tokenId(0) {};
    bool	visible;
    Loc	x, y;
    short	distance;
    TokenId	tokenId;
};

class Rat {

public:
    Rat() :  playing(0), x(1), y(1), dir(NORTH),
    xMis(0), yMis(0), hasMissile(false), 
    score(0), name("other"), id(0), seqNum(0), seqMis(0) {};
    bool playing;
    Loc	x, y;
    Direction dir;
    Loc xMis, yMis;
    bool hasMissile;
    Score score;
    string name;
    RatId id;
    uint32_t seqNum;
    uint8_t seqMis;
};

typedef	RatAppearance			RatApp_type [MAX_RATS];
typedef	RatAppearance 			*RatLook;

/* defined in display.c */
extern RatApp_type 			Rats2Display;

/* variables "exported" by the mazewar "module" */
class MazewarInstance :  public Fwk::NamedInterface  {
public:
    typedef Fwk::Ptr<MazewarInstance const> PtrConst;
    typedef Fwk::Ptr<MazewarInstance> Ptr;

    static MazewarInstance::Ptr mazewarInstanceNew(string s) {
        MazewarInstance *m = new MazewarInstance(s);
        return m;
    }

    inline Direction dir() const {
        return dir_;
    }
    void dirIs(Direction dir) {
        this->dir_ = dir;
    }
    inline Direction dirPeek() const {
        return dirPeek_;
    }
    void dirPeekIs(Direction dirPeek) {
        this->dirPeek_ = dirPeek;
    }

    inline long mazePort() const {
        return mazePort_;
    }
    void mazePortIs(long  mazePort) {
        this->mazePort_ = mazePort;
    }
    inline Sockaddr *myAddr() const {
        return myAddr_;
    }
    void myAddrIs(Sockaddr *myAddr) {
        this->myAddr_ = myAddr;
    }
    inline RatId myRatId() const {
        return myRatId_;
    }
    void myRatIdIs(RatId myRatId) {
        this->myRatId_ = myRatId;
    }

    inline bool peeking() const {
        return peeking_;
    }
    void peekingIs(bool peeking) {
        this->peeking_ = peeking;
    }
    inline int theSocket() const {
        return theSocket_;
    }
    void theSocketIs(int theSocket) {
        this->theSocket_ = theSocket;
    }
    inline Score score() const {
        return score_;
    }
    void scoreIs(Score score) {
        this->score_ = score;
    }
    inline Loc xloc() const {
        return xloc_;
    }
    void xlocIs(Loc xloc) {
        this->xloc_ = xloc;
    }
    inline Loc yloc() const {
        return yloc_;
    }
    void ylocIs(Loc yloc) {
        this->yloc_ = yloc;
    }
    inline Loc xPeek() const {
        return xPeek_;
    }
    void xPeekIs(Loc xPeek) {
        this->xPeek_ = xPeek;
    }
    inline Loc yPeek() const {
        return yPeek_;
    }
    void yPeekIs(Loc yPeek) {
        this->yPeek_ = yPeek;
    }
    inline int active() const {
        return active_;
    }
    void activeIs(int active) {
        this->active_ = active;
    }
    inline Rat rat(RatIndexType num) const {
        return mazeRats_[num.value()];
    }
    void ratIs(Rat rat, RatIndexType num) {
        this->mazeRats_[num.value()] = rat;
    }
    /* Missile */
    inline bool hasMissile() const {
        return hasMissile_;
    }
    void hasMissileIs(bool hasMissile){
        this->hasMissile_=hasMissile;
    }
    inline Loc xMissile() const {
        return xMissile_;
    }
    void xMissileIs(Loc xMissile) {
        this->xMissile_ = xMissile;
    }
    inline Loc yMissile() const {
        return yMissile_;
    }
    void yMissileIs(Loc yMissile) {
        this->yMissile_ = yMissile;
    }
    inline Direction dirMissile() const {
        return dirMissile_;
    }
    void dirMissileIs(Direction dirMissile) {
        this->dirMissile_ = dirMissile;
    }
    inline timeval updateMissile() const {
        return updateMissile_;
    }
    void updateMissileIs(timeval updateMissile) {
        this->updateMissile_ = updateMissile;
    }
    inline std::vector<infoMis>& trackedMissile(){
        return trackedMissile_;
    }


    MazeType maze_;
    RatName myName_;
protected:
    MazewarInstance(string s) : Fwk::NamedInterface(s), 
    dir_(0), dirPeek_(0), 
    myRatId_(0), score_(0),
    xloc_(1), yloc_(3), xPeek_(0), yPeek_(0),
    xMissile_(0), yMissile_(0), dirMissile_(0), hasMissile_(false) {
        myAddr_ = (Sockaddr *)malloc(sizeof(Sockaddr));
        if(!myAddr_) {
            printf("Error allocating sockaddr variable");
        }
    }

    Direction	dir_;
    Direction dirPeek_;

    long mazePort_;
    Sockaddr *myAddr_;
    Rat mazeRats_[MAX_RATS];
    RatId myRatId_;

    bool peeking_;
    int theSocket_;
    Score score_;
    Loc xloc_;
    Loc yloc_;
    Loc xPeek_;
    Loc yPeek_;
    int active_;

    Loc xMissile_;
    Loc yMissile_;
    Direction dirMissile_;
    bool hasMissile_;
    timeval updateMissile_;
    std::vector<infoMis> trackedMissile_;

};
extern MazewarInstance::Ptr M;

#define MY_RAT_INDEX		0
#define MY_DIR			M->dir().value()
#define MY_X_LOC		M->xloc().value()
#define MY_Y_LOC		M->yloc().value()
#define MY_DIR_MIS		M->dirMissile().value()
#define MY_X_MIS        M->xMissile().value()
#define MY_Y_MIS        M->yMissile().value()

/* events */

#define	EVENT_A		1		/* user pressed "A" */
#define	EVENT_S		2		/* user pressed "S" */
#define	EVENT_F		3		/* user pressed "F" */
#define	EVENT_D		4		/* user pressed "D" */
#define	EVENT_BAR	5		/* user pressed space bar */
#define	EVENT_LEFT_D	6		/* user pressed left mouse button */
#define	EVENT_RIGHT_D	7		/* user pressed right button */
#define	EVENT_MIDDLE_D	8		/* user pressed middle button */
#define	EVENT_LEFT_U	9		/* user released l.M.b */
#define	EVENT_RIGHT_U	10		/* user released r.M.b */

#define	EVENT_NETWORK	16		/* incoming network packet */
#define	EVENT_INT	17		/* user pressed interrupt key */
#define	EVENT_TIMEOUT	18		/* nothing happened! */

extern unsigned short	ratBits[];

typedef	struct {
    short		                eventType;
    MW244BPacket   *eventDetail;	/* for incoming data */
    Sockaddr	                eventSource;
}MWEvent;

void		*malloc();
Sockaddr	*resolveHost();

/* display.c */
void InitDisplay(int, char **);
void StartDisplay(void);
void ShowView(Loc, Loc, Direction);
void SetMyRatIndexType(RatIndexType);
void SetRatPosition(RatIndexType, Loc, Loc, Direction);
void ClearRatPosition(RatIndexType);
void ShowPosition(Loc, Loc, Direction);
void ShowAllPositions(void);
void showMe(Loc, Loc, Direction);
void clearPosition(RatIndexType, Loc, Loc);
void clearSquare(Loc xClear, Loc yClear);
void NewScoreCard(void);
void UpdateScoreCard(RatIndexType);
void FlipBitmaps(void);
void bitFlip(BitCell *, int size);
void SwapBitmaps(void);
void byteSwap(BitCell *, int size);
void showMissile(Loc, Loc, Direction, Loc, Loc, bool);


/* init.c */
void MazeInit(int, char **);
void ratStates(void);
void getMaze(void);
void setRandom(void);
void getName(char *, char **);
void getString(char *, char **);
void getHostName(char *, char **, Sockaddr *);
Sockaddr *resolveHost(char *);
bool emptyAhead();
bool emptyRight();
bool emptyLeft();
bool emptyBehind();

/* toplevel.c */
void play(void);
void aboutFace(void);
void leftTurn(void);
void rightTurn(void);
void forward(void);
void backward(void);
void peekLeft(void);
void peekRight(void);
void peekStop(void);
void shoot(void);
void quit(int);
void NewPosition(MazewarInstance::Ptr M);
void MWError(char *);
Score GetRatScore(RatIndexType);
const char  *GetRatName(RatIndexType);
RatIndexType getRatIndexById(RatId id);
void ratState(void);
void manageHits(void);
void manageMissiles(void);
void DoViewUpdate(void);

bool isConflictPosition(Loc x, Loc y);
bool checkTimeout(timeval, long);
uint32_t generateId();
long timediff(timeval t1, timeval t2);

void sendPacket(mazePacket *);
void sendHeartbeat();
void sendKilled();

/* Packet processing routines */
void processPacket(MWEvent *);
void processHeartbeat(heartbeat *);
void processNameRequest(nameRequest *);
void processNameResponse(nameResponse *);
void processKilled(killed *);
void processKillConfirmed(killConfirmed *);
void processLeave(leave *);

void netInit(void);

/* winsys.c */
void InitWindow(int, char **);
void StartWindow(int, int);
void ClearView(void);
void DrawViewLine(int, int, int, int);
void NextEvent(MWEvent *, int);
bool KBEventPending(void);
void HourGlassCursor(void);
void RatCursor(void);
void DeadRatCursor(void);
void HackMazeBitmap(Loc, Loc, BitCell *);
void DisplayRatBitmap(int, int, int, int, int, int);
void WriteScoreString(RatIndexType);
void ClearScoreLine(RatIndexType);
void InvertScoreLine(RatIndexType);
void NotifyPlayer(void);
void DrawString(const char *, uint32_t, uint32_t, uint32_t);
void StopWindow(void);


class packetFactory{
    public:
    static mazePacket *createPacket(uint8_t type) {
        switch (type){
            case TYPE_HEARTBEAT:
               return new heartbeat;
            case TYPE_NAME_REQUEST:
              return new nameRequest; 
            case TYPE_NAME_RESPONSE:
              return new nameResponse;
            case TYPE_KILLED:
              return new killed;
            case TYPE_KILLCONFIRMED:
              return new killConfirmed;
            case TYPE_LEAVE:
              return new leave;
            default:
              MWError("bad packet type for factory"); 
        }
        return NULL;
    }
    
};

#endif

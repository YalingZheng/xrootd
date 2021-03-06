#ifndef __XRDOFSTPC_HH__
#define __XRDOFSTPC_HH__
/******************************************************************************/
/*                                                                            */
/*                          X r d O f s T P C . h h                           */
/*                                                                            */
/* (c) 2012 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <stdlib.h>
#include <string.h>
  
#include "XrdOfs/XrdOfsTPCInfo.hh"

class XrdAccAuthorize;
class XrdOfsTPCAllow;
class XrdOfsTPCJob;
class XrdOss;
class XrdOucEnv;
class XrdOucErrInfo;
class XrdOucPListAnchor;
class XrdOucTList;
class XrdSecEntity;

class XrdOfsTPC
{
public:

struct Facts
      {const char          *Key;
       const char          *Lfn;
       const char          *Pfn;
       const char          *Org;
       const char          *Dst;
       const XrdSecEntity  *Usr;
             XrdOucErrInfo *eRR;
             XrdOucEnv     *Env;

       Facts(const XrdSecEntity *vEnt, XrdOucErrInfo *vInf, XrdOucEnv *vEnv,
             const char *vKey, const char *vLfn, const char *vPfn=0)
            : Key(vKey), Lfn(vLfn), Pfn(vPfn), Org(0), Dst(0),
              Usr(vEnt), eRR(vInf), Env(vEnv) {}
      };

static  void  Allow(char *vDN, char *vGN, char *vHN, char *vVO);

static  int   Authorize(XrdOfsTPC **theTPC,
                        Facts      &Args,
                        int         isPLE=0);

virtual void  Del() {}

struct  iParm {char *Pgm;
               char *Ckst;
               int   Dflttl;
               int   Maxttl;
               int   Logok;
               int   Strm;
               int   Xmax;
                     iParm() : Pgm(0), Ckst(0), Dflttl(-1), Maxttl(-1),
                               Logok(-1), Strm(-1), Xmax(-1) {}
              };

static  void  Init(iParm &Parms);

static  void  Init(XrdAccAuthorize *accP) {fsAuth = accP;}

static  void  Init(XrdOss *ossP)          {OfsOss = ossP;}

static  const int reqALL = 0;
static  const int reqDST = 1;
static  const int reqORG = 2;

static  void  Require(const char *Auth, int RType);

static  int   Restrict(const char *Path);

static  int   Start();

virtual int   Sync(XrdOucErrInfo *error) {return 0;}

static  int   Validate (XrdOfsTPC **theTPC, Facts &Args);

              XrdOfsTPC() : Refs(1), inQ(0) {}

              XrdOfsTPC(const char *Url, const char *Org,
                        const char *Lfn, const char *Pfn, const char *Cks=0)
                       : Info(Url, Org, Lfn, Pfn, Cks), Refs(1), inQ(0) {}

virtual      ~XrdOfsTPC() {}

XrdOfsTPCInfo Info;

static char  *cksType;

protected:

static int    Fatal(Facts &Args, const char *eMsg, int eCode, int nomsg=0);
static int    genOrg(const XrdSecEntity *client, char *Buff, int Blen);
static int    getTTL(XrdOucEnv *Env);
static int    Screen(Facts &Args, XrdOucTList *tP, int wasEnc=0);
static char  *Verify(const char *Who,const char *Name,char *Buf,int Blen);

static XrdAccAuthorize   *fsAuth;
static XrdOss            *OfsOss;

static char              *XfrProg;

static XrdOucTList       *AuthDst;
static XrdOucTList       *AuthOrg;

static XrdOfsTPCAllow    *ALList;
static XrdOucPListAnchor *RPList;

static int                dflTTL;
static int                maxTTL;
static int                LogOK;
static int                nStrms;
static int                xfrMax;
static int                tpcOK;
static int                encTPC;

       char               Refs;      // Reference count
       char               inQ;       // Object in queue
};
#endif

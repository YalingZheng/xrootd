#ifndef __XRDXROOTDCALLBACK_H__
#define __XRDXROOTDCALLBACK_H__
/******************************************************************************/
/*                                                                            */
/*                  X r d X r o o t d C a l l B a c k . h h                   */
/*                                                                            */
/* (c) 2006 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include "XrdOuc/XrdOucErrInfo.hh"
#include "XrdSys/XrdSysPthread.hh"

class XrdScheduler;
class XrdOurError;
class XrdXrootdStats;

class XrdXrootdCallBack : public XrdOucEICB
{
public:

        void        Done(int           &Result,   //I/O: Function result
                         XrdOucErrInfo *eInfo,    // In: Error information
                         const char    *Path=0);  // In: Path related to call

        const char *Func() {return Opname;}

        const char  Oper() {return Opcode;}

        int         Same(unsigned long long arg1, unsigned long long arg2);

        void        sendError(int rc, XrdOucErrInfo *eInfo, const char *Path);

        void        sendResp(XrdOucErrInfo *eInfo,
                             XResponseType  xrt,       int  *Data=0,
                             const char    *Msg=0,     int   ovhd=0);

static  void        setVals(XrdSysError    *erp,
                            XrdXrootdStats *SIp,
                            XrdScheduler   *schp,
                            int             port)
                           {eDest=erp; SI=SIp; Sched=schp; Port=port;}

                    XrdXrootdCallBack(const char *opn, const char opc)
                                     : Opname(opn), Opcode(opc) {}

                   ~XrdXrootdCallBack() {}
private:
static XrdSysError        *eDest;
static XrdXrootdStats     *SI;
static XrdScheduler       *Sched;
       const char         *Opname;
       const char          Opcode;
static int                 Port;
};
#endif

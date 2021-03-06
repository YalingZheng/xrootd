#ifndef __FRMXFRJOB__
#define __FRMXFRJOB__
/******************************************************************************/
/*                                                                            */
/*                       X r d F r m X f r J o b . h h                        */
/*                                                                            */
/* (c) 2010 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include <sys/param.h>
#include "XrdFrc/XrdFrcRequest.hh"

class XrdFrcReqFile;
class XrdOucTList;

class XrdFrmXfrJob
{
public:

XrdFrmXfrJob  *Next;
XrdOucTList   *NoteList;
XrdFrcReqFile *reqFQ;
char          *reqFile;
XrdFrcRequest  reqData;
const char    *Type;
char           PFN[MAXPATHLEN+16];
int            pfnEnd;
int            RetCode;
int            qNum;
char           Act;
};
#endif

/******************************************************************************/
/*                                                                            */
/*                        X r d O u c M s u b s . c c                         */
/*                                                                            */
/* (c) 2006 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

//         $Id$

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <ctype.h>
#include <stdio.h>
#include <strings.h>

#include "XrdOuc/XrdOucMsubs.hh"

/******************************************************************************/
/*                               G l o b a l s                                */
/******************************************************************************/

const char *XrdOucMsubs::vName[vMax] = {0};
  
/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/
  
XrdOucMsubs::XrdOucMsubs(XrdOucError *errp)
{

   if (vName[0] == 0)
      {vName[0]     = "$";
       vName[vLFN]  = "$LFN";
       vName[vPFN]  = "$PFN";
       vName[vRFN]  = "$RFN";
       vName[vLFN2] = "$LFN2";
       vName[vPFN2] = "$PFN2";
       vName[vRFN2] = "$RFN2";
       vName[vFM]   = "$FMODE";
       vName[vOFL]  = "$OFLAG";
       vName[vUSR]  = "$USER";
       vName[vHST]  = "$HOST";
       vName[vTID]  = "$TID";
      }
    mText = 0; 
    mData[0] = 0; mDlen[0] = 0;
    numElem = 0;
    eDest = errp;
}

/******************************************************************************/
/*                            D e s t r u c t o r                             */
/******************************************************************************/

XrdOucMsubs::~XrdOucMsubs() 
{  int i;

   if (mText) free(mText);

   for (i = 0; i < numElem; i++) if (mDlen[i] < 0) free(mData[i]);
}
  
/******************************************************************************/
/*                                 P a r s e                                  */
/******************************************************************************/
  
int XrdOucMsubs::Parse(const char *dName, char *msg)
{
   char ctmp, *vp, *ip, *infop;
   int i, j = 0;

// Setup the additional stage information vector. Variable substitution:
// <data>$var<data>.... (max of MaxArgs substitutions)
//
   infop = mText = strdup(msg);
   while ((ip = index(infop, '$')) && j < maxElem)
         if (isalnum(*(ip+1)))
            {if ((mDlen[j] = ip-infop)) mData[j++] = infop;
             vp = ip; ip++;
             while(isalnum(*ip) || *ip == '.') ip++;
             ctmp = *ip; *ip = '\0';
             mDlen[j] = -(ip-vp);
             mData[j] = vp = strdup(vp); mData[j++]++;
             *ip = ctmp; infop = ip;
             if (isupper(*(vp+1)))
                for (i = 1; i <= vMax; i++)
                    if (!strcmp(vp, vName[i])) 
                       {mDlen[j-1] = i; mData[j-1] = 0; free(vp); break;}
            }

// Make sure we have not exceeded the array
//
   if (j < maxElem)
      {if ((mDlen[j] = strlen(infop))) mData[j++] = infop;
       numElem = j;
      } else {
       eDest->Emsg(dName, "Too many variables in", dName, "string.");
       return 0;
          }

// All done
//
   return 1;
}

/******************************************************************************/
/*                                  S u b s                                   */
/******************************************************************************/

int XrdOucMsubs::Subs(XrdOucMsubsInfo &Info, char **Data, int *Dlen)
{
   int k;

// Perform substitutions
//
   for (k = 0; k < numElem; k++)
       {     if (!mData[k])
                {Data[k] = getVal(Info, mDlen[k]);
                 Dlen[k] = strlen(Data[k]);
                }
        else if (mDlen[k] < 0)
                {if ((Data[k] = Info.Env->Get(mData[k])))
                    Dlen[k] = strlen(Data[k]);
                    else {Data[k]=mData[k]-1; Dlen[k]=(-mDlen[k]);}
                }
        else    {Data[k] = mData[k]; Dlen[k] = mDlen[k];}
       }
   return numElem;
}
  
/******************************************************************************/
/*                                g e t V a l                                 */
/******************************************************************************/
  
char *XrdOucMsubs::getVal(XrdOucMsubsInfo &Info, int vNum)
{
   char buff[1024];
   char *op;

   switch(vNum)
     {case vLFN:  return (char *)Info.lfn;

      case vPFN:  if (!Info.N2N) return (char *)Info.lfn;
                  if (Info.pfnbuff)    return Info.pfnbuff;
                  if (Info.N2N->lfn2pfn(Info.lfn,buff,sizeof(buff))) break;
                  Info.pfnbuff = strdup(buff);
                  return Info.pfnbuff;

      case vRFN:  if (!Info.N2N) return (char *)Info.lfn;
                  if (Info.rfnbuff)    return Info.rfnbuff;
                  if (Info.N2N->lfn2rfn(Info.lfn,buff,sizeof(buff))) break;
                  Info.rfnbuff = strdup(buff);
                  return Info.rfnbuff;

      case vPFN2: if (!Info.lfn2) break;
                  if (!Info.N2N) return (char *)Info.lfn2;
                  if (Info.pfn2buff)   return Info.pfn2buff;
                  if (Info.N2N->lfn2pfn(Info.lfn2,buff,sizeof(buff))) break;
                  Info.pfn2buff = strdup(buff);
                  return Info.pfn2buff;

      case vRFN2: if (!Info.lfn2) break;
                  if (!Info.N2N) return (char *)Info.lfn2;
                  if (Info.rfn2buff)   return Info.rfn2buff;
                  if (Info.N2N->lfn2rfn(Info.lfn2,buff,sizeof(buff))) break;
                  Info.rfn2buff = strdup(buff);
                  return Info.rfn2buff;

      case vFM:   sprintf(Info.mbuff, "%o", Info.Mode);
                  return Info.mbuff;

      case vOFL:  op = Info.obuff;
                  if (!(Info.Oflag & (O_WRONLY | O_RDWR)))  *op++ = 'r';
                     else {*op++ = 'w';
                           if (Info.Oflag & O_CREAT) *op++ = 'c';
                           if (Info.Oflag & O_EXCL)  *op++ = 'x';
                           if (Info.Oflag & O_TRUNC) *op++ = 't';
                          }
                  *op = '\0';
                  return Info.obuff;
      case vHST:  if ((op = Info.Env->Get(SEC_HOST))) return op;
                  break;
      case vUSR:  if ((op = Info.Env->Get(SEC_USER))) return op;
                  break;
      case vTID:  return (char *)Info.Tid;
      default:    return (char *)"$";
     }
   return (char *)vName[vNum];
}
/******************************************************************************/
/*                                                                            */
/*                    X r d P o s i x P r e l o a d . c c                     */
/*                                                                            */
/* (c) 2005 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

//           $Id$

#include <dirent.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream.h>

#include "XrdPosix/XrdPosixExtern.hh"
#include "XrdPosix/XrdPosixLinkage.hh"
#include "XrdPosix/XrdPosixStream.hh"
#include "XrdPosix/XrdPosixXrootd.hh"
 
/******************************************************************************/
/*                         L o c a l   C l a s s e s                          */
/******************************************************************************/
  
class XrdPosixPreloadEnv
{
public:
      XrdPosixPreloadEnv()
              {if (!getenv("XRDPOSIX_DEBUG")) XrdPosixXrootd::setDebug(-1);}
     ~XrdPosixPreloadEnv() {}
};

/******************************************************************************/
/*                   G l o b a l   D e c l a r a t i o n s                    */
/******************************************************************************/
  
extern XrdPosixLinkage    Xunix;

extern XrdPosixStream     streamX;
 
       XrdPosixRootVec    xinuX;

       XrdPosixPreloadEnv dummyENV;

/******************************************************************************/
/*                                a c c e s s                                 */
/******************************************************************************/
  
extern "C"
{
int access(const char *path, int amode)
{
  return xinuX.Access(path, amode);
}
}

/******************************************************************************/
/*                                 c l o s e                                  */
/******************************************************************************/

extern "C"
{
int     close(int fildes)
{
        return (fildes >= XrdPosixFD)
               ? xinuX.Close(fildes)
               : Xunix.Close(fildes);
}
}

/******************************************************************************/
/*                              c l o s e d i r                               */
/******************************************************************************/
  
extern "C"
{
int     closedir(DIR *dirp)
{
        return xinuX.Closedir(dirp);
}
}

/******************************************************************************/
/*                                 c r e a t                                  */
/******************************************************************************/
  
extern "C"
{
int     creat64(const char *path, mode_t mode)
{
        return xinuX.Open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}
}
  
/******************************************************************************/
/*                                f c l o s e                                 */
/******************************************************************************/
  
extern "C"
{
int fclose(FILE *stream)
{
  return streamX.Fclose(stream);
}
}

/******************************************************************************/
/*                                 f o p e n                                  */
/******************************************************************************/
  
extern "C"
{
FILE  *fopen64(const char *path, const char *mode)
{

  return xinuX.isMyPath(path)
         ? streamX.Fopen  (path, mode)
         :   Xunix.Fopen64(path, mode);
}
}
  
/******************************************************************************/
/*                                 f s t a t                                  */
/******************************************************************************/

extern "C"
{
#if defined __linux__ && __GNUC__ && __GNUC__ >= 2
int  __fxstat64(int ver, int fildes, struct stat64 *buf)
#else
int     fstat64(         int fildes, struct stat64 *buf)
#endif
{
        return (fildes >= XrdPosixFD)
               ? xinuX.Fstat  (     fildes, (struct stat *)buf)
#ifdef __linux__
               : Xunix.Fstat64(ver, fildes,                buf);
#else
               : Xunix.Fstat64(     fildes,                buf);
#endif
}
}

/******************************************************************************/
/*                                 f s y n c                                  */
/******************************************************************************/
  
extern "C"
{
int     fsync(int fildes)
{
        return (fildes >= XrdPosixFD) 
               ? xinuX.Fsync(fildes)
               : Xunix.Fsync(fildes);
}
}
  
/******************************************************************************/
/*                             f g e t x a t t r                              */
/******************************************************************************/
  
#ifdef __linux__
extern "C"
{
ssize_t fgetxattr (int fd, const char *name, void *value, size_t size)
{
  if (fd >= XrdPosixFD) {errno = ENOTSUP; return -1;}
  return Xunix.Fgetxattr(fd, name, value, size);
}
}
#endif

/******************************************************************************/
/*                              g e t x a t t r                               */
/******************************************************************************/
  
#ifdef __linux__
extern "C"
{
ssize_t getxattr (const char *path, const char *name, void *value, size_t size)
{
  if (xinuX.isMyPath(path)) {errno = ENOTSUP; return -1;}
  return Xunix.Getxattr(path, name, value, size);
}
}
#endif
  
/******************************************************************************/
/*                             l g e t x a t t r                              */
/******************************************************************************/
  
#ifdef __linux__
extern "C"
{
ssize_t lgetxattr (const char *path, const char *name, void *value, size_t size)
{
  if (xinuX.isMyPath(path)) {errno = ENOTSUP; return -1;}
  return Xunix.Lgetxattr(path, name, value, size);
}
}
#endif

/******************************************************************************/
/*                                 l s e e k                                  */
/******************************************************************************/
  
extern "C"
{
off_t   lseek64(int fildes, off_t offset, int whence)
{
        return (fildes >= XrdPosixFD) 
               ? xinuX.Lseek  (fildes, offset, whence)
               : Xunix.Lseek64(fildes, offset, whence);
}
}

/******************************************************************************/
/*                                l l s e e k                                 */
/******************************************************************************/
  
extern "C"
{
off_t   llseek(int fildes, off_t offset, int whence)
{
        return lseek64(fildes, offset, whence);
}
}

/******************************************************************************/
/*                                 l s t a t                                  */
/******************************************************************************/

extern "C"
{
#if defined __GNUC__ && __GNUC__ >= 2
int     __lxstat64(int ver, const char *path, struct stat64 *buf)
#else
int        lstat64(         const char *path, struct stat64 *buf)
#endif
{
        return xinuX.Lstat(path, (struct stat *)buf);
}

#if defined __GNUC__ && __GNUC__ >= 2
int        lstat64(         const char *path, struct stat64 *buf)
{
        return xinuX.Lstat(path, (struct stat *)buf);
}
#endif
}

/******************************************************************************/
/*                                 m k d i r                                  */
/******************************************************************************/
  
extern "C"
{
int     mkdir(const char *path, mode_t mode)
{
        return xinuX.Mkdir(path, mode);
}
}

/******************************************************************************/
/*                                  o p e n                                   */
/******************************************************************************/

extern "C"
{
int     open64(const char *path, int oflag, ...)
{
   va_list ap;
   int mode;

   va_start(ap, oflag);
   mode = va_arg(ap, int);
   va_end(ap);
   return xinuX.Open(path, oflag, mode);
}
}

/******************************************************************************/
/*                               o p e n d i r                                */
/******************************************************************************/
  
extern "C"
{
DIR*    opendir(const char *path)
{
        return xinuX.Opendir(path);
}
}
  
/******************************************************************************/
/*                                 p r e a d                                  */
/******************************************************************************/
  
extern "C"
{
ssize_t pread64(int fildes, void *buf, size_t nbyte, off_t offset)
{
        return (fildes >= XrdPosixFD) 
               ? xinuX.Pread  (fildes, buf, nbyte, offset)
               : Xunix.Pread64(fildes, buf, nbyte, offset);
}
}

/******************************************************************************/
/*                                  r e a d                                   */
/******************************************************************************/
  
extern "C"
{
ssize_t read(int fildes, void *buf, size_t nbyte)
{
   return fildes >= XrdPosixFD || (fildes = streamX.myFD(fildes)) >= XrdPosixFD
        ? xinuX.Read(fildes, buf, nbyte)
        : Xunix.Read(fildes, buf, nbyte);
}
}
  
/******************************************************************************/
/*                                 r e a d v                                  */
/******************************************************************************/
  
extern "C"
{
ssize_t readv(int fildes, const struct iovec *iov, int iovcnt)
{
        return (fildes >= XrdPosixFD) 
               ? xinuX.Readv(fildes, iov, iovcnt)
               : Xunix.Readv(fildes, iov, iovcnt);
}
}

/******************************************************************************/
/*                               r e a d d i r                                */
/******************************************************************************/

extern "C"
{
struct dirent64* readdir64(DIR *dirp)
{
       return (struct dirent64 *)xinuX.Readdir(dirp);
}
}

/******************************************************************************/
/*                             r e a d d i r _ r                              */
/******************************************************************************/
  
extern "C"
{
int     readdir64_r(DIR *dirp, struct dirent64 *entry, struct dirent64 **result)
{
        return xinuX.Readdir_r(dirp, (struct dirent *)entry, (struct dirent **)result);
}
}

/******************************************************************************/
/*                             r e w i n d d i r                              */
/******************************************************************************/

#ifndef rewinddir
extern "C"
{
void    rewinddir(DIR *dirp)
{
        xinuX.Rewinddir(dirp);
}
}
#endif

/******************************************************************************/
/*                                 r m d i r                                  */
/******************************************************************************/
  
extern "C"
{
int     rmdir(const char *path)
{
        return xinuX.Rmdir(path);
}
}

/******************************************************************************/
/*                               s e e k d i r                                */
/******************************************************************************/
  
extern "C"
{
void    seekdir(DIR *dirp, long loc)
{
        xinuX.Seekdir(dirp, loc);
}
}

/******************************************************************************/
/*                                  s t a t                                   */
/******************************************************************************/

extern "C"
{
#if defined __GNUC__ && __GNUC__ >= 2
int     __xstat64(int ver, const char *path, struct stat64 *buf)
#else
int        stat64(         const char *path, struct stat64 *buf)
#endif
{
        return xinuX.Stat(path, (struct stat *)buf);
}
}

/******************************************************************************/
/*                                p w r i t e                                 */
/******************************************************************************/
  
extern "C"
{
ssize_t pwrite64(int fildes, const void *buf, size_t nbyte, off_t offset)
{
        return (fildes >= XrdPosixFD) 
               ? xinuX.Pwrite(fildes, buf, nbyte, offset)
               : Xunix.Pwrite(fildes, buf, nbyte, offset);
}
}

/******************************************************************************/
/*                               t e l l d i r                                */
/******************************************************************************/
  
extern "C"
{
long    telldir(DIR *dirp)
{
        return xinuX.Telldir(dirp);
}
}

/******************************************************************************/
/*                                u n l i n k                                 */
/******************************************************************************/
  
extern "C"
{
int     unlink(const char *path)
{
        return xinuX.Unlink(path);
}
}

/******************************************************************************/
/*                                 w r i t e                                  */
/******************************************************************************/
  
extern "C"
{
ssize_t write(int fildes, const void *buf, size_t nbyte)
{
        return (fildes >= XrdPosixFD) 
               ? xinuX.Write(fildes, buf, nbyte)
               : Xunix.Write(fildes, buf, nbyte);
}
}

/******************************************************************************/
/*                                w r i t e v                                 */
/******************************************************************************/
  
extern "C"
{
ssize_t writev(int fildes, const struct iovec *iov, int iovcnt)
{
        return (fildes >= XrdPosixFD) 
               ? xinuX.Writev(fildes, iov, iovcnt)
               : Xunix.Writev(fildes, iov, iovcnt);
}
}
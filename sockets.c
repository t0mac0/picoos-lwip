/*
 * Copyright (c) 2015, Ari Suutari <ari@stonepile.fi>.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <picoos.h>
#include <picoos-u.h>
#include <picoos-lwip.h>
#include <string.h>

#if !LWIP_COMPAT_SOCKETS

#if !defined(UOSCFG_MAX_OPEN_FILES) || UOSCFG_MAX_OPEN_FILES == 0
#error UOSCFG_MAX_OPEN_FILES must be > 0
#endif

typedef struct {

  UosFS base;

} SockFS;

static SockFS sockFS;

static int sockInit(const UosFS*);
static int sockClose(UosFile* file);
static int sockRead(UosFile* file, char* buf, int max);
static int sockWrite(UosFile* file, const char* buf, int max);
static int sockFStat(UosFile* file, UosFileInfo* st);

static const UosFSConf sockFSConf = {
  
  .init   = sockInit,
};

static const UosFileConf sockConf = {

  .close  = sockClose,
  .read   = sockRead,
  .write  = sockWrite,
  .fstat  = sockFStat
};

void netInit()
{
  sockFS.base.mountPoint = "/socket";
  sockFS.base.cf = &sockFSConf;

  uosMount(&sockFS.base);

}
static int sockInit(const UosFS* fs)
{
  return 0;
}

int netLwIP_FD(int s)
{
  UosFile* file = uosSlot2File(s);
  if (file == NULL)
    return -1;

  P_ASSERT("lwipFD", file->fs->cf == &sockFSConf);
  return file->fsPrivFd;
}

int socket(int domain, int type, int protocol)
{
  UosFile* file = uosFileAlloc();
  if (file == NULL)
    return -1;

  int sock;

  sock = lwip_socket(domain, type, protocol);
  if (sock == -1) {

    uosFileFree(file);
    return -1;
  }

  file->fs       = &sockFS.base;
  file->cf       = &sockConf;
  file->fsPrivFd = sock;

  return uosFile2Slot(file);
}

int accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
  UosFile* file = uosFileAlloc();
  if (file == NULL)
    return -1;

  int sock;

  sock = lwip_accept(netLwIP_FD(s), addr, addrlen);
  if (sock == -1) {

    uosFileFree(file);
    return -1;
  }

  file->fs       = &sockFS.base;
  file->cf       = &sockConf;
  file->fsPrivFd = sock;

  return uosFile2Slot(file);
}

static int sockClose(UosFile* file)
{
  P_ASSERT("sockClose", file->fs->cf == &sockFSConf);
  int sock = file->fsPrivFd;

  lwip_close(sock);
  uosFileFree(file);
  return 0;
}

static int sockRead(UosFile* file, char *buf, int len)
{
  P_ASSERT("sockRead", file->fs->cf == &sockFSConf);
  int sock = file->fsPrivFd;

  return lwip_read(sock, buf, len);
}

static int sockWrite(UosFile* file, const char *buf, int len)
{
  P_ASSERT("sockWrite", file->fs->cf == &sockFSConf);
  int sock = file->fsPrivFd;

  return lwip_write(sock, buf, len);
}

static int sockFStat(UosFile* file, UosFileInfo* st)
{
  st->isDir = false;
  st->isSocket = true;
  st->size = 0;
  return 0;
}


#endif

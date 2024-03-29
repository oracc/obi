/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: fname.c,v 0.3 1997/09/08 14:50:04 sjt Exp s $
*/

#include <psd_base.h>
#include <fname.h>
#include <ctype128.h>

const char *
oracc_home ()
{
  static char *cached_home = NULL;
  if (NULL == cached_home)
    {
      cached_home = getenv ("ORACC");
      if (NULL == cached_home)
	cached_home = "/usr/local/oracc";
    }
  return cached_home;
}

/* this should really be called oracc_home but that function
   returns the ORACC env var */
const char *
oracc_project_home ()
{
  static char *cached_home = NULL;
  if (NULL == cached_home)
    {
      cached_home = getenv ("ORACC_HOME");
      if (NULL == cached_home)
	cached_home = "/usr/local/oracc";
    }
  return cached_home;
}

const char *
oracc_var ()
{
  static char *cached_var = NULL;
  if (NULL == cached_var)
    {
      cached_var = getenv ("ORACC");
      if (NULL == cached_var)
	cached_var = "/usr/local/oracc";
    }
  return cached_var;
}

char *
new_ext (char *fn, const char *fext, Boolean force)
{
  char *tmp;
  size_t len;

  if (NULL == fn || NULL == fext)
    return fn;

  slashes (fn);

  if (!strcmp (fn, "-"))
    fn = fname_from_prog(fn);

  /* does fext wrongly begin with a period? */
  if ('.' == *fext)
    ++fext;
  tmp = find_ext (fn);
  if (*tmp)
    {
      if (FALSE == force)
        return fn;
      else
        *(fn + (tmp - fn)) = '\0';
    }
  len = strlen(fn);
  if (*fext)
    {
      fn = realloc(fn, len + strlen(EXTSEP_STRING) + strlen(fext) + 1);
      strcat (fn, EXTSEP_STRING);
      strcat (fn, fext);
    }
  else
    {
      fn = realloc(fn, len + 1);
      fn[len] = '\0';
    }
  return fn;
}

char *
new_ext_no_alloc(char *oldfile,const char *ext)
{
  char *p;
  p = oldfile + strlen(oldfile);
  while (p > oldfile && p[-1] != '.')
    --p;
  if ((p-oldfile) > 1 && isalnum(p[-2]))
    strcpy(p-1,ext);
  else
    strcat(p,ext);
  return oldfile;
}

char *
find_ext(char *fn)
{
  char *tmp = strrchr (fn, '.');
  if (NULL == tmp || tmp == fn || *(tmp + 1) == DIRSEP_CHAR || *(tmp - 1) == '.')
    return fn + strlen (fn);
  else
    return tmp;
}

extern const char *prog;

char *
fname_from_prog(char *fn)
{
  size_t fn_len = min(strlen(fn), MAX_BASENAME_);
  fn = realloc(fn, fn_len + 1);
  strncpy (fn, prog, fn_len);
  fn[fn_len] = '\0';
  return fn;
}

void
slashes (register char *fn)
{
  while (*fn)
    if ('\\' == *fn)
      *fn++ = '/';
    else
      ++fn;
}

char *
new_path (char *fn, const char *path, Boolean force)
{
  char *tmp, *npath, *npathsep;

  if (NULL == fn || '\0' == *fn)
    return NULL;

  if (NULL == path)
    return filename (fn);

  if (!strcmp (fn, "-"))
    fn = fname_from_prog(fn);

  if (FALSE == force)
    {
      const char *p = find_path (fn);
      if (*p)
        return fn;
    }
  npath = xstrdup(path);
  slashes (npath);
  slashes (fn);
  fn = filename (fn);
  tmp = malloc (strlen(npath) + 1 + strlen(fn) + 1);
  npathsep = 
    (npath == NULL || *npath == '\0') 
    ? "" : (npath[strlen (npath) - 1] != DIRSEP_CHAR ? DIRSEP_STRING : "");
  sprintf (tmp, "%s%s%s", npath, npathsep, fn);
  free (npath);
  free (fn);
  return tmp;
}

char *
filename (char *path)
{
  const char *tmp;
  if (NULL == path)
    return NULL;
  slashes (path);
  tmp = find_path (path);
  if (*tmp)
    {
      if (tmp != path)
	{
	  char *save = path;
	  path = xstrdup (tmp+1);
	  free (save);
	}
    }
  return path;
}

char *
find_path (char *fn)
{
  char *p = xstrdup (fn), *end;
  slashes (p);
  if (DIRSEP_CHAR) /* paranoia: just in case DA is ever implemented on 
		      a flat file system */
    {
      end = strrchr (p, DIRSEP_CHAR);
    }
  else
    {
      end = NULL;
    }
  if (NULL == end)
    end = fn + strlen (fn);
  else
    end = fn + (end - p);
  free (p);
  return end;
}

const char *
preslash(const char *s, const char*slash)
{
  static char buf[128];
  if (slash - s > 127)
    return NULL;
  strncpy(buf,s,slash-s);
  buf[slash-s] = '\0';
  return buf;
}

const char *
expand (const char *project, const char *pqid, const char *ext)
{
  static char buf[_MAX_PATH];
  static const char*cdl;
  char projbuf[1024], prefix[5], *bp;

  if (!pqid || !ext)
    return NULL;
  if (!project)
    *projbuf = '\0';
  else
    strcpy(projbuf,project);
  cdl = cdl_home();
  bp = buf;
  strncpy(prefix,pqid,4);
  prefix[4] = '\0';
  sprintf(buf,"%s/bld/%s/%s/%s/%s.%s",cdl,project,prefix,pqid,pqid,ext);
  return buf;
}

char *
l2_expand (const char *project, const char *pqid, const char *ext)
{
  static char buf[_MAX_PATH];
  static const char*cdl;
  char projbuf[1024], prefix[5], *bp;
  const char*colon;

  *buf = '\0';
  if (!pqid || !ext)
    return NULL;
  if (!project)
    *projbuf = '\0';
  if ((colon = strchr(pqid, ':')))
    {
      strcpy(projbuf,pqid);
      projbuf[colon-pqid] = '\0';
      pqid = ++colon;
    }
  else if (project)
    strcpy(projbuf,project);
  if (!cdl)
    cdl = cdl_home();
  bp = buf;
  strncpy(prefix,pqid,4);
  prefix[4] = '\0';
  sprintf(buf,"%s/bld/%s/%s/%s/%s.%s",cdl,projbuf,prefix,pqid,pqid,ext);
  return buf;
}

/* code can be code only, e.g., 'project', or code-lang, e.g.,
   'project-en'; in the latter case lang should be NULL */
const char *
expand_xtr (const char *project, const char *pqid, const char *code, const char *lang)
{
  static char buf[_MAX_PATH];
  static const char*cdl;
  char projbuf[1024], prefix[5], *bp;

  if (!pqid || !code)
    return NULL;
  if (!project)
    *projbuf = '\0';
  else
    sprintf(projbuf,"%s",project);
  cdl = cdl_home();
  bp = buf;
  strncpy(prefix,pqid,4);
  prefix[4] = '\0';
  if (lang)
    sprintf(buf,"%s/bld/%s/%s/%s/%s_%s-%s.xtr",cdl,projbuf,prefix,pqid,pqid,code,lang);
  else
    sprintf(buf,"%s/bld/%s/%s/%s/%s_%s.xtr",cdl,projbuf,prefix,pqid,pqid,code);
  return buf;
}

char *
l2_expand_xtr (const char *project, const char *pqid, const char *code, const char *lang)
{
  static char buf[_MAX_PATH];
  static const char*cdl;
  char projbuf[1024], prefix[5], *bp, *colon;

  if (!pqid || !code)
    return NULL;

  if ((colon = strchr(pqid, ':')))
    {
      strcpy(projbuf,pqid);
      projbuf[colon-pqid] = '\0';
      pqid = ++colon;
    }
  else
    {
      if (!project)
	*projbuf = '\0';
      else
	sprintf(projbuf,"%s",project);
    }

  cdl = cdl_home();
  bp = buf;
  strncpy(prefix,pqid,4);
  prefix[4] = '\0';
  if (lang)
    sprintf(buf,"%s/bld/%s/%s/%s/%s_%s-%s.xtr",cdl,projbuf,prefix,pqid,pqid,code,lang);
  else
    sprintf(buf,"%s/bld/%s/%s/%s/%s_%s.xtr",cdl,projbuf,prefix,pqid,pqid,code);
  return buf;
}

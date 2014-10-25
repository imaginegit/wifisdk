
#include "sdio_card.h"
#include "tools.h"


/* character change lower case to capital */
char tolower(char c)
{
    if((c >= 'A') && (c <= 'Z'))
        return (c + 0x20);
    return c;
}

#if 0
/* character change captital to lower case.*/
char toupper(char c)
{
    if((c >= 'a') && (c <= 'z'))
        return (c - 0x20);
    return c;
}

void strshrinkpath(char *path)
{
  unsigned int i;

  for(i=strlen(path); i!=0; i--)
  {
    if(path[i] == '/')
    {
      break;
    }
  }
  path[i] = 0;

  return;
}
#endif


char* strrmvspace(char *dst, const char *src)//remove the space at the end of head and tail...
{
	unsigned int i;

	if(*src == 0)
	{
		*dst = 0;
	}
	else
	{
		//at start
		for(i=0; isspace(src[i]); i++);
		strcpy(dst, &src[i]);

		//at end
		i=strlen(dst)-1;
		for(i=strlen(dst)-1; isspace(dst[i]); i--)
		{
			dst[i] = 0;
		}
	}

	return dst;
}

#if 0
char* strtoupper(char *dst, const char *src)
{
  char c;

  while(*src)
  {
    c    = *src++;
    *dst++ = toupper(c);
  }
  *dst = 0;

  return dst;
}


int strstrk(char *dst, const char *src, const char *key) //key = "first\0second\third\0\0"
{
  int fnd=0;
  const char *k, *s;
  char c;

  *dst = 0;

  s = src;
  k = key;
  for(;;)
  {
    s = strstri(s, k);
    if(s != 0)
    {
      s += strlen(k);
      while(*s && (*s ==  ' ')){ s++; } //skip spaces
      while(*k){ k++; } k++; //get next key
      if(*k == 0) //last key
      {
        while(*s)
        {
          c = *s++;
          if((c == '<')  ||
             (c == '>')  ||
             (isspace(c)))
          {
            break;
          }
          else
          {
            *dst++ = c;
          }
        }
        *dst = 0;
        fnd  = 1;
        break;
      }
    }
    else
    {
      break;
    }
  }

  return fnd;
}
#endif


uint8 * strstri(uint8 *s, uint8 *pattern)
{
	size_t len;

	if (*pattern)
	{
		len = strlen(pattern);
		while (*s)
		{
			if (strncmpi(s, pattern, len) == 0)
			{
				return s;
			}
			s++;
		}
	}

	return 0;
}

int strncmpi(const char *s1, const char *s2, size_t n)
{
	unsigned char c1, c2;

	if(n == 0)
	{
		return 0;
	}

	do
	{
		c1 = tolower(*s1++);//if /parm is character then change it to lower,otherwise do not care.
		c2 = tolower(*s2++);
		if((c1 == 0) || (c2 == 0))
		{
			break;
		}
	}
	while((c1 == c2) && --n);

	return c1-c2;
}

#if 0
unsigned int atou_hex(const char *s)
{
  unsigned int value=0;

  if(!s)
  {
    return 0;
  }  

  while(*s)
  {
    if((*s >= '0') && (*s <= '9'))
    {
      value = (value*16) + (*s-'0');
    }
    else if((*s >= 'A') && ( *s <= 'F'))
    {
      value = (value*16) + (*s-'A'+10);
    }
    else if((*s >= 'a') && (*s <= 'f'))
    {
      value = (value*16) + (*s-'a'+10);
    }
    else
    {
      break;
    }
    s++;
  }

  return value;
}
#endif


unsigned int atou(const char *s)
{
	unsigned int value=0;

	if(!s)
	{
		return 0;
	}  

	while(*s)
	{
		if((*s >= '0') && (*s <= '9'))
		{
			value = (value*10) + (*s-'0');
		}
		else
		{
			break;
		}
		s++;
	}

	return value;
}

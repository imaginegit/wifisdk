#ifndef _TOOLS_H_
#define _TOOLS_H_


//----- PROTOTYPES -----
extern char                                   tolower(char c);
//extern char                                   toupper(char c);

//extern void                                   strshrinkpath(char *path);
extern char*                                  strrmvspace(char *dst, const char *src);
//extern char*                                  strtoupper(char *dst, const char *src);
//extern int                                    strstrk(char *dst, const char *src, const char *key);
extern uint8 *                            strstri(uint8 *s, uint8 *pattern);
extern int                                    strncmpi(const char *s1, const char *s2, size_t n);

//extern unsigned int                           atou_hex(const char *s);
extern unsigned int                           atou(const char *s);

#endif //_TOOLS_H_

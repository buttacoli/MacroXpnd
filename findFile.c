#include "findFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

extern const char* SystemPaths[];
extern const char** LocalPaths;


//! Joins two strings with a / character, and returns the dynamically
//! allocated string.  It becomes the clients responsibility to free 
//! the returned memory.
static char* join( const char* front, const char* back )
{
   int len = strlen(front) + strlen(back) + 2;
   char* prod = (char*) malloc( len );
   snprintf(prod,len,"%s/%s",front,back);
   return prod;
}
   

//! Note: stat() follows links --lstat does not.
static int exists( const char* path )
{
   struct stat st_buf;
   if ( lstat( path, &st_buf ) != 0 ) {
      return -1;
   }
   return S_ISREG( st_buf.st_mode );
}


//! Find a given file name within the search paths provided, the variable
//! length list MUST be null terminated by a zero for this to work properly.
static char* search_paths( const char* fn, const char** paths )
{
   char* cp = 0;
   char* rcp = 0;
   const char* path=0;
   while ( (path=*(paths++)) != 0 ) {
      cp = join( path, fn );
      if ( exists( cp ) ) {
         rcp = cp;
         break;
      }
      else
         free(cp);
   }
   return rcp;
}


FILE* findFile( const char* word )
{
   char* fn = 0;
   FILE* fp = 0;
   int len = 0;
   
   if ( word == 0 ) return 0;

   /* Set the file name after striping the first and last characters */
   fn = strdup(&word[1]);
   len = strlen(fn) - 1;
   if ( fn[len] == '\n' )   // wipe out extra new lines???
      fn[len--] = 0;
   fn[len] = 0;   /* null out the last char (" or >) */
   
   /* check if we should search system paths, or local paths based on the 
      type of string which was used in the #include */
   char *lfn = 0;
   if ( word[0] == '<' ) {
      lfn = search_paths( fn, SystemPaths );
   }
   else {
      const char* arr[] = { ".", 0 };
      lfn = search_paths( fn, arr );
   }
 
   /* check the filename and try to open the file */  
   if ( !lfn || (fp=fopen(lfn,"r")) == 0 ) {
      fprintf(stderr,"Unable to open: %s\n", fn );
   }
   
   /* free allocated memory */
   if ( lfn ) free(lfn);
   if ( fn ) free(fn);
   
   return fp;
}
        

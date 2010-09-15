#include <stdio.h>
#include <stdlib.h>
#include <popt.h>
#include <stdarg.h>


const char USAGE[] = "macroXpn [-I<dir> ...] <file|->";
const char HELP[] =
"Purpose: Expand all includes and macro values so you have a\n"
"         complete compilation unit and can track where macros\n"
"         are set and what they alter.\n"
"Arguments:\n"
"  -I<dir>         Path which can be searched beyond the system\n"
"                   default paths.\n";

const char* SystemPaths[] = {
   "/usr/include",
   "/usr/local/include",
   "/usr/lib/gcc/i686-redhat-linux/4.4.4/include",
   "/usr/include/c++/4.4.4",
   0
};

#define MAX_PATHS 100
char* LocalPaths[MAX_PATHS+1];
int numPaths = 0;

static char* errorString = 0;
extern FILE* yyin;


static int pushToLocalPaths( char* s )
{
   if ( numPaths >= MAX_PATHS )
      return -1;
   LocalPaths[numPaths++] = s;
   return 0;
}


//! Populate the errorString global variable.
static void putError(const char* fmt, ... )
{
   va_list args;
   va_start(args,fmt);
      if ( errorString ) free(errorString);
      errorString = (char*)malloc(256);  
      vsnprintf(errorString,255,fmt,args);
   va_end(args);
}



//! Fills the global arrays and option structures.
//! @param argc      Number of char* in the argv
//! @param argv      Character array of command line options
//! @returns a character pointer to the filename if all args could be found
//!          a null pointer if there are problems with the problem itself 
//!          left in the global errorString character pointer.
static char* getOptions(int argc, char** argv )
{
   char* fn = 0;
   int i;
   
   // check for no args
   if ( argc <= 0 ) {
      putError("Usage: %s\n",USAGE);
      return 0;
   }
   
   // check all args
   for( i=0; i<argc; ++i ) {
      if ( strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"--help") == 0 ) {
         putError("Usage: %s\n%s\n",USAGE,HELP);
         return 0;
      }
      else if ( strncmp(argv[i],"-I",2) == 0 ) {
         if ( pushToLocalPaths( &argv[i][2] ) == -1 ) {
            putError("Exceeded the maximum number of local paths of %d\n",MAX_PATHS);
            return 0;
         }
      }
      else if ( fn==0 ) {
         fn = argv[i];
      }
      else {
         putError("Bad option: %s\n",argv[i]);
         return 0;
      }
   }
   if ( fn == 0 )
      putError("No filename argument found!\n");
   
   return fn;
}
 

int main( int argc, char** argv )
{   
   char* filename = 0;

   // Read the args
   if ( (filename=getOptions(argc-1,&argv[1])) == 0 ) {
      fprintf(stderr,"%s\n",errorString);;
      exit(-1);
   }
   
   if ( (yyin=fopen(filename,"r")) == 0 ) {
      fprintf(stderr,"Unable to open: %s\n", filename );
      exit(1);
   }
  
   return yylex();
}


%{
#include <stdio.h>
#include "findFile.h"

YY_BUFFER_STATE stack[10];
int stack_ptr = 0;


int push(const char*);

%}

%option noyywrap
%x INCL

%%
^#[ \t]*include[ \t]+   { ECHO;
                          BEGIN INCL;
                        }
<INCL>[^\n]+            { ECHO;
                          push(yytext); 
                        }
<<EOF>>                 { if ( pop() < 0 ) 
                             yyterminate(); 
                        }
%%

/**
int main(int argc, char** argv )
{
   return yylex();
}
*/

int push( const char* fn ) {
  FILE* fp = findFile( fn );
  if ( fp && stack_ptr < 9 ) {
     stack[stack_ptr++] = YY_CURRENT_BUFFER;
     YY_BUFFER_STATE newSt = yy_create_buffer(fp,YY_BUF_SIZE);
     yy_switch_to_buffer( newSt );
  }
  BEGIN INITIAL; 
  return stack_ptr;
}

int pop() {
  if ( stack_ptr > 0 ) {
     yy_delete_buffer( YY_CURRENT_BUFFER );
     yy_switch_to_buffer( stack[stack_ptr-1] );
  }
  return --stack_ptr;
}


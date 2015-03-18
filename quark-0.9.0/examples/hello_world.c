#include <string.h>
#include <stdlib.h>

#include "quark.h"

/* A task printing a character from a string */
/* Printed character is x'ed out, changing the string */
void hello_world_task( Quark *quark ) {
  int idx; char *str; 
  quark_unpack_args_2( quark, idx, str );
  printf( "%c - from the thread rank %d\n", str[idx], QUARK_Thread_Rank(quark) );
  str[idx] = 'x';
}


void test1( Quark *quark ) {
  int idx;
  char str[] = "Hello World";
  printf( "Initial contents of string: %s\n", str );
  printf( "Each QUARK task will print off a character from string, and replace it with an x.\n" );
  printf( "The dependencies are all INPUT, so all the tasks can run in parallel.\n" );
  for ( idx=0; idx<strlen(str); idx++ ) {
      QUARK_Insert_Task( quark, hello_world_task, NULL,
                         sizeof(int), &idx, VALUE, 
                         strlen(str)*sizeof(char), str, INPUT, 
                         0 );
  }
  QUARK_Barrier( quark );
  printf( "Final contents of string: %s\n\n", str );
}

void test2( Quark *quark ) {
  int idx;
  char str[] = "Hello World";
  printf( "Initial contents of string: %s\n", str );
  printf( "Each QUARK task will print off a character from string, and replace it with an x.\n" );
  printf( "The dependencies are all INOUT, so all the tasks are serialized.\n" );
  printf( "Because of locality and serialization, the tasks will all run on one thread.\n" );
  for ( idx=0; idx<strlen(str); idx++ ) {
      QUARK_Insert_Task( quark, hello_world_task, NULL,
                         sizeof(int), &idx, VALUE, 
                         strlen(str)*sizeof(char), str, INOUT, 
                         0 );
  }
  QUARK_Barrier( quark );
  printf( "Final contents of string: %s\n\n", str );
}

main() {
  Quark *quark = QUARK_New( 3 );
  printf( "QUARK is going to use 3 threads.\n\n" );
  test1( quark );
  test2( quark );
  QUARK_Delete( quark );
}  

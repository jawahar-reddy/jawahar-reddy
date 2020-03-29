#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
void* student_actions( void* student_id );
void* ta_actions();

#define no_of_chairs_for_waiting 3
#define no_of_students 4

sem_t sem_students;
sem_t sem_ta;
pthread_mutex_t mutex_thread;

int waiting_hall_chairs[3];
int number_students_waiting = 0;
int next_seating_position = 0;
int next_teaching_position = 0;
int ta_sleeping = 0;

int main( int arg1, char **arg2 ){

int i;
int student_num;

if (arg1 > 1 )
{
if ( isNumber( arg2[1] ) == 1) {
student_num = atoi( arg2[1] );
}
else {
printf("Invalid input");
return 0;
}
}
else {
student_num = no_of_students;
}
int student_ids[student_num];
pthread_t students[student_num];
pthread_t ta;

sem_init( &sem_students, 0, 0 );
sem_init( &sem_ta, 0, 1 );

//Create threads.
pthread_mutex_init( &mutex_thread, NULL );
pthread_create( &ta, NULL, ta_actions, NULL );
for( i = 0; i < student_num; i++ )
{
student_ids[i] = i + 1;
pthread_create( &students[i], NULL, student_actions, (void*) &student_ids[i] );
}

//Join threads
pthread_join(ta, NULL);
for( i =0; i < student_num; i++ )
{
pthread_join( students[i],NULL );
}

return 0;
}

void* ta_actions() {
printf( "Checking for students.\n" );
while( 1 ) {
//if students are waiting
if ( number_students_waiting > 0 ) {
ta_sleeping = 0;
sem_wait( &sem_students );
pthread_mutex_lock( &mutex_thread );
int help_time = rand() % 5;
//TA helping student.
printf( "Helping a student for %d seconds, no of Students waiting = %d.\n", help_time, (number_students_waiting - 1) );
printf( "Student %d receiving help.\n",waiting_hall_chairs[next_teaching_position] );
waiting_hall_chairs[next_teaching_position]=0;
number_students_waiting--;
next_teaching_position = ( next_teaching_position + 1 ) % no_of_chairs_for_waiting;

sleep( help_time );

pthread_mutex_unlock( &mutex_thread );
sem_post( &sem_ta );

}
//if no students are waiting
else {

if ( ta_sleeping == 0 ) {

printf( "No students are waiting TA takes nap\n" );
ta_sleeping = 1;
}
}
}
}

void* student_actions( void* student_id ) {
int id_student = *(int*)student_id;
while( 1 )
{
//if student is waiting, continue waiting
if ( isWaiting( id_student ) == 1 ) { continue; }
//student is programming.
int time = rand() % 5;
printf( "\tStudent %d is programming for %d seconds.\n", id_student, time );
sleep( time );
pthread_mutex_lock( &mutex_thread );
if( number_students_waiting < no_of_chairs_for_waiting ) {
waiting_hall_chairs[next_seating_position] = id_student;
number_students_waiting++;
//student takes a seat in the hallway.
printf( "\t\tStudent %d takes a seat in the hallway, Students waiting = %d.\n", id_student, number_students_waiting );
next_seating_position = ( next_seating_position + 1 ) % no_of_chairs_for_waiting;
pthread_mutex_unlock( &mutex_thread );

//wake TA if sleeping
sem_post( &sem_students );
sem_wait( &sem_ta );

}
else {
pthread_mutex_unlock( &mutex_thread );
//No chairs available then Student will try later.
printf( " no chairs are available in hallway Student %d will try later.\n",id_student );
}

}

}
int isNumber(char number[])
{
int i;
for ( i = 0 ; number[i] != 0; i++ )
{
if (!isdigit(number[i]))
return 0;
}
return 1;
}
int isWaiting( int student_id ) {
int i;
for ( i = 0; i < 3; i++ ) {
if ( waiting_hall_chairs[i] == student_id ) { return 1; }
}
return 0;
}


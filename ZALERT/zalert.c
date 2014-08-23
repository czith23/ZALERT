#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_ALERTS 100
#define WEB_FILE "/home/cjz32/public_html/zalert"

struct alert {
   struct tm * alert_time;
   char * title;
   char * email;
};

struct alert * alert_def = NULL;
struct alert alert_defs[MAX_ALERTS];
int alert_count = 0;

void usage() {
   printf("Invalid command format\n");
   printf("./zalert <filename>\n");
   printf("or \n");
   printf("./zalert <month[0-11]> <day[1-31]> <hour[0-23]> <minute[0-59]> <title> <email>\n");
}

void * alert_manager() {
   int done = 0;
   time_t alert_time_epoch;
   alert_time_epoch = mktime(alert_def->alert_time);
   while (done==0) {
      printf("now %lld\n",(long long)time(NULL));
      printf("alert %lld\n",(long long)alert_time_epoch);

      if (time(NULL) > alert_time_epoch) {
         char mailer_cmd[100];
         sprintf(mailer_cmd,"mail -s %s %s < alert_message",alert_def->title,alert_def->email);
	 system(mailer_cmd);
         done = 1;
      }
      usleep(1000);
   }
}

void * multiple_alert_manager() {
}

void parse_alert_file(char * filename) {
   
}

void alert_def_print(struct alert * alert_def, FILE * f) {
   return; 
   fprintf(f,"\"month\":%d,",alert_def->alert_time->tm_mon);
   fprintf(f,"\"day\":%d,",alert_def->alert_time->tm_mday);
   fprintf(f,"\"hour\":%d,",alert_def->alert_time->tm_hour);
   fprintf(f,"\"minute\":%d,",alert_def->alert_time->tm_min);
   fprintf(f,"\"title\":%s,",alert_def->title);
   fprintf(f,"\"email\":%s",alert_def->email);
}

void * json_printer() {
   int i;
   FILE *f = fopen(WEB_FILE,"wb");
   if (f==NULL)
      printf("File failed to open\n");
      return;
   printf("print access");
   fprintf(f,"[");
   for (i=0;i<alert_count;i++) {
      fprintf(f,"{");
      if (i+1 == alert_count) {
         alert_def_print(&alert_defs[i],f); 
         fprintf(f,"}");   
      }
      else {
         alert_def_print(&alert_defs[i],f); 
         fprintf(f,"},");
      }
   }
   fprintf(f,"]");
}

int main(int argc, char ** argv) {
   struct tm alert_time;

   // File
   if (argc==2) {
      parse_alert_file(argv[1]);
   }
   // Manual entry
   else if (argc==7) {
      alert_time.tm_year = 114; // This needs to be updated for scaled versions
      alert_time.tm_mon = atoi(argv[1]);
      alert_time.tm_mday = atoi(argv[2]);
      alert_time.tm_hour = atoi(argv[3]);
      alert_time.tm_min = atoi(argv[4]);
      alert_time.tm_sec = 0;
      char * title = argv[5];
      char * email = argv[6];
      alert_def = (struct alert *) malloc(sizeof(struct alert));
      alert_def->alert_time = &alert_time;
      alert_def->title = title;
      alert_def->email = email;
      alert_defs[alert_count] = *alert_def;
      pthread_t alert_thread;
      pthread_t json_thread;
      if (pthread_create(&alert_thread,NULL,alert_manager,NULL)) {
         printf("Alert thread failed to start. Try again.");
         exit(1);
      }

      if (pthread_create(&json_thread,NULL,json_printer,NULL)) {
         printf("JSON printing thread failed to start/ Try again.");
         exit(1);
      }

      pthread_join(alert_thread,NULL);
      pthread_join(json_thread,NULL);
   }
   else {
      usage();
   }

   if (alert_def!=NULL)
      free(alert_def);
   return 0;
}



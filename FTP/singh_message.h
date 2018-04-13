#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <sys/select.h>

#define max_message_size 1024
using namespace std;

struct message_t;
void to_str(char **,int,char*,char*,char*,char*,char*);
void to_str(char **,message_t);
void to_msg(message_t&,int,char*,char*,char*,char*);
void to_msg(message_t&,char*);
void itoa(char **,int);
int send_message(int,message_t);
int send_message(int,char*);
int receive_message(int,message_t&,char**);

struct message_t
{
  int service_type;     // range from 0-9 only
  char client_name[100];
  char message[100];
  char read_pipe[100];
  char special_field[100];
  message_t(){}
  message_t(int service_type,char* client_name,char* message=NULL,char* read_pipe=NULL,char* special_field=NULL)
  {
    this->service_type = service_type;
    strcpy(this->client_name,client_name);
    strcpy(this->message,message);
    strcpy(this->read_pipe,read_pipe);
    strcpy(this->special_field,special_field);
  }
  message_t clone()
  {
    message_t m;
    m.service_type = this->service_type;
    strcpy(m.client_name,this->client_name);
    strcpy(m.message,this->message);
    strcpy(m.read_pipe,this->read_pipe);
    strcpy(m.special_field,this->special_field);
    return m;
  }
  void print()
  {
    cout<<"\n****************************************************************************\n";
    cout<<"\nService Type : "<<this->service_type;
    cout<<"\nClient Name : "<<this->client_name;
    cout<<"\nMessage : "<<this->message;
    cout<<"\nRead Pipe : "<<this->read_pipe;
    cout<<"\nSpecial Field : "<<this->special_field<<endl;
    cout<<"\n****************************************************************************\n";
  }
  void print(ostream &fout)
  {
    fout<<"\n****************************************************************************\n";
    fout<<"\nService Type : "<<this->service_type;
    fout<<"\nClient Name : "<<this->client_name;
    fout<<"\nMessage : "<<this->message;
    fout<<"\nRead Pipe : "<<this->read_pipe;
    fout<<"\nSpecial Field : "<<this->special_field<<endl;
    fout<<"\n****************************************************************************\n";
  }
};

void to_str(char **str_ptr,int service_type,char *client_name,char *message=NULL,char* read_pipe=NULL,char *special_field=NULL)
{
  *str_ptr = new char[max_message_size];
  *str_ptr[0]=service_type + 48;
  strcat(*str_ptr,"|");
  strcat(*str_ptr,client_name);
  strcat(*str_ptr,"|");
  strcat(*str_ptr,message);
  strcat(*str_ptr,"|");
  strcat(*str_ptr,read_pipe);
  strcat(*str_ptr,"|");
  strcat(*str_ptr,special_field);
  strcat(*str_ptr,"|");
  strcat(*str_ptr,"\0");
}

void to_str(char** str_ptr,message_t msg)
{
  cout<<"\na1\n";
  cout<<"\na1\n";
  *str_ptr  = new char[max_message_size];
  cout<<"\na12\n";
  cout<<"\na12\n";
  *str_ptr[0] = msg.service_type + 48;
  cout<<"\na13\n";
  cout<<"\na13\n";
  strcat(*str_ptr,"|");
  cout<<"\na14\n";
  cout<<"\na14\n";
  strcat(*str_ptr,msg.client_name);
  cout<<"\na15\n";
  cout<<"\na15\n";
  strcat(*str_ptr,"|");
  cout<<"\na16\n";
  cout<<"\na16\n";
  strcat(*str_ptr,msg.message);
  cout<<"\na17\n";
  cout<<"\na17\n";
  strcat(*str_ptr,"|");
  cout<<"\na1\n";
  cout<<"\na1\n";
  strcat(*str_ptr,msg.read_pipe);
  strcat(*str_ptr,"|");
  strcat(*str_ptr,msg.special_field);
  strcat(*str_ptr,"|");
  strcat(*str_ptr,"\0");
  cout<<"\na1000000000000\n";
  cout<<"\na1000000000000\n";
}

void to_msg(message_t &msg,int service_type,char *client_name,char* message,char *read_pipe,char *special_field)
{
  char *str = new char[max_message_size];
  to_str(&str,service_type,client_name,message,read_pipe,special_field);
  to_msg(msg,str);
}

void to_msg(message_t &msg,char* str)
{
  int i=0,j=0;
  cout<<"\n1";
  cout<<"\nString is : "<<str<<endl;
  cout<<"\n1";
  char temp[max_message_size];
  msg.service_type = str[0]-48;
  i=2;
  while(str[i]!='|')
  {
    temp[j++]=str[i++];
  }
  temp[j]='\0';
  i++;
  strcpy(msg.client_name,temp);
  j=0;
  cout<<"\n12";
  cout<<"\n12";
  while(str[i]!='|')
  {
    temp[j++]=str[i++];
  }
  temp[j]='\0';
  i++;
  strcpy(msg.message,temp);
  j=0;
  cout<<"\n13";

  cout<<"\n13";

  while(str[i]!='|')
  {
    temp[j++]=str[i++];
  }
  temp[j]='\0';
  i++;
  j=0;
  strcpy(msg.read_pipe,temp);
  cout<<"\n14";

  cout<<"\n14";

  while(str[i]!='|')
  {
    temp[j++]=str[i++];
  }
  temp[j]='\0';
  cout<<"\n15";

  cout<<"\n15";

  strcpy(msg.special_field,temp);
   cout<<"\n16";

  cout<<"\n16";
  cout<<"\nReturned\n";
}

void itoa(char **str_ptr,int num)
{
  *str_ptr = new char[max_message_size];
  cout<<"\nEnter number : ";
  cin>>*str_ptr;
  return;
  cout<<"\nnum = "<<num<<endl;
  cout<<"\nitoa started\n";
  cout<<"\nitoa started\n";
  char *str = new char[max_message_size];
  cout<<"\nStarting loop\n";
  cout<<"\nStarting loop\n";
  int i = 0;
  while(num)
  {
    str[i] =(char)( num%10 + 48);
    num /= 10;
    i++;
  }
  cout<<"\nAfter loop\n";
  cout<<"\nAfter loop\n";
  str[i]='\0';
  cout<<"\ndirect string is = "<<*str_ptr<<endl;
  cout<<"\ndirect string is = "<<*str_ptr<<endl;
  int len = strlen(str);
  for(int j=len/2,k=((len+2)/2 - len%2);j>=0;j--,k++)
  {
    char temp=str[j];
    str[j]=str[k];
    str[k]=temp;
  }
  cout<<"\nafter string is = "<<*str_ptr<<endl;
  cout<<"\nafter string is = "<<*str_ptr<<endl;
}

int send_message(int fd,message_t msg)
{
  cout<<"\nSendig msg 1\n";
  cout<<"\nSendig msg 1\n";
  char *temp = new char[100];
  cout<<"\nSendig msg 2\n";
  cout<<"\nSendig msg 2\n";
  to_str(&temp,msg);
  cout<<"\nSendig msg\n";
  cout<<"\nSendig msg\n";
  return write(fd,temp,max_message_size-1);
}

int send_message(int fd,char *str_msg)
{
  return write(fd,str_msg,strlen(str_msg)+1);
}

int receive_message(int fd,message_t &msg,char **str_msg)
{
  cout<<"\nReceiving messasge\n";
  cout<<"\nReceiving messasge\n";
  int bytes = read(fd,*str_msg,max_message_size+1);
  cout<<"\nMessage received is of "<<bytes<<" bytes.";
  cout<<"\nMessage received is of "<<bytes<<" bytes.";
  to_msg(msg,*str_msg);
  cout<<"\nmessage converted\n";
  cout<<"\nmessage converted\n";
  return bytes;
}


void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        swap(*(str+start), *(str+end));
        start++;
        end--;
    }
}

// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

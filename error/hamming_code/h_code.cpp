#include <bits/stdc++.h>

using namespace std;

int main()
{
  string data;
  cout<<"\nEnter data : ";
  cin>>data;
  string with_blanks="";
  int pow_2 = 1;
  for(int pos=0,i=0;i<data.length();pos++)
  {
    if(pow_2-1 == pos)
    {
      with_blanks += "*";
      pow_2 *= 2;
    }
    else
      with_blanks +=data[i++];
  }
  cout<<"\n"<<with_blanks<<endl;
  data = with_blanks;
  // now completing code
  int skip = 1;
  for(int i=0;i<data.length();i++)
  {
    if(data[i]=='*')
    {
      int sum = 0;
      for(int  j=i;j<data.length();)
      {
        int count = 0;
        while(count != skip)
        {
          if(data[j]!='*')
          sum += data[j]-'0';
          count++;
          j++;
        }
        j += skip;
      }
      if(sum % 2 == 0)
        data[i]='0';
      else
        data[i] = '1';
      skip *= 2;
    }
  }
  cout<<"\nAns = "<<data<<endl;
  cout<<"\nEnter data for checking correctness.\n";
  cin>>data;
  bool flag = true;
  int incorrec_pos = 0;
  for(int i=1;i<data.length();i*=2)
  {
    int sum = 0;
    for(int  j=i-1;j<data.length();)
    {
      int count = 0;
      while(count != i)
      {
        sum += data[j]-'0';
        count++;
        j++;
      }
      j += i;
    }
    if(sum%2 != 0)
    {
      flag = false;
      incorrec_pos += i;
    }
  }
  if(flag)
  {
    cout<<"\nCorrect\n";
  }
  else
  {
    cout<<"\nIncorrect bit : "<<incorrec_pos<<" [ index starting from 1]"<<endl;
  }
  return 0;
}

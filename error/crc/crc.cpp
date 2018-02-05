#include <bits/stdc++.h>

using namespace std;

string divisor,data;

string sub(string a,string b)
{
  for(int i=0;i<a.length();i++)
  {
    if(a[i]>b[i])
    {
      break;
    }
    else if(b[i]>a[i])
    {
      swap(a,b);
      break;
    }
  }
  string ans="";
  int borrow = 0;
  for(int i=a.length()-1;i>=0;i--)
  {
    int temp_b = 0;
    a[i] -= borrow;
    if(a[i] < b[i])
    {
      a[i] += 2;
      temp_b = 1;
    }
    borrow = temp_b;
  }
  for(int i=0;i<a.length();i++)
  {
    ans += (char)(a[i]-b[i]+48);
  }
  return ans;
}

string divison(string data,string divisor,int flag)
{
  if(flag)
  for(int i=0;i<divisor.length()-1;i++)
  {
    data += "0";    // padding zeros to data
  }
  //cout<<"\nActual data = "<<data<<endl;
  int pos = divisor.length();
  string ans="";
  string part_a = data.substr(0,divisor.length());
  //cout<<"\ninitially part_a "<<part_a<<endl;
  for(int i=pos;i<data.length()+1;i++)
  {
    if(part_a[0]>'0')
    {
      ans+="1";
      //cout<<part_a <<" - "<<divisor<<" = "<<sub(part_a,divisor)<<endl;
      part_a = sub(part_a,divisor);
    }
    else
    {
        ans+="0";
        //cout<<part_a<<" - 0 = "<<part_a<<endl;
    }
    //left shift
    for(int j=0;j<part_a.length()-1;j++)
      part_a[j]=part_a[j+1];
    part_a[part_a.length()-1]=data[i];
    //cout<<part_a<<endl;
  }
  return part_a;
}

int main()
{
  // string a,b;
  // cin>>a>>b;
  // cout<<sub(a,b)<<endl;
  // return 0;
  string temp_data;
  cout<<"\nEnter divisor : ";
  getline(cin,divisor);
  cout<<"\nEnter data : ";
  getline(cin,data);
  temp_data = data;
  cout<<"\nCoded data is : "<<temp_data<<divison(data,divisor,1)<<endl;
  cout<<"\nEnter coded data to check : ";
  getline(cin,data);
  cout<<divison(data,divisor,0)<<endl;
  return 0;
}

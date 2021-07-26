#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <sys/mman.h>
#include <fstream>  
#include <sys/stat.h>
#include <stdlib.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// RPI class list API
// Author: Yi Yang
// 7/26/21

using namespace std;

const vector<string> keywds1 = {"table","\\table"};
const vector<string> keywds2 = {"tr","\\tr"};
const vector<string> keywds3 = {"td","\\td"};
const vector<string> keywds4 = {">","<"};

vector<string> keywds5 ;



template<typename T>
void pop_front(std::vector<T>& vec)
{
    vec.erase(vec.begin());
}


template<typename T>
void vec_lookahead(uint8_t* infile,vector<char>*buf,T* i){
  pop_front(*buf);
  *i+=1;
  buf->push_back(infile[*i]);
}

class mat{
  public:
    vector<string> wds;
    uint8_t*data;
    int indx1=-1;
    unsigned long int ender=0;
    int indx2=-1;
    mat(uint8_t*dt){
      data=dt;
    }
    long int last_buf;
    vector<char> out_ary;

    void matcher (vector<char>&buf, unsigned long int indx,vector<string> keywds){
      unsigned long int ct=0;
      last_buf = (long int)&buf;
      for (int b=0;b<keywds.size(); b++){   
        ct=0;
        for(unsigned long int i=0;i<keywds[b].size();i++){
          // if (indx1==0&&i!=0)return;
          // if (indx1!=0&&i==0)continue;
          if((int)((char)keywds[b][i]^(char)buf[i])==0)ct++;  
          if (ct>=keywds[b].size()){
            // printf("Matched: %s, index: %ld\n",buf.data(),indx);
            if(indx1==-1) indx1=indx;
            else if(indx2==-1) {
              indx2=indx;
              ender=keywds[b].size();
            }
          }    
        }    
      }
    }

    vector<char> get_ary()
    {   

      return out_ary ;
    }

    int pr_btwn (uint8_t* buf){
      if(wds.size()!=2){
        printf("No matching keywords exiting...");
        exit(0);
      }
      if(indx1!=-1 && indx2!=-1){
        for (unsigned long int i=indx1;i< indx2;i++){
          // cout<<buf[i]<<flush;
         if (buf[i]=='\n'){  break;}
          out_ary.push_back(buf[i]);
        }
        out_ary.push_back(',');

        out_ary.push_back('\n');
        indx1=-1;
        indx2=-1;
        return 1;
      }
      return 0;
    }
    void refine1(vector<char>buff, vector<string>keywds, unsigned long int sz){
      // for (unsigned int i=0;i<11;i++)buff.push_back(infile[i]);
      wds=keywds;
      unsigned long int idx=0;
      if (keywds.size()==0)return;
      for (unsigned int i=0;i<sz-keywds[0].size();i++){
        matcher(buff,i,keywds);
        if (pr_btwn(data)){ 
        }
        vec_lookahead(data,&buff,&idx);
      }
    }



};

int match_key(string infile){
  
  vector<char>buff;
  struct stat sb;
  long int fd;
  uint8_t * data;
  fd = open(infile.data(), O_RDONLY); // Reading file
  if (fd == -1) {
    perror("Cannot open file ");
    cout << infile << " ";
    exit(1);
  }
  fstat(fd, & sb);
  data = (uint8_t * )(mmap( 0, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
  mat m(data);
  for (unsigned int i=0;i<11;i++)buff.push_back(infile[i]);



  m.refine1(buff,keywds5,sb.st_size);

  vector<char>ary= m.get_ary();
  for (unsigned int i=0;i<ary.size();i++){
    cout<<ary[i];
  }
  return 1;
}

int main (int argc, char** argv){
  if(argc<2){
    printf("Need 1 input, exiting...\n");
    return EXIT_FAILURE;
  }
  string infile= argv[1];
  if (argc==3) keywds5 = keywds4;
  else keywds5 = keywds3;
  match_key(infile);
  return EXIT_SUCCESS;
}


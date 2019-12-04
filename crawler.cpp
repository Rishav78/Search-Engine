#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#define BASE_URL "https://www."
#define MAX_DEPTH 5

using namespace std;

// GENERATE FILE NAME
string name() {
    static int i = 0;
    return to_string(i++);
}

// STRING COMPARE
int validBaseUrl(string str2) {
    string str = BASE_URL;
    int sl1 = str.length(), sl2 = str2.length(), i = 0;
    if(sl1 > sl2) return false;
    while(i<sl1 && i<sl2) {
        if(str[i]!=str2[i]) return false;
        i++;
    }
    return true;
}

// VALID URL
bool vlaidURL(string URL) {
    string spider = "wget --spider -q " + URL;
    return !system(spider.c_str());
}

// CHECK IF READ WRITE OPERATION IS VALID AND ITS A DIR
void testDir(char *dir)
{
  struct stat statbuf;
  if ( stat(dir, &statbuf) == -1 )
  {
    fprintf(stderr, "-----------------\n");
    fprintf(stderr, "Invalid directory\n");
    fprintf(stderr, "-----------------\n");
    exit(1);
  }

  //Both check if there's a directory and if it's writable
  if ( !S_ISDIR(statbuf.st_mode) )
  {
    fprintf(stderr, "-----------------------------------------------------\n");
    fprintf(stderr, "Invalid directory entry. Your input isn't a directory\n");
    fprintf(stderr, "-----------------------------------------------------\n");
    exit(1);
  }

  if ( (statbuf.st_mode & S_IWUSR) != S_IWUSR )
  {
    fprintf(stderr, "------------------------------------------\n");
    fprintf(stderr, "Invalid directory entry. It isn't writable\n");
    fprintf(stderr, "------------------------------------------\n");
    exit(1);
  }
}


// VALIDITY
bool validArguments(int argc, char *argv[]) {
    if(argc != 4) {
        cout << "Too few arguments" << endl;
        return false;
    }
    testDir(argv[3]);
    if(!validBaseUrl(argv[1])) {
        cout << "Invalid base URL" << endl;
        return false;
    }
    if(stoi(argv[2]) > MAX_DEPTH) {
        cout << "Depth lenght exceded" << endl;
        return false;
    }
    if(!vlaidURL(argv[1])) {
        cout << "Invalid URL" << endl;
        return false;
    }
    return true;
}

// GET THE CODE OF PAGE AT A PARTICULAR URL
void getpage(string URL,string path, string filename) {
    string spider = "wget -O " + path + filename + " " + URL;
    system(spider.c_str());
}

string readFile(string path, string filename) {
    string htmlfile = path + filename, htmlCode = "";
    FILE *htmlF = fopen(htmlfile.c_str(), "r");
    if(!htmlF){
        return "error";
    }
    char ch = getc(htmlF);
    while(ch != EOF) {
        htmlCode += ch;
        ch = getc(htmlF);
    }
    return htmlCode;
}

vector<string> ftechLinks(string htmlCode) {
    int p = 0, i=0, anchor=0;
    vector<string> links;

    while((anchor=htmlCode.find("<a", p))!= string::npos ||
          (anchor=htmlCode.find("<A", p))!= string::npos) {
        int anchorend = htmlCode.find(">", anchor+1);
        int href = htmlCode.find("href", anchor+1);
        if(href != string::npos && href < anchorend) {
            char quote = htmlCode[href+5]; // check quotes use are single or double (",')
            int j = href+6, http;
            string link = "";
            while(htmlCode[j] != quote) {
                link += htmlCode[j];
                j++; 
            }
            if(link.find("http") != string::npos || link.find("https") != string::npos) {
                links.push_back(link);
            }
        }
        p = anchorend;
    }
    return links;
}

int getAndSavepage(string URL, string path, int depth) {
    if(depth == 0) {
        cout << "Max depth reached" << endl;
        return -1;
    }
    if(!vlaidURL(URL)) {
        cout << "Inavlid URL => " << URL << endl;
        return 404;
    }
    string filename = name() + ".txt", htmlCode;
    getpage(URL, path, filename);
    htmlCode = readFile(path, filename);
    if(htmlCode == "error") return 0;
    vector<string> links = ftechLinks(htmlCode);
    for(int i=0;i<links.size();i++){
        int error = getAndSavepage(links[i], path, depth -1);
        if(error == -1) break;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    // ERROR CHECKING
    if(!validArguments(argc, argv)) return 0;
    
    int depth = stoi(argv[2]);
    string URL = argv[1], path = argv[3];
    if(path[path.length()-1] != '/') 
        path += '/';
    getAndSavepage(URL, path, depth);

    return 0;
}

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<bits/stdc++.h>

using namespace std;



string readFile(string path, string filename) {
    string htmlfile = path + filename, htmlCode = "";
    FILE *htmlF = fopen(htmlfile.c_str(), "r");
    if(!htmlF) return "error";
    char ch = getc(htmlF);
    while(ch != EOF) {
        htmlCode += ch;
        ch = getc(htmlF);
    }
    return htmlCode;
}

int fetchTitle(string htmlCode, int i) {
    i = htmlCode.find(">", i) + 1;
    string keywords = "";
    while(htmlCode[i] != '<') {
        keywords += htmlCode[i++];
    }
    cout << keywords << endl;
    return i+8;
}

int fetchMeta(string htmlCode, int i) {
    int tagend = htmlCode.find(">", i);
    int content = htmlCode.find("content=\"", i) + 9;
    if(content > tagend) return tagend+1;
    string keywords = "";
    while(htmlCode[content] != '"') {
        keywords += htmlCode[content++];
    }
    cout << keywords << endl;
    return tagend+1;
}

int fetchHeading(string htmlCode, int i, string tag){
    i = htmlCode.find(">", i) + 1;
    string tage = "</" + tag + ">";
    cout << tage << endl;
    getchar();getchar();
    int tagend = htmlCode.find(tage.c_str(), i);
    string keywords = "";
    while (i < tagend) {
        if(htmlCode[i] == '<') {
            i = htmlCode.find(">", i+1) + 1;
        } else {
            keywords += htmlCode[i++];
        }
    }
    cout << keywords << endl;
    return tagend + 5;
}

void fetchTags(string htmlCode) {
    int i=0, p=0;
    while((i=htmlCode.find("<", p)) != string::npos) {
        if(htmlCode[i+1] == '/') {
            p = i + 1;
            continue;
        }
        string tag = "";
        int j = i + 1;
        while(htmlCode[j] != ' ' && htmlCode[j] != '>') {
            tag += htmlCode[j++];
        }
        string dup = tag;
        transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
        if(tag == "title") p = fetchTitle(htmlCode, i);
        else if(tag == "meta") p = fetchMeta(htmlCode, i);
        else if(tag == "h1" || tag == "h2" || tag == "h3" || tag == "h4" || tag == "h5" || tag == "h6")
            p = fetchHeading(htmlCode, i, dup);
        else p = i + 1;
    }
}

void getAllHtmlFiles(string path) {
    int i=0;
    while(i<5) {
        string filename = to_string(i++) + ".txt";
        string htmlCode = readFile(path, filename);
        if(htmlCode == "error") break;
        fetchTags(htmlCode);
        i++;
    }
}

int main(int argc, char *argv[]) {
    string path = argv[1];
    if(path[path.length()-1] != '/') path += '/';
    getAllHtmlFiles(path);
}
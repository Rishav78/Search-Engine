#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<bits/stdc++.h>

using namespace std;

struct Tag {
    string str;
    int count;
    Tag *next;
};

struct Filename {
    string str;
    int count;
    Filename *next;
    Tag *tags;
};

struct Keyword {
    string str;
    Keyword *next;
    int count;
    Filename *files;
};

struct Keywords {
    Keyword *ptr;
}keyhash[123];

Tag *createTag(string tagname) {
    Tag *ptr = new Tag;
    ptr->str = tagname;
    ptr->count = 1;
    ptr->next = nullptr;
    return ptr;
}

Filename *createFile(string filename, string tagname) {
    Filename *ptr = new Filename;
    ptr->str = filename;
    ptr->count = 1;
    ptr->next = nullptr;
    ptr->tags = createTag(tagname);
    return ptr;
}

Keyword *createKeyword(string keyword, string filename, string tagname) {
    Keyword *ptr = new Keyword;
    ptr->str = keyword;
    ptr->count = 1;
    ptr->files = createFile(filename, tagname);
    ptr->next = nullptr;
    return ptr;
}

// SAVE DATA

void saveTag(Filename *file, string tagname) {
    Tag *tag = file->tags;
    while(true) {
        if(tag->str == tagname) {
            tag->count++;
            return;
        }
        if(!(tag->next)) break;
        tag = tag->next;
    }
    tag->next = createTag(tagname);
}

void saveFilename(Keyword *key, string filename, string tagname) {
    Filename *file = key->files;
    while(true) {
        if(file->str == filename) {
            file->count++;
            saveTag(file, tagname);
            return;
        }
        if(!(file->next)) break;
        file = file->next;
    }
    file->next = createFile(filename, tagname);
}

void saveKeyword(string keyword, string filename, string tagname) {
    transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
    int index = (int)keyword[0];
    if(index < 0 || index > 122) return;
    Keyword *ptr = keyhash[index].ptr;
    if(!ptr) {
        keyhash[index].ptr = createKeyword(keyword, filename, tagname);
    } else {
        while(true) {
            if(ptr->str == keyword) {
                ptr->count++;
                saveFilename(ptr, filename, tagname);
                return;
            }
            if(!(ptr->next)) break;
            ptr = ptr->next;
        }
        ptr->next = createKeyword(keyword, filename, tagname);
    }
}

void show() {
    for(int i=0;i<123;i++) {
        Keyword *ptr = keyhash[i].ptr;
        if(ptr) {
            while(ptr) {
                Filename *file = ptr->files;
                while(file) {
                    Tag *tag = file->tags;
                    while(tag) {
                        cout << file->str << " " << tag->str << " " << ptr->str << " " << tag->count << endl;
                        tag = tag->next;
                    }
                    file = file->next;
                }
                ptr = ptr->next;
            }
        }
    }
}

void saveIndex() {
    FILE *p = fopen("./Index/index.txt", "a");
    for(int i=0;i<123;i++) {
        Keyword *ptr = keyhash[i].ptr;
        if(ptr) {
            while(ptr) {
                Filename *file = ptr->files;
                while(file) {
                    Tag *tag = file->tags;
                    while(tag) {
                        fprintf(p, "%s %s %s %d\n", file->str.c_str(), tag->str.c_str(), ptr->str.c_str(), tag->count);
                        tag = tag->next;
                    }
                    file = file->next;
                }
                ptr = ptr->next;
            }
        }
    }
}

void splitAndSave(string str, string filename, string tag) {
    int l=str.length();
    string s = "";
    for(int i=0;i<=l;i++) {
        if(str[i] < 'A' || (str[i] > 'Z' && str[i] < 'a') || str[i] > 'z' || i==l) {
            if(s != "") saveKeyword(s, filename, tag);
            s = "";
        } else {
            s += str[i];
        }
    }
}

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

int fetchTitle(string htmlCode, int i, string file) {
    i = htmlCode.find(">", i) + 1;
    string keywords = "";
    while(htmlCode[i] != '<') {
        keywords += htmlCode[i++];
    }
    splitAndSave(keywords, file, "title");
    return i+8;
}

int fetchMeta(string htmlCode, int i, string file) {
    int tagend = htmlCode.find(">", i);
    int content = htmlCode.find("content=\"", i) + 9;
    if(content > tagend) return tagend+1;
    string keywords = "";
    while(htmlCode[content] != '"') {
        keywords += htmlCode[content++];
    }
    splitAndSave(keywords, file, "meta");
    return tagend+1;
}

int fetchHeading(string htmlCode, int i, string tag, string file){
    i = htmlCode.find(">", i) + 1;
    string tage = "</" + tag + ">";
    int tagend = htmlCode.find(tage.c_str(), i);
    string keywords = "";
    while (i < tagend) {
        if(htmlCode[i] == '<') {
            i = htmlCode.find(">", i+1) + 1;
        } else {
            keywords += htmlCode[i++];
        }
    }
    splitAndSave(keywords, file, tag);
    return tagend + 5;
}

void fetchTags(string htmlCode, string path) {
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
        if(tag == "title") p = fetchTitle(htmlCode, i, path);
        else if(tag == "meta") p = fetchMeta(htmlCode, i, path);
        else if(tag == "h1" || tag == "h2" || tag == "h3" || tag == "h4" || tag == "h5" || tag == "h6")
            p = fetchHeading(htmlCode, i, dup, path);
        else p = i + 1;
    }
}

void getAllHtmlFiles(string path) {
    int i=0;
    while(true) {
        string filename = to_string(i++) + ".txt";
        string htmlCode = readFile(path, filename);
        if(htmlCode == "error") break;
        fetchTags(htmlCode, path+filename);
        i++;
    }
}

int main(int argc, char *argv[]) {
    cout << keyhash[0].ptr << endl;
    string path = argv[1];
    if(path[path.length()-1] != '/') path += '/';
    getAllHtmlFiles(path);
    // show();
    saveIndex();
}
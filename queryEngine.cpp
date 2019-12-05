#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include<bits/stdc++.h>
#include <vector>

using namespace std;

vector<string> cw = {"where","what","is","are","who","how","were","had","do","does","did","will"};

struct Pagelist {
    string str;
    int count;
    Pagelist *next, *prev;
};


// -------------------------------------
struct Tag {
    string str;
    int count;
    Tag *next;
    int wt;
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

Tag *createTag(string tagname, int count) {
    Tag *ptr = new Tag;
    ptr->str = tagname;
    ptr->count = count;
    ptr->next = nullptr;
    if(tagname == "meta") ptr->wt = 10;
    else if(tagname == "title") ptr->wt = 7;
    else ptr->wt = 5;
    return ptr;
}

Filename *createFile(string filename, string tagname, int count) {
    Filename *ptr = new Filename;
    ptr->str = filename;
    ptr->count = count;
    ptr->next = nullptr;
    ptr->tags = createTag(tagname, count);
    return ptr;
}

Keyword *createKeyword(string keyword, string filename, string tagname, int count) {
    Keyword *ptr = new Keyword;
    ptr->str = keyword;
    ptr->count = 1;
    ptr->files = createFile(filename, tagname, count);
    ptr->next = nullptr;
    return ptr;
}

Pagelist *createPagelist(string pagename, int wt, Pagelist *prev) {
    Pagelist *ptr = new Pagelist;
    ptr->str = pagename;
    ptr->count = wt;
    ptr->next = nullptr;
    ptr->prev = prev;
    return ptr;
}

// SAVE DATA
void saveTag(Filename *file, string tagname, int count) {
    Tag *tag = file->tags;
    while(true) {
        if(tag->str == tagname) {
            tag->count += count;
            return;
        }
        if(!(tag->next)) break;
        tag = tag->next;
    }
    tag->next = createTag(tagname, count);
}

void saveFilename(Keyword *key, string filename, string tagname, int count) {
    Filename *file = key->files;
    while(true) {
        if(file->str == filename) {
            file->count += count;
            saveTag(file, tagname, count);
            return;
        }
        if(!(file->next)) break;
        file = file->next;
    }
    file->next = createFile(filename, tagname, count);
}

void saveKeyword(string keyword, string filename, string tagname, int count) {
    int index = (int)keyword[0];
    if(index < 0 || index > 122) return;
    Keyword *ptr = keyhash[index].ptr;
    if(!ptr) {
        keyhash[index].ptr = createKeyword(keyword, filename, tagname, count);
    } else {
        while(true) {
            if(ptr->str == keyword) {
                ptr->count += count;
                saveFilename(ptr, filename, tagname, count);
                return;
            }
            if(!(ptr->next)) break;
            ptr = ptr->next;
        }
        ptr->next = createKeyword(keyword, filename, tagname, count);
    }
}

void retriveIndex(string path) {
    FILE *ptr = fopen(path.c_str(), "r");
    char file[100], keyword[100], tags[100];
    int d;
    while(fscanf(ptr,"%s %s %s %d", file, tags, keyword, &d) != EOF) {
        saveKeyword(keyword, file, tags, d);
    }
}

vector<string> filter(vector<string> words) {
    vector<string> w;
    for(int i=0;i<words.size();i++) {
        int j;
        for(j=0;j<cw.size();j++) {
            if(words[i] == cw[j]) break;
        }
        if(j == cw.size()) w.push_back(words[i]);
    }
    return w;
}

vector<string> split(string query) {
    int l=query.length();
    vector<string> words;
    string s = "";
    for(int i=0;i<=l;i++) {
        if(query[i] < 'A' || (query[i] > 'Z' && query[i] < 'a') || query[i] > 'z' || i==l) {
            transform(s.begin(), s.end(), s.begin(), ::tolower);
            if(s != "") words.push_back(s);
            s = "";
        } else {
            s += query[i];
        }
    }
    return words;
}

void upadtePageList(Pagelist **page, string filename, int total) {
    Pagelist *p = *page;
    if(p == nullptr) {
        *page = createPagelist(filename, total, nullptr);
    } else {
        while(p) {
            if(p->str == filename) {
                p->count += total;
                return;
            }
            if(!(p->next)) break;
            p = p->next;
        }
        p->next = createPagelist(filename, total, p);
    }
}

void userQuery() {
    string query;
    Pagelist *page = nullptr;
    getline(cin, query);
    vector<string> words = split(query);
    words = filter(words);
    for(int i=0;i<words.size();i++) {
        int index = (int)words[i][0];
        Keyword *key = keyhash[index].ptr;
        if(!key) continue;
        while(key) {
            if(key->str == words[i] || key->str.find(words[i]) != string::npos) {
                Filename *file = key->files;
                while(file) {
                    int total = 0;
                    Tag *tag = file->tags;
                    while(tag) {
                        total = total + tag->wt;
                        tag = tag->next;
                    }
                    upadtePageList(&page, file->str, total);
                    file = file->next;
                }
            }
            key = key->next;
        }
    }

    while(page != nullptr) {
        Pagelist *ptr = page, *dup = nullptr;
        int m = 0;
        while(ptr != nullptr) {
            if(ptr->count > m) {
                m = ptr->count;
                dup = ptr;
            }
            ptr = ptr->next;
        }
        if(!dup) return;
        cout << dup->str << " " << dup->count << endl;
        dup->count = -1;
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
                        cout << file->str << " " << tag->str << " " << ptr->str << " " << tag->count << " " << i << endl;
                        tag = tag->next;
                    }
                    file = file->next;
                }
                ptr = ptr->next;
            }
        }
    }
}

bool validArguments(int argc, char *argv[]) {
    if(argc <2 || argc > 2) {
        cout << "Invalid number of arguments" << endl;
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    if(!validArguments(argc, argv)) return 0;
    string path = argv[1];
    if(path[path.length()-1] != '/') path += '/'; 
    retriveIndex(path+"Index/index.txt");
    userQuery();
}
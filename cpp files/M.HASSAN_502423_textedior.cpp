#include "texteditor.h"
#include <map>
#include <sstream>
#include <iostream>
using namespace std;

class CharNode {
public:
    char data;
    CharNode* next;
    CharNode* prev;
    
    CharNode(char c) : data(c), next(nullptr), prev(nullptr) {}
};


struct EditorData {
    CharNode* head;
    CharNode* cursor;  
    int cursorPos;
    
    EditorData() : head(nullptr), cursor(nullptr), cursorPos(0) {}
};

static map<const TextEditor*, EditorData*> editorData;

EditorData* getData(const TextEditor* ed) {
    if (editorData.find(ed) == editorData.end()) {
        editorData[ed] = new EditorData();
    }
    return editorData[ed];
}

void TextEditor::insertChar(char c) {
    EditorData* data = getData(this);
    CharNode* newNode = new CharNode(c);
    
    if (data->head == nullptr) {
        data->head = newNode;
        data->cursor = newNode;
    } else if (data->cursor == nullptr) {
        newNode->next = data->head;
        data->head->prev = newNode;
        data->head = newNode;
        data->cursor = newNode;
    } else {
        newNode->next = data->cursor->next;
        newNode->prev = data->cursor;
        
        if (data->cursor->next != nullptr) {
            data->cursor->next->prev = newNode;
        }
        data->cursor->next = newNode;
        data->cursor = newNode;
    }
    
    data->cursorPos++;
}

void TextEditor::deleteChar() {
    EditorData* data = getData(this);
    
    if (data->cursorPos == 0 || data->head == nullptr) {
        return;
    }
    
    CharNode* toDelete = data->cursor;
    
    if (toDelete == data->head) {
        data->head = toDelete->next;
        if (data->head != nullptr) {
            data->head->prev = nullptr;
        }
        data->cursor = nullptr;
    } else {
        toDelete->prev->next = toDelete->next;
        if (toDelete->next != nullptr) {
            toDelete->next->prev = toDelete->prev;
        }
        data->cursor = toDelete->prev;
    }
    
    delete toDelete;
    data->cursorPos--;
}

void TextEditor::moveLeft() {
    EditorData* data = getData(this);
    
    if (data->cursorPos == 0) {
        return;
    }
    
    if (data->cursor != nullptr) {
        data->cursor = data->cursor->prev;
    }
    data->cursorPos--;
}

void TextEditor::moveRight() {
    EditorData* data = getData(this);
    
    int length = 0;
    CharNode* temp = data->head;
    while (temp != nullptr) {
        length++;
        temp = temp->next;
    }
    
    if (data->cursorPos >= length) {
        return;
    }
    
    if (data->cursor == nullptr) {
        data->cursor = data->head;
    } else {
        data->cursor = data->cursor->next;
    }
    data->cursorPos++;
}

string TextEditor::getTextWithCursor() const {
    EditorData* data = getData(this);
    ostringstream oss;
    
    CharNode* current = data->head;
    int pos = 0;
    
    if (data->cursorPos == 0) {
        oss << "|";
    }
    
    while (current != nullptr) {
        oss << current->data;
        pos++;
        
        if (pos == data->cursorPos) {
            oss << "|";
        }
        
        current = current->next;
    }
    
    return oss.str();
}

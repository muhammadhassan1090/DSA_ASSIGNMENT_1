#include "polynomial.h"
#include <map>
#include <algorithm>
#include <sstream>
#include <iostream>
using namespace std;

class Node {
public:
    int coef;
    int exp;
    Node* next;
    
    Node(int c, int e) : coef(c), exp(e), next(nullptr) {}
};

static map<const Polynomial*, Node*> polynomialData;

Node*& getHead(const Polynomial* p) {
    return polynomialData[p];
}

Node* copyList(Node* source) {
    if (!source) return nullptr;
    
    Node* newHead = new Node(source->coef, source->exp);
    Node* current = newHead;
    Node* sourcePtr = source->next;
    
    while (sourcePtr) {
        current->next = new Node(sourcePtr->coef, sourcePtr->exp);
        current = current->next;
        sourcePtr = sourcePtr->next;
    }
    
    return newHead;
}

void deleteList(Node*& h) {
    while (h) {
        Node* temp = h;
        h = h->next;
        delete temp;
    }
}

void simplify(Node*& h) {
    if (!h) return;

    bool swapped;
    do {
        swapped = false;
        Node* current = h;
        
        while (current && current->next) {
            if (current->exp < current->next->exp) {
                swap(current->coef, current->next->coef);
                swap(current->exp, current->next->exp);
                swapped = true;
            }
            current = current->next;
        }
    } while (swapped);

    Node* current = h;
    while (current && current->next) {
        if (current->exp == current->next->exp) {
            current->coef += current->next->coef;
            Node* temp = current->next;
            current->next = current->next->next;
            delete temp;
        } else {
            current = current->next;
        }
    }

    while (h && h->coef == 0) {
        Node* temp = h;
        h = h->next;
        delete temp;
    }

    if (h) {
        current = h;
        while (current->next) {
            if (current->next->coef == 0) {
                Node* temp = current->next;
                current->next = current->next->next;
                delete temp;
            } else {
                current = current->next;
            }
        }
    }
}

void Polynomial::insertTerm(int coefficient, int exponent) {
    if (coefficient == 0 || exponent < 0) return;
    
    Node*& head = getHead(this);
    Node* newNode = new Node(coefficient, exponent);
    newNode->next = head;
    head = newNode;
    
    simplify(head);
}

string Polynomial::toString() const {
    Node* head = getHead(this);
    if (!head) return "0";

    ostringstream oss;
    Node* current = head;
    bool first = true;

    while (current) {
        if (first) {
            if (current->coef < 0) {
                oss << "-";
            }
            first = false;
        } else {
            if (current->coef > 0) {
                oss << " + ";
            } else {
                oss << " - ";
            }
        }

        int absCoef = abs(current->coef);

        if (absCoef != 1 || current->exp == 0) {
            oss << absCoef;
        }

        if (current->exp > 0) {
            oss << "x";
        }

        if (current->exp > 1) {
            oss << "^" << current->exp;
        }

        current = current->next;
    }
    return oss.str();
}

Polynomial Polynomial::add(const Polynomial &other) const {
    Polynomial result;
    Node*& resultHead = getHead(&result);
    
    Node* current = getHead(this);
    while (current) {
        Node* newNode = new Node(current->coef, current->exp);
        newNode->next = resultHead;
        resultHead = newNode;
        current = current->next;
    }
    
    current = getHead(&other);
    while (current) {
        Node* newNode = new Node(current->coef, current->exp);
        newNode->next = resultHead;
        resultHead = newNode;
        current = current->next;
    }
    
    simplify(resultHead);
    return result;
}

Polynomial Polynomial::multiply(const Polynomial &other) const {
    Polynomial result;
    Node*& resultHead = getHead(&result);
    
    Node* current1 = getHead(this);
    while (current1) {
        Node* current2 = getHead(&other);
        while (current2) {
            Node* newNode = new Node(
                current1->coef * current2->coef, 
                current1->exp + current2->exp
            );
            newNode->next = resultHead;
            resultHead = newNode;
            current2 = current2->next;
        }
        current1 = current1->next;
    }
    
    simplify(resultHead);
    return result;
}

Polynomial Polynomial::derivative() const {
    Polynomial result;
    Node*& resultHead = getHead(&result);
    
    Node* current = getHead(this);
    while (current) {
        if (current->exp != 0) {
            Node* newNode = new Node(
                current->coef * current->exp, 
                current->exp - 1
            );
            newNode->next = resultHead;
            resultHead = newNode;
        }
        current = current->next;
    }
    
    simplify(resultHead);
    return result;
}

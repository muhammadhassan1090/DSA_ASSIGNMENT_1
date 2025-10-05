# CS250 Data Structures and Algorithms - Assignment 1

**Student Name:** M. Hassan  
**Qalam ID:** 503423  
**Class:** BSDS-2A

---

## 📌 Overview

This assignment implements three Abstract Data Types (ADTs) using linked lists:
1. **Polynomial ADT** - Mathematical polynomial operations
2. **Text Editor** - Cursor-based text manipulation
3. **UNO Game** - Card game simulation

---

## 🎯 Approach

### **Problem 1: Polynomial ADT**

**Data Structure:** Singly Linked List

**Design Decisions:**
- Each node stores a term's coefficient and exponent
- Global `map<const Polynomial*, Node*>` stores each polynomial instance's head pointer
- This approach allows multiple polynomial objects without modifying the header file

**Algorithm:**
1. **Insert Term:** Add node at head, then simplify (sort by exponent descending, combine like terms, remove zeros)
2. **Add:** Copy all terms from both polynomials into result, then simplify
3. **Multiply:** Generate all term products (coef1×coef2, exp1+exp2), then simplify
4. **Derivative:** Apply power rule: d/dx(ax^n) = nax^(n-1)
5. **ToString:** Traverse sorted list, format with proper signs and notation


### **Problem 2: Text Editor with Cursor**

**Data Structure:** Doubly Linked List

**Design Decisions:**
- Doubly linked list allows efficient bidirectional cursor movement
- Cursor pointer tracks the node BEFORE the cursor position
- Each node contains a character and prev/next pointers

**Algorithm:**
1. **Insert Char:** Create new node after cursor position, update links
2. **Delete Char:** Remove node at cursor position (backspace behavior)
3. **Move Left/Right:** Update cursor pointer with bounds checking
4. **Display:** Traverse list, insert '|' at cursor position

---

### **Problem 3: UNO Game Simulation**

**Data Structure:**  Singly Linked Lists

**Design Decisions:**
- Separate linked lists for: deck, discard pile, each player's hand
- Fixed seed (1234) for reproducible shuffling using `<random>`
- Card matching priority: color match → value match → action cards

**Algorithm:**
1. **Initialize:**
   - Create 76 cards (4 colors × [1×0 + 2×(1-9) + 2×3 actions])
   - Shuffle using `std::shuffle` with seed 1234
   - Deal 7 cards to each player
   - Place top card on discard pile

2. **Play Turn:**
   - Search for matching card in priority order
   - If found: play card, handle action effects
   - If not found: draw card, play if matches
   - Update current player based on direction

3. **Action Cards:**
   - **Skip:** Next player loses turn
   - **Reverse:** Change direction (clockwise ↔ counter-clockwise)
   - **Draw Two:** Next player draws 2 and loses turn


## 💡 Challenges Faced

### **Challenge 1: Object Slicing in Polynomial**
**Problem:** Initially tried returning `Polynomial` objects by value from virtual functions, causing object slicing and data loss.

**Solution:** Used a global `map` to store each polynomial's data externally, keyed by the object's address. This preserves data across function returns without modifying the header file.

**Learning:** Understanding the difference between stack and heap allocation, and how polymorphism works with value returns.

---

### **Challenge 2: Cursor Position Management in Text Editor**
**Problem:** Determining whether cursor should point to a node or be `nullptr` at position 0 caused confusion in edge cases.

**Solution:** Defined clear semantics: cursor points to the node BEFORE the cursor position. At position 0, cursor is `nullptr`. This made insert/delete logic consistent.

**Learning:** Importance of clear data structure invariants and edge case documentation.

---

### **Challenge 3: UNO Card Matching Priority**
**Problem:** Assignment specifies matching priority (color → value → action) but wasn't clear if this applies per turn or per card search.

**Solution:** Implemented three-pass search algorithm:
1. First pass: color-matching non-action cards
2. Second pass: value-matching cards
3. Third pass: action cards with matching color

**Learning:** Breaking down complex requirements into clear, testable steps.

---

## 🛠️ Build Instructions

```bash
# Compile all programs
g++ -std=c++11 M.HASSAN_503423_polynomial.cpp -o polynomial
g++ -std=c++11 M.HASSAN_503423_texteditor.cpp -o texteditor
g++ -std=c++11 M.HASSAN_503423_uno.cpp -o uno

# Run programs
./polynomial
./texteditor
./uno
```

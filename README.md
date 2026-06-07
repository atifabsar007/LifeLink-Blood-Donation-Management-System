# LifeLink - Blood Donation Management System

## Overview

LifeLink is a comprehensive **Blood Donation Management System** developed in **C++** using Object-Oriented Programming principles. The system provides a complete solution for managing blood donors, recipient requests, and blood inventory while maintaining persistent records through file storage.

The project demonstrates advanced C++ programming concepts including **Inheritance, Polymorphism, Templates, Operator Overloading, File Handling, STL Containers, Composition, and Generic Programming** through a practical healthcare management application.

---

## Key Features

### Donor Management

* Register new blood donors
* Maintain donor profiles
* Track donation history
* Automatic donor count tracking
* Search donors by blood group

### Recipient Management

* Register blood recipients
* Manage blood requests
* Track request fulfillment status
* Store recipient information

### Blood Inventory Management

* Track stock for all major blood groups
* Record blood donations
* Fulfill blood requests
* Real-time inventory updates
* Low-stock monitoring

### Data Persistence

* Save donor records
* Save recipient records
* Store inventory information
* Automatic file loading and saving

### Search & Reporting

* View all donors
* View all recipients
* Search donors by blood group
* View current blood inventory
* Donation and request tracking

---

## Technology Stack

* C++
* Object-Oriented Programming (OOP)
* Standard Template Library (STL)
* File Handling (fstream)
* GCC / G++ Compatible
* Console-Based User Interface

---

## System Architecture

The project follows a modular object-oriented design with clear class relationships.

### Class Hierarchy

```text
Person
│
├── Donor
│
└── Recipient
```

### Core Components

#### Person Class

Base class containing:

* Name
* Age
* Blood Group
* Contact Information

#### Donor Class

Extends Person and provides:

* Unique Donor ID
* Donation History
* Donation Tracking
* Static Donor Counter

#### Recipient Class

Extends Person and provides:

* Unique Recipient ID
* Blood Request Information
* Units Required
* Request Status Tracking

#### BloodBank<T>

Generic template container responsible for:

* Managing records
* Storing donors and recipients
* Searching and retrieval operations

#### BloodInventory

Handles:

* Blood stock management
* Inventory updates
* Donation transactions
* Request fulfillment

#### FileHandler

Provides:

* Data persistence
* File loading
* File saving
* Record management

#### LifeLink

Main application controller responsible for:

* Menu management
* User interaction
* System coordination

---

## OOP Concepts Demonstrated

### Encapsulation

* Private data members
* Controlled access through methods

### Inheritance

* Person → Donor
* Person → Recipient

### Polymorphism

* Virtual functions
* Virtual destructor
* Runtime method binding

### Templates

* Generic BloodBank<T> class
* Reusable utility functions

### Composition

* LifeLink contains:

  * BloodBank<Donor>
  * BloodBank<Recipient>
  * BloodInventory

### Friend Functions

* Donation update operations
* Controlled access to private data

### Static Members

* Global donor tracking

---

## Operator Overloading

| Operator | Class            | Purpose                 |
| -------- | ---------------- | ----------------------- |
| <<       | Donor, Recipient | Formatted output        |
| ++       | Donor            | Increase donation count |
| +        | BloodInventory   | Inventory calculations  |
| -        | BloodInventory   | Inventory updates       |
| ==       | Donor            | Donor comparison        |
| int()    | Donor            | Type conversion         |

---

## STL Containers Used

### Vector

```cpp
vector<T*>
```

Used for storing donor and recipient records dynamically.

### Map

```cpp
map<string, int>
```

Used for blood inventory management and fast blood group lookup.

---

## Data Storage

The system maintains persistent data using text files:

```text
donors.txt
recipients.txt
inventory.txt
```

This allows records to remain available between program executions.

---

## Learning Outcomes

This project provided practical experience in:

* Advanced Object-Oriented Programming
* Generic Programming with Templates
* STL Containers and Algorithms
* File Handling and Data Persistence
* Healthcare Information Systems
* Memory Management
* Real-World Software Design
* Modular Application Development

---

## Future Enhancements

* Blood Compatibility Matching System
* Binary File Storage
* Exception Handling (try-catch)
* Advanced Reporting Dashboard
* GUI-Based Interface
* Database Integration (MySQL/PostgreSQL)
* Online Blood Donation Platform
* Automated Donor Notification System

---

## Academic Project

This project was developed as part of the **Object-Oriented Programming (OOP)** course at **Khulna University of Engineering & Technology (KUET)**.

It demonstrates how modern C++ programming techniques can be applied to solve real-world healthcare management problems through clean software architecture and reusable design patterns.

---

## Author

Md. Atif Absar | Agomon Das Dhrubo | Ritovash Chanda
Department of Computer Science & Engineering
Khulna University of Engineering & Technology (KUET)

GitHub: https://github.com/atifabsar007

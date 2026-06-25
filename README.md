# ✈️ Airline Reservation System

A console-based **Airline Reservation System** built in C++ that demonstrates core Object-Oriented Programming concepts including Inheritance, Polymorphism, Encapsulation, and Abstraction. The system allows users to view flights, reserve seats, manage bookings, and cancel reservations — with data persisting across sessions via CSV file storage.

---

## 🚀 Features

- View all available flights with real-time seat availability
- Reserve a seat on any available flight
- View bookings by passenger name
- Cancel an existing booking (automatically restores the seat)
- Auto-generated unique Booking IDs with timestamp
- Data persistence using CSV files — no data loss between runs
- Input validation to handle incorrect entries gracefully

---

## 🛠️ Tech Stack

| Technology | Usage |
|------------|-------|
| C++ (C++17) | Core language |
| OOP | Abstraction, Inheritance, Polymorphism, Encapsulation |
| STL | `vector`, `string`, `fstream`, `sstream` |
| File Handling | CSV-based persistent storage |

---

## 🧱 OOP Concepts Demonstrated

### 1. Abstraction
`Transport` is an **abstract base class** with a pure virtual `display()` method — enforces a contract that all transport types must implement their own display logic.

```cpp
class Transport {
public:
    virtual void display() const = 0;  // pure virtual
    virtual string serialize() const;
};
```

### 2. Inheritance
`Flight` inherits from `Transport`, extending it with airline-specific fields like `departureTime` and `airline`.

```cpp
class Flight : public Transport {
    string departureTime;
    string airline;
};
```

### 3. Polymorphism
`display()` and `serialize()` are overridden in `Flight`. The system calls them through `Transport*` pointers — runtime polymorphism in action.

```cpp
for (const auto* t : flights)
    t->display();  // calls Flight::display() at runtime
```

### 4. Encapsulation
All data members in `Transport` and `Flight` are `private` or `protected`, accessible only through getters/setters.

```cpp
string getId()             const { return id; }
int    getAvailableSeats() const { return availableSeats; }
```

### 5. Separate Booking Class (Single Responsibility)
The `Booking` class handles only booking-related logic — passenger name, flight ID, booking ID, and timestamp — keeping concerns separated from the flight logic.

---

## 📁 Project Structure

```
airline-reservation-system/
│
├── airline_reservation.cpp   # Main source code
├── flights.csv               # Auto-generated: stores flight data
├── bookings.csv              # Auto-generated: stores booking records
└── README.md
```

> `flights.csv` and `bookings.csv` are auto-created on first run. You do not need to create them manually.

---

## ⚙️ How to Run

### Prerequisites
- GCC compiler with C++17 support
- Any terminal (VS Code terminal, PowerShell, bash)

### Compile
```bash
g++ -std=c++17 airline_reservation.cpp -o airline
```

### Run
```bash
# Linux / Mac
./airline

# Windows
.\airline.exe
```

---

## 🖥️ Sample Output

```
====================================
   Airline Reservation System
====================================
1. View Available Flights
2. Reserve a Seat
3. View My Bookings
4. Cancel a Booking
5. Exit
====================================
Enter choice: 1

========== Available Flights ==========
[F101]  Delhi -> Dubai        | Airline: Air India       | Departs: 08:00 | Seats: 50/50
[F202]  Mumbai -> New York    | Airline: IndiGo          | Departs: 14:30 | Seats: 40/40
[F303]  Chennai -> London     | Airline: British Airways | Departs: 22:15 | Seats: 30/30
[F404]  Kolkata -> Toronto    | Airline: Air Canada      | Departs: 11:00 | Seats: 2/2
========================================
```

```
Enter Flight ID to reserve: F404
Enter your name: Ishan

  *** Booking Confirmed ***
  Booking ID   : B1001
  Passenger    : Ishan
  Flight       : F404
  Booked At    : 2026-06-25 19:58
```

---

## 📌 Default Flights (Seeded on First Run)

| Flight ID | Origin | Destination | Airline | Departure | Seats |
|-----------|--------|-------------|---------|-----------|-------|
| F101 | Delhi | Dubai | Air India | 08:00 | 50 |
| F202 | Mumbai | New York | IndiGo | 14:30 | 40 |
| F303 | Chennai | London | British Airways | 22:15 | 30 |
| F404 | Kolkata | Toronto | Air Canada | 11:00 | 2 |

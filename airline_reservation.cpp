/*
 * Airline Reservation System
 * Demonstrates: Inheritance, Polymorphism, Encapsulation, Abstraction,
 *               File Handling, Input Validation, STL (vector)
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <ctime>

using namespace std;

// ─────────────────────────────────────────────
//  Abstract Base Class  (Abstraction)
// ─────────────────────────────────────────────
class Transport {
protected:
    string id;
    string origin;
    string destination;
    int    totalSeats;
    int    availableSeats;

public:
    Transport(const string& id, const string& origin,
              const string& dest, int seats)
        : id(id), origin(origin), destination(dest),
          totalSeats(seats), availableSeats(seats) {}

    virtual ~Transport() {}

    // Pure virtual – every transport must implement its own display
    virtual void display() const = 0;

    // Getters (Encapsulation)
    string getId()            const { return id; }
    string getOrigin()        const { return origin; }
    string getDestination()   const { return destination; }
    int    getTotalSeats()    const { return totalSeats; }
    int    getAvailableSeats()const { return availableSeats; }

    bool hasSeatsAvailable()  const { return availableSeats > 0; }

    bool reserveSeat() {
        if (!hasSeatsAvailable()) return false;
        --availableSeats;
        return true;
    }

    // Serialize to a single CSV line
    virtual string serialize() const {
        ostringstream ss;
        ss << id << "," << origin << "," << destination
           << "," << totalSeats << "," << availableSeats;
        return ss.str();
    }

    // Load available seats from a deserialized value
    void setAvailableSeats(int s) { availableSeats = s; }
};

// ─────────────────────────────────────────────
//  Derived Class: Flight  (Inheritance)
// ─────────────────────────────────────────────
class Flight : public Transport {
private:
    string departureTime;
    string airline;

public:
    Flight(const string& id, const string& origin,
           const string& dest, int seats,
           const string& depTime, const string& airlineName)
        : Transport(id, origin, dest, seats),
          departureTime(depTime), airline(airlineName) {}

    string getDepartureTime() const { return departureTime; }
    string getAirline()       const { return airline; }

    // Override display (Polymorphism)
    void display() const override {
        cout << "  [" << id << "]  "
             << origin << " -> " << destination
             << "  |  Airline: " << airline
             << "  |  Departs: " << departureTime
             << "  |  Seats: " << availableSeats << "/" << totalSeats
             << endl;
    }

    // Override serialize to include extra fields
    string serialize() const override {
        ostringstream ss;
        ss << "FLIGHT," << id << "," << origin << "," << destination
           << "," << totalSeats << "," << availableSeats
           << "," << departureTime << "," << airline;
        return ss.str();
    }
};

// ─────────────────────────────────────────────
//  Booking class  (Encapsulation / SRP)
// ─────────────────────────────────────────────
class Booking {
private:
    string bookingId;
    string passengerName;
    string flightId;
    string timestamp;

    static string currentTimestamp() {
        time_t now = time(nullptr);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&now));
        return string(buf);
    }

public:
    Booking(const string& bId, const string& pName, const string& fId)
        : bookingId(bId), passengerName(pName),
          flightId(fId), timestamp(currentTimestamp()) {}

    string getBookingId()     const { return bookingId; }
    string getPassengerName() const { return passengerName; }
    string getFlightId()      const { return flightId; }
    string getTimestamp()     const { return timestamp; }

    void display() const {
        cout << "\n  *** Booking Confirmed ***\n"
             << "  Booking ID   : " << bookingId       << "\n"
             << "  Passenger    : " << passengerName    << "\n"
             << "  Flight       : " << flightId         << "\n"
             << "  Booked At    : " << timestamp        << "\n";
    }

    string serialize() const {
        return bookingId + "," + passengerName + "," +
               flightId  + "," + timestamp;
    }
};

// ─────────────────────────────────────────────
//  AirlineSystem – manages flights & bookings
// ─────────────────────────────────────────────
class AirlineSystem {
private:
    vector<Transport*> flights;   // polymorphic container
    vector<Booking>    bookings;

    const string FLIGHTS_FILE  = "flights.csv";
    const string BOOKINGS_FILE = "bookings.csv";

    int bookingCounter = 1000;

    // ── File helpers ──────────────────────────
    void saveFlights() const {
        ofstream out(FLIGHTS_FILE);
        if (!out) { cerr << "Error: Cannot write " << FLIGHTS_FILE << "\n"; return; }
        for (const auto* t : flights)
            out << t->serialize() << "\n";
    }

    void saveBookings() const {
        ofstream out(BOOKINGS_FILE);
        if (!out) { cerr << "Error: Cannot write " << BOOKINGS_FILE << "\n"; return; }
        for (const auto& b : bookings)
            out << b.serialize() << "\n";
    }

    void loadFlights() {
        ifstream in(FLIGHTS_FILE);
        if (!in) return;   // first run – no file yet

        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            string token;
            vector<string> fields;
            while (getline(ss, token, ','))
                fields.push_back(token);

            if (fields.size() >= 8 && fields[0] == "FLIGHT") {
                // Constructor takes: id, origin, dest, totalSeats, depTime, airline
                Flight* f = new Flight(
                    fields[1], fields[2], fields[3],
                    stoi(fields[4]),          // totalSeats
                    fields[6], fields[7]);    // departureTime, airline
                // Restore the saved available seats (may differ from total)
                f->setAvailableSeats(stoi(fields[5]));
                flights.push_back(f);
            }
        }
    }

    void loadBookings() {
        ifstream in(BOOKINGS_FILE);
        if (!in) return;

        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            string f[4];
            for (int i = 0; i < 4; ++i) getline(ss, f[i], ',');
            bookings.emplace_back(f[0], f[1], f[2]);
            // update counter so new IDs don't clash
            try {
                int num = stoi(f[0].substr(1));
                if (num >= bookingCounter) bookingCounter = num + 1;
            } catch (...) {}
        }
    }

    // ── Input helpers ─────────────────────────
    static int getIntInput(const string& prompt) {
        int val;
        while (true) {
            cout << prompt;
            if (cin >> val) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return val;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  Invalid input. Please enter a number.\n";
        }
    }

    static string getStringInput(const string& prompt) {
        string val;
        cout << prompt;
        getline(cin, val);
        return val;
    }

    // ── Find a flight by ID ───────────────────
    Transport* findFlight(const string& id) {
        for (auto* t : flights)
            if (t->getId() == id) return t;
        return nullptr;
    }

public:
    AirlineSystem() {
        loadFlights();
        loadBookings();

        // Seed default flights only if the file didn't exist
        if (flights.empty()) {
            flights.push_back(new Flight("F101", "Delhi",   "Dubai",    50, "08:00", "Air India"));
            flights.push_back(new Flight("F202", "Mumbai",  "New York", 40, "14:30", "IndiGo"));
            flights.push_back(new Flight("F303", "Chennai", "London",   30, "22:15", "British Airways"));
            flights.push_back(new Flight("F404", "Kolkata", "Toronto",   2, "11:00", "Air Canada"));
            saveFlights();
        }
    }

    ~AirlineSystem() {
        for (auto* t : flights) delete t;
    }

    // ── Menu actions ──────────────────────────
    void showAllFlights() const {
        cout << "\n  ========== Available Flights ==========\n";
        bool any = false;
        for (const auto* t : flights) {
            if (t->hasSeatsAvailable()) {
                t->display();   // polymorphic call
                any = true;
            }
        }
        if (!any) cout << "  No flights available at the moment.\n";
        cout << "  =======================================\n";
    }

    void reserveSeat() {
        showAllFlights();

        string flightId = getStringInput("\n  Enter Flight ID to reserve: ");
        Transport* t = findFlight(flightId);

        if (!t) {
            cout << "  Flight not found.\n";
            return;
        }
        if (!t->hasSeatsAvailable()) {
            cout << "  Sorry, no seats available on this flight.\n";
            return;
        }

        string name = getStringInput("  Enter your name: ");
        if (name.empty()) { cout << "  Name cannot be empty.\n"; return; }

        t->reserveSeat();

        string bId = "B" + to_string(++bookingCounter);
        Booking b(bId, name, flightId);
        b.display();
        bookings.push_back(b);

        saveFlights();
        saveBookings();
    }

    void viewMyBookings() const {
        string name = getStringInput("\n  Enter your name to search: ");
        bool found = false;
        for (const auto& b : bookings) {
            if (b.getPassengerName() == name) {
                b.display();
                found = true;
            }
        }
        if (!found) cout << "  No bookings found for \"" << name << "\".\n";
    }

    void cancelBooking() {
        string bId = getStringInput("\n  Enter Booking ID to cancel: ");
        for (int i = 0; i < (int)bookings.size(); ++i) {
            if (bookings[i].getBookingId() == bId) {
                // restore seat
                Transport* t = findFlight(bookings[i].getFlightId());
                if (t) t->setAvailableSeats(t->getAvailableSeats() + 1);

                cout << "  Booking " << bId << " cancelled.\n";
                bookings.erase(bookings.begin() + i);
                saveFlights();
                saveBookings();
                return;
            }
        }
        cout << "  Booking ID not found.\n";
    }

    // ── Main loop ─────────────────────────────
    void run() {
        while (true) {
            cout << "\n  ====================================\n"
                 << "     Airline Reservation System\n"
                 << "  ====================================\n"
                 << "  1. View Available Flights\n"
                 << "  2. Reserve a Seat\n"
                 << "  3. View My Bookings\n"
                 << "  4. Cancel a Booking\n"
                 << "  5. Exit\n"
                 << "  ====================================\n";

            int choice = getIntInput("  Enter choice: ");

            switch (choice) {
                case 1: showAllFlights();  break;
                case 2: reserveSeat();     break;
                case 3: viewMyBookings();  break;
                case 4: cancelBooking();   break;
                case 5:
                    cout << "  Thank you for using Airline Reservation System. Goodbye!\n";
                    return;
                default:
                    cout << "  Invalid option. Please choose 1-5.\n";
            }
        }
    }
};

// ─────────────────────────────────────────────
int main() {
    AirlineSystem system;
    system.run();
    return 0;
}
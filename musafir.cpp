#include "raylib.h"
#include <string>
using namespace std;

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const Color green= {0, 102, 51, 255};

const int MAX_HOTELS = 60;
const int MAX_BOOKINGS = 50;
const int MAX_CITIES = 7;

const string CITIES[MAX_CITIES] = {
    "Islamabad", "Lahore", "Karachi", "Peshawar",
    "Quetta", "Gilgit", "Muzaffarabad"
};
//Data structures
struct Hotel {
    string name;
    string city;
    string category;
    float basePrice;
    float currentPrice;
    float rating;
    bool hasWifi;
    bool hasPool;
    bool isActive;
    bool hasDeal;
    float dealPercent;
};
struct Booking {
    string hotelName;
    string city;
    string bookingId;
    int nights;
    int guests;
    float totalCost;
    bool isActive;
};

struct UserProfile {
    string name;
    int totalBookings;
    float totalSpent;
    float maxBudget;
    bool budgetMode;
};
// Global arrays
Hotel hotels[MAX_HOTELS];
int hotelCount = 0;
UserProfile user;

// Add hotel helper
void addHotel(string name, string city, string category, float price, float rating, bool hasPool) {
    if (hotelCount >= MAX_HOTELS) return;
    Hotel h;
    h.name = name;
    h.city = city;
    h.category = category;
    h.basePrice = price;
    h.currentPrice = price;
    h.rating = rating;
    h.hasWifi = true;
    h.hasPool = hasPool;
    h.isActive = true;
    h.hasDeal = false;
    h.dealPercent = 0;
    hotels[hotelCount++] = h;
}
void initializeHotels() {
    hotelCount = 0;
    addHotel("Serena Hotel", "Islamabad", "Luxury", 25000, 4.8f, true);
    addHotel("Marriott Islamabad", "Islamabad", "Luxury", 22000, 4.7f, true);
    addHotel("Hotel One", "Islamabad", "Budget", 4000, 3.9f, false);
    addHotel("Pearl Continental", "Lahore", "Luxury", 20000, 4.6f, true);
    addHotel("Avari Hotel", "Lahore", "Luxury", 18000, 4.5f, true);
    addHotel("Movenpick Karachi", "Karachi", "Luxury", 19000, 4.5f, true);
    addHotel("Beach Luxury Hotel", "Karachi", "Resort", 15000, 4.2f, true);
}
int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Musafir - Pakistan Travel Guide"); //Initialize window
    SetTargetFPS(60);
    //Initialize data
    initializeHotels();
    user.name = "Traveler";
    user.totalBookings = 0;
    user.totalSpent = 0;
    user.maxBudget = 50000;
    user.budgetMode = true;
    // Main loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        
        ClearBackground(green);
        DrawText("MUSAFIR", 380, 300, 64, WHITE);
        DrawText("Pakistan Travel Guide", 380, 380, 24, Color{200, 255, 200, 255});
        EndDrawing();
    }
    // Cleanup
    CloseWindow();
    return 0;
}

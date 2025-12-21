#include "raylib.h"
#include <string>
using namespace std;

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const Color green= {0, 102, 51, 255};

const int MAX_HOTELS = 60;
const int MAX_BOOKINGS = 50;
const int MAX_CITIES = 7;
const int MAX_GUESTS_PER_ROOM = 4;
const int TRAVEL_GOAL = 5;
const int MAX_CATEGORIES = 5;
const int MAX_PREFERRED_CITIES = 3;
//categories and cities strings
const string CATEGORIES[MAX_CATEGORIES] = {
    "Luxury", "Budget", "Business", "Resort", "Heritage"
};
const string CITIES[MAX_CITIES] = {
    "Islamabad", "Lahore", "Karachi", "Peshawar",
    "Quetta", "Gilgit", "Muzaffarabad"
};
const Color LIGHT = {249, 250, 251, 255};  //Gui colors 
const Color WHITE = {255, 255, 255, 255};    
const Color PakGreen = {0, 102, 51, 255}; 
const Color Red = {239, 68, 68, 255};    
const Color SuccessGreen = {34, 197, 94, 255};
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
    string checkInDate;
    string checkOutDate;
    int rooms;
};

struct UserProfile {
    string name;
    int totalBookings;
    float totalSpent;
    float maxBudget;
    bool budgetMode;
    int placesVisited;
    float travelerScore;
    int level;
    string preferredCities[MAX_PREFERRED_CITIES];
    int preferredCityCount;
};
// Global arrays
Hotel hotels[MAX_HOTELS];
int hotelCount = 0;
Booking bookings[MAX_BOOKINGS];
int bookingCount = 0;
Font appFont;
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
    appFont = LoadFontEx("C:/Windows/Fonts/arial.ttf", 48, 0, 250);//for arial font
    if (appFont.texture.id == 0) 
        appFont = GetFontDefault();
    SetTextureFilter(appFont.texture, TEXTURE_FILTER_BILINEAR);
    //Initialize data
    initializeHotels();
    user.name = "Traveler";
    user.totalBookings = 0;
    user.totalSpent = 0;
    user.maxBudget = 50000;
    user.budgetMode = true;
    user.placesVisited = 0;
    user.travelerScore = 0;
    user.level = 0;
    user.preferredCityCount = 0;
    // Main loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        
        ClearBackground(PakGreen)
    // Centered Title
    int titleWidth = MeasureText("MUSAFIR", 64);
    DrawText("MUSAFIR", (WINDOW_WIDTH - titleWidth) / 2, 280, 64, WHITE);

    // Centered Subtitle
    int subTitleWidth = MeasureText("Pakistan Travel Guide", 24);
    DrawText("Pakistan Travel Guide", (WINDOW_WIDTH - subTitleWidth) / 2, 360, 24, Color{200, 255, 200, 255});

    // Add the new info lines
    string info = "Hotels: " + to_string(hotelCount) + " | Bookings: " + to_string(bookingCount);
    DrawText(info.c_str(), (WINDOW_WIDTH - MeasureText(info.c_str(), 18)) / 2, 420, 18, WHITE);

    string cities = "Cities: ";
    for (int i = 0; i < MAX_CITIES; i++){
        cities += CITIES[i];
        if (i < MAX_CITIES - 1) cities += ", ";
        }
    DrawText(cities.c_str(), 50, 480, 14, Color{200, 255, 200, 255});
        
    string categories = "Categories: ";
    for (int i = 0; i < MAX_CATEGORIES; i++){
        categories += CATEGORIES[i];
        if (i < MAX_CATEGORIES - 1) categories += ", ";
        }
    DrawText(categories.c_str(), 50, 510, 14, Color{200, 255, 200, 255});
        
    const char* hint = "Press ESC to exit";
    int hintWidth = MeasureText(hint, 16);
    DrawText(hint, (WINDOW_WIDTH - hintWidth) / 2, 560, 16, Color{200, 255, 200, 255});
        
    // Cleanup
    CloseWindow();
    return 0;
}

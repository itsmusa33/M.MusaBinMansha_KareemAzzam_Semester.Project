#include "raylib.h"
#include <string>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <fstream> 
using namespace std;
//constants
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const int MAX_HOTELS = 60;
const int MAX_BOOKINGS = 50;
const int MAX_CITIES = 7;
const int MAX_GUESTS_PER_ROOM = 4;
const int TRAVEL_GOAL = 5;
const int MAX_CATEGORIES = 5;
const int MAX_PREFERRED_CITIES = 3;
const int MAX_WEATHER = 7;
const int MAX_VISITED_HOTELS = 100;
//categories and cities strings
const string CATEGORIES[MAX_CATEGORIES] = {
    "Luxury", "Budget", "Business", "Resort", "Heritage"
};

// Weather type constants: 0=normal, 1=rain, 2=festival
const int WEATHER_NORMAL = 0;
const int WEATHER_RAIN = 1;
const int WEATHER_FESTIVAL = 2;

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
struct Weather 
{
    string city;
    int type;              // 0=normal, 1=rain, 2=festival
    float priceMultiplier; // multiplier applied to hotel prices
};

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
//Budget planner settings
struct BudgetPlanner {
    bool enabled;
    float totalBudget;
    int plannedTrips;
    float spentInPlan;
    int tripsInPlan;
};
// Achievement badges
struct Achievements {
    bool frequentTraveler;
    bool budgetMaster;
    bool explorer;
};
// Notes for planner
const int MAX_NOTES = 5;
string plannerNotes[MAX_NOTES];
int noteCount = 0;
string newNoteInput = "";

//app screens
enum Screen {
    SCREEN_SPLASH,
    SCREEN_LOGIN,
    SCREEN_HOME,
    SCREEN_EXPLORE,
    SCREEN_DETAIL,
    SCREEN_BOOKINGS,
    SCREEN_MESSAGE,
    SCREEN_SEARCH,
    SCREEN_EDIT_BOOKING,
    SCREEN_SETTINGS,
	SCREEN_PLANNER,
    SCREEN_SUMMARY
};


// Global arrays
Hotel hotels[MAX_HOTELS];
int hotelCount = 0;

Weather weatherData[MAX_WEATHER];
int weatherCount = 0;

Booking bookings[MAX_BOOKINGS];
int bookingCount = 0;
Font appFont;
UserProfile user;
BudgetPlanner planner;

Achievements badges;
int destinationsTravelled = 0;


Screen currentScreen = SCREEN_SPLASH;
int selectedHotelIndex = -1;
string visitedHotelNames[MAX_VISITED_HOTELS];
string visitedHotelCities[MAX_VISITED_HOTELS];
int visitedHotelCount = 0;

int selectedBookingIndex = -1;
int scrollPosition = 0;
float splashTimer = 0;
string inputText = "";
string messageText = "";
//Search variables
string searchName = "";
string searchCity = "";
string searchCategory = "";
float searchMinPrice = 0;
float searchMaxPrice = 50000;
int inputFieldActive = 0;
//Booking form variables
int nights = 1;
int guests = 2;
int bookingDay = 20;
int bookingMonth = 12;
int bookingYear = 2025;
//App's current date (simulated)
int appDay = 20;
int appMonth = 12;
int appYear = 2025;
float dateTimer = 0;
//Edit booking variables
int editNights = 1;
int editGuests = 2;
int editDay = 20;
int editMonth = 12;
int editYear = 2025;

//helper functions
float clamp(float value, float minVal, float maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}
string formatDate(int day, int month, int year) {
    string d = (day < 10) ? "0" + to_string(day) : to_string(day);
    string m = (month < 10) ? "0" + to_string(month) : to_string(month);
    return d + "-" + m + "-" + to_string(year);
}
string generateBookingId() {
    int num = 1000 + rand() % 9000;
    return to_string(num);
}

bool containsIgnoreCase(string text, string search) 
{
    if (search.empty()) return true;
    if (text.empty()) return false;
    
    string textLower = "";
    for (int i = 0; i < (int)text.length(); i++)
    {
        textLower += tolower(text[i]);
    }

    string searchLower = "";
    for (int i = 0; i < (int)search.length(); i++) 
    {
        searchLower += tolower(search[i]);
    }
    
    int position = textLower.find(searchLower);
    return position != (int)string::npos;
}

// Parse a date string "DD-MM-YYYY" into day, month, year
void parseDate(string dateStr, int& day, int& month, int& year) {
    int dash1 = dateStr.find('-');
    int dash2 = dateStr.find('-', dash1 + 1);
    day = stoi(dateStr.substr(0, dash1));
    month = stoi(dateStr.substr(dash1 + 1, dash2 - dash1 - 1));
    year = stoi(dateStr.substr(dash2 + 1));
}

void calculateCheckoutDate(int startDay, int startMonth, int startYear, int numNights,
                           int& outDay, int& outMonth, int& outYear) {
    outDay = startDay + numNights;
    outMonth = startMonth;
    outYear = startYear;
while (outDay > 30) {
        outDay -= 30;
        outMonth++;
        if (outMonth > 12) {
            outMonth = 1;
            outYear++;
        }
    }
}

void drawScreenHeader(string title, Screen backScreen);  // Forward declaration
bool isDateInPast(int day, int month, int year) {
    if (year < appYear) return true;
    if (year > appYear) return false;
    if (month < appMonth) return true;
    if (month > appMonth) return false;
    return day < appDay;
}

bool isHotelVisited(string hotelName, string city) {
    for (int i = 0; i < visitedHotelCount; i++) {
        if (visitedHotelNames[i] == hotelName && visitedHotelCities[i] == city) {
            return true;
        }
    }
    return false;
}

void markHotelVisited(string hotelName, string city) {
    if (!isHotelVisited(hotelName, city) && visitedHotelCount < MAX_VISITED_HOTELS) {
        visitedHotelNames[visitedHotelCount] = hotelName;
        visitedHotelCities[visitedHotelCount] = city;
        visitedHotelCount++;
    }
}

bool isHotelBooked(string hotelName, string city) {
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].isActive && bookings[i].hotelName == hotelName && bookings[i].city == city) {
            return true;
        }
    }
    return false;
}


bool hasDateConflict(int startDay, int startMonth, int numNights, int excludeIndex = -1) {
    int endDay = startDay + numNights;
    int endMonth = startMonth;
    while (endDay > 30) {
        endDay -= 30;
        endMonth++;
    }

    for (int i = 0; i < bookingCount; i++) {
        if (i == excludeIndex) continue;
        if (!bookings[i].isActive) continue;
        
        int bDay, bMonth, bYear;
        parseDate(bookings[i].checkInDate, bDay, bMonth, bYear);
        int bEndDay, bEndMonth, bEndYear;
        parseDate(bookings[i].checkOutDate, bEndDay, bEndMonth, bEndYear);
        
        if (!(endDay < bDay || startDay > bEndDay)) {
            return true;
        }
       }
    return false;
}


//functions for color
Color getCategoryColor(string category) {
    if (category == "Luxury") return Color{168, 85, 247, 255};
    if (category == "Budget") return Color{34, 197, 94, 255};
    if (category == "Business") return Color{59, 130, 246, 255};
    if (category == "Resort") return Color{236, 72, 153, 255};
    if (category == "Heritage") return Color{234, 179, 8, 255};
    return Color{107, 114, 128, 255};
}

Color getCityColor(string city) {
    if (city == "Islamabad") return Color{34, 197, 94, 255};
    if (city == "Lahore") return Color{239, 68, 68, 255};
    if (city == "Karachi") return Color{59, 130, 246, 255};
    if (city == "Peshawar") return Color{234, 179, 8, 255};
    if (city == "Quetta") return Color{168, 85, 247, 255};
    if (city == "Gilgit") return Color{14, 165, 233, 255};
    if (city == "Muzaffarabad") return Color{236, 72, 153, 255};
    return Color{107, 114, 128, 255};
}

Color getWeatherColor(int weatherType) {
    if (weatherType == WEATHER_RAIN) return Color{100, 149, 237, 255};
    if (weatherType == WEATHER_FESTIVAL) return Color{255, 165, 0, 255};
    return Color{135, 206, 235, 255};
}

string getWeatherIcon(int weatherType) {
    if (weatherType == WEATHER_RAIN) return "~";
    if (weatherType == WEATHER_FESTIVAL) return "*";
    return "o";
}

string getWeatherName(int weatherType) {
    if (weatherType == WEATHER_RAIN) return "Rainy";
    if (weatherType == WEATHER_FESTIVAL) return "Festival";
    return "Normal";
}
//Preferred cities
bool isCityPreferred(string city) {
    for (int i = 0; i < user.preferredCityCount; i++) {
        if (user.preferredCities[i] == city) return true;
    }
    return false;
}
//select city
void togglePreferredCity(string city){
    int foundIndex = -1;
    for (int i = 0; i < user.preferredCityCount; i++){
        if (user.preferredCities[i] == city){
            foundIndex = i;
            break;
           }
    }

    if (foundIndex != -1){
        // Remove city
        for (int i = foundIndex; i < user.preferredCityCount - 1; i++){
            user.preferredCities[i] = user.preferredCities[i + 1];
        }
        user.preferredCityCount--;
    } else{
        // Add city if room
        if (user.preferredCityCount < MAX_PREFERRED_CITIES){
            user.preferredCities[user.preferredCityCount] = city;
            user.preferredCityCount++;
        }
    }
}

Color getLevelColor(int level){
    if (level == 0) return Color{205, 127, 50, 255};  // Bronze
    if (level == 1) return Color{192, 192, 192, 255}; // Silver
    return Color{255, 215, 0, 255};  // Gold
}

string getLevelName(int level){
    if (level == 0) return "Bronze";
    if (level == 1) return "Silver";
    return "Gold";
}
float getRemainingPlannerBudget() { //planner function
    if (!planner.enabled) return 999999;
    return planner.totalBudget - planner.spentInPlan;
}

void updateUserLevel() {
    if (user.totalSpent >= 100000) {
        user.level = 2;  // Gold
    } else if (user.totalSpent >= 50000) {
        user.level = 1;  // Silver
    } else {
        user.level = 0;  // Bronze
    }
}

void updateTravelerScore() {
    float bookingScore = user.totalBookings * 10.0f;
    float spendingScore = user.totalSpent / 1000.0f;
    float visitScore = user.placesVisited * 20.0f;
    user.travelerScore = bookingScore + spendingScore + visitScore;
}

void updateAchievements() {
    // Frequent Traveler: 3+ destinations visited
    if (destinationsTravelled >= 3) {
        badges.frequentTraveler = true;
    }

    // Explorer: 5+ hotels booked
    if (user.totalBookings >= 5) {
        badges.explorer = true;
    }

    // Budget Master: 3+ trips while staying under 70% of planner budget
    if (planner.enabled && user.totalBookings >= 3) {
        if (user.totalSpent <= planner.totalBudget * 0.70f) {
            badges.budgetMaster = true;
        }
    }
}


//helper functions for drawing gui elements
void drawText(string text, int x, int y, int size, Color color){
    DrawTextEx(appFont, text.c_str(), {(float)x, (float)y}, (float)size, 1, color);
}

int measureText(string text, int size){
    return (int)MeasureTextEx(appFont, text.c_str(), (float)size, 1).x;
}
//rounded box ui
void drawRoundedBox(int x, int y, int w, int h, Color color){
    DrawRectangle(x + 5, y, w - 10, h, color);
    DrawRectangle(x, y + 5, w, h - 10, color);
    DrawCircle(x + 5, y + 5, 5, color);
    DrawCircle(x + w - 5, y + 5, 5, color);
    DrawCircle(x + 5, y + h - 5, 5, color);
    DrawCircle(x + w - 5, y + h - 5, 5, color);
}
//button with text
bool drawButton(int x, int y, int w, int h, string text, Color color, int fontSize = 14){
    Rectangle rect = {(float)x, (float)y, (float)w, (float)h};
    bool isHovering = CheckCollisionPointRec(GetMousePosition(), rect);
    drawRoundedBox(x, y, w, h, isHovering ? Fade(color, 0.8f) : color);
    int textWidth = measureText(text, fontSize);
    drawText(text, x + (w - textWidth) / 2, y + (h - fontSize) / 2, fontSize, WHITE);
    return isHovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool drawSmallButton(int x, int y, int w, int h, string text, Color color){
    return drawButton(x, y, w, h, text, color, 12);
}

void handleTextInput(string& buffer, int maxLength){
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 126 && (int)buffer.length() < maxLength) {
            buffer += (char)key;
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && !buffer.empty()) {
        buffer.pop_back();
    }
}
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
    h.hasDeal = (rand() % 5 == 0);
    if (h.hasDeal) {
        h.dealPercent = 10 + rand() % 21;}
    else {
        h.dealPercent = 0;}
    hotels[hotelCount++] = h;
}

void initializeWeather() {
    weatherCount = 0;
    
    for (int i = 0; i < MAX_CITIES && weatherCount < MAX_WEATHER; i++) {
        Weather w;
        w.city = CITIES[i];
        
        int chance = rand() % 100;
        if (chance < 60) {
            w.type = WEATHER_NORMAL;
            w.priceMultiplier = 1.0f;
        } else if (chance < 85) {
            w.type = WEATHER_RAIN;
            w.priceMultiplier = 0.85f;
        } else {
            w.type = WEATHER_FESTIVAL;
            w.priceMultiplier = 1.25f;
        }
        
        weatherData[weatherCount] = w;
        weatherCount++;
    }
}
Weather* getWeatherForCity(string city) {
    for (int i = 0; i < weatherCount; i++) {
        if (weatherData[i].city == city) {
            return &weatherData[i];
        }
    }
    return nullptr;
}

float getPriceMultiplier(bool budgetMode) {
    if (budgetMode) {
        return 0.95f;
    } else {
        return 1.10f;
    }
}
void updateHotelPrices() {
    for (int i = 0; i < hotelCount; i++) {
        Hotel& h = hotels[i];
        
        float price = h.basePrice;
        
        Weather* w = getWeatherForCity(h.city);
        if (w != nullptr) {
            price *= w->priceMultiplier;
        }
        
        if (h.hasDeal) {
            price *= (1.0f - h.dealPercent / 100.0f);
        }
        
        price *= getPriceMultiplier(user.budgetMode);
        
        h.currentPrice = price;
    }
}

void advanceDay() {
    appDay++;
    if (appDay > 30) {
        appDay = 1;
        appMonth++;
        if (appMonth > 12) {
            appMonth = 1;
            appYear++;
        }
    }
    initializeWeather();
    updateHotelPrices();
}


void initializeHotels() {
    hotelCount = 0;
    //Islamabad
    addHotel("Serena Hotel", "Islamabad", "Luxury", 25000, 4.8f, true);
    addHotel("Marriott Islamabad", "Islamabad", "Luxury", 22000, 4.7f, true);
    addHotel("Roomy Signature", "Islamabad", "Business", 8000, 4.3f, false);
    addHotel("Envoy Continental", "Islamabad", "Business", 6000, 4.1f, false);
    addHotel("Hotel One", "Islamabad", "Budget", 4000, 3.9f, false);
    //Lahore
    addHotel("Pearl Continental", "Lahore", "Luxury", 20000, 4.6f, true);
    addHotel("Avari Hotel", "Lahore", "Luxury", 18000, 4.5f, true);
    addHotel("Nishat Hotel", "Lahore", "Business", 7000, 4.2f, false);
    addHotel("Luxus Grand", "Lahore", "Business", 9000, 4.4f, false);
    addHotel("Shalimar Hotel", "Lahore", "Heritage", 5000, 4.0f, false);
    //Karachi
    addHotel("Movenpick Karachi", "Karachi", "Luxury", 19000, 4.5f, true);
    addHotel("Pearl Continental KHI", "Karachi", "Luxury", 17000, 4.4f, true);
    addHotel("Avari Towers", "Karachi", "Business", 12000, 4.3f, true);
    addHotel("Beach Luxury Hotel", "Karachi", "Resort", 15000, 4.2f, true);
    addHotel("Regent Plaza", "Karachi", "Budget", 5000, 3.8f, false);
    // Peshawar
    addHotel("Pearl Continental PSH", "Peshawar", "Luxury", 14000, 4.3f, true);
    addHotel("Shelton Rezidor", "Peshawar", "Business", 8000, 4.1f, false);
    addHotel("Green's Hotel", "Peshawar", "Heritage", 4500, 4.0f, false);
    addHotel("Shelton Guest House", "Peshawar", "Budget", 2500, 3.7f, false);
    addHotel("Rose Hotel", "Peshawar", "Budget", 3000, 3.8f, false);
    // Quetta
    addHotel("Serena Quetta", "Quetta", "Luxury", 16000, 4.4f, true);
    addHotel("Lourdes Hotel", "Quetta", "Business", 5000, 4.0f, false);
    addHotel("Bloom Star Hotel", "Quetta", "Budget", 3000, 3.6f, false);
    addHotel("Hotel Imdad", "Quetta", "Budget", 2000, 3.5f, false);
    addHotel("Quetta Inn", "Quetta", "Business", 4000, 3.8f, false);
    // Gilgit
    addHotel("Serena Gilgit", "Gilgit", "Luxury", 18000, 4.6f, true);
    addHotel("PTDC Motel", "Gilgit", "Resort", 6000, 4.1f, false);
    addHotel("Riveria Hotel", "Gilgit", "Business", 4500, 4.0f, false);
    addHotel("Madina Hotel", "Gilgit", "Budget", 2500, 3.7f, false);
    addHotel("Mountain View", "Gilgit", "Resort", 8000, 4.3f, false);
    // Muzaffarabad
    addHotel("PC Muzaffarabad", "Muzaffarabad", "Luxury", 12000, 4.2f, false);
    addHotel("Hill View Hotel", "Muzaffarabad", "Resort", 7000, 4.1f, false);
    addHotel("Kashmir Continental", "Muzaffarabad", "Business", 5000, 3.9f, false);
    addHotel("Red Onion Hotel", "Muzaffarabad", "Budget", 3000, 3.7f, false);
    addHotel("Neelum View", "Muzaffarabad", "Resort", 6000, 4.0f, false);

    // Update prices based on weather after initialization
    updateHotelPrices();
}
//functions for booking
bool createBooking(int hotelIndex, int numNights, int numGuests) {
    if (hotelIndex < 0 || hotelIndex >= hotelCount) return false;
    if (bookingCount >= MAX_BOOKINGS) return false;
    
    Hotel& hotel = hotels[hotelIndex];
    int roomsNeeded = (numGuests + MAX_GUESTS_PER_ROOM - 1) / MAX_GUESTS_PER_ROOM;
    float totalCost = hotel.currentPrice * numNights * roomsNeeded;
    //Check budget
    if (user.maxBudget > 0 && (user.totalSpent + totalCost) > user.maxBudget) 
        return false;
    //Calculates checkout date
    int checkoutDay = bookingDay + numNights;
    int checkoutMonth = bookingMonth;
    int checkoutYear = bookingYear;
    while (checkoutDay > 30) {
        checkoutDay -= 30;
        checkoutMonth++;
        if (checkoutMonth > 12) { checkoutMonth = 1; checkoutYear++; }
    }
    Booking booking;
    booking.hotelName = hotel.name;
    booking.city = hotel.city;
    booking.bookingId = generateBookingId();
    booking.checkInDate = formatDate(bookingDay, bookingMonth, bookingYear);
    booking.checkOutDate = formatDate(checkoutDay, checkoutMonth, checkoutYear);
    booking.nights = numNights;
    booking.guests = numGuests;
    booking.rooms = roomsNeeded;
    booking.totalCost = totalCost;
    booking.isActive = true;

    bookings[bookingCount++] = booking;
    user.totalBookings++;
    user.totalSpent += totalCost;
    user.placesVisited++;
	updateUserLevel();
    updateTravelerScore();
    updateAchievements();

    //Track in planner
    if (planner.enabled){
        planner.spentInPlan += totalCost;
        planner.tripsInPlan++;
	}
    saveGame();
    return true;
}

int SearchHotels(int outResults[], int maxResults,
                 string name, string city,
                 string category, float minPrice, float maxPrice) {
    int count = 0;

    for (int i = 0; i < hotelCount && count < maxResults; i++) {
        Hotel h = hotels[i];
        if (!h.isActive) continue;

        bool nameMatch = containsIgnoreCase(h.name, name);
        bool cityMatch = containsIgnoreCase(h.city, city);
        bool categoryMatch = containsIgnoreCase(h.category, category);
        bool priceMatch = (h.currentPrice >= minPrice && h.currentPrice <= maxPrice);

        if (nameMatch && cityMatch && categoryMatch && priceMatch) {
            outResults[count] = i;
            count++;
        }
    }

    return count;
}


bool cancelBooking(int index) {
    if (index < 0 || index >= bookingCount) return false;
    if (!bookings[index].isActive) return false;
    
    user.totalSpent -= bookings[index].totalCost;
    user.totalBookings--;
    user.placesVisited--;
    bookings[index].isActive = false;
	updateUserLevel();
    updateTravelerScore();
    saveGame();

    return true;
}

void drawWeatherAlerts(int x, int y) {
    drawText("Weather Alerts:", x, y, 16, BLACK);
    
    int alertY = y + 30;
    int alertCount = 0;
    
    for (int i = 0; i < weatherCount && alertCount < 4; i++) {
        Weather& w = weatherData[i];
        
        if (w.type != WEATHER_NORMAL) {
            Color alertColor = getWeatherColor(w.type);
            string icon = getWeatherIcon(w.type);
            string weatherName = getWeatherName(w.type);
            
            drawRoundedBox(x, alertY, 280, 35, Fade(alertColor, 0.2f));
            drawRoundedBox(x, alertY, 5, 35, alertColor);
            
            drawText(icon, x + 15, alertY + 8, 18, alertColor);
            string alertText = w.city + ": " + weatherName;
            drawText(alertText, x + 40, alertY + 10, 14, DARKGRAY);
            
            string impact;
            if (w.type == WEATHER_RAIN) {
                impact = "-15%";
                drawText(impact, x + 230, alertY + 10, 14, SuccessGreen);
            } else if (w.type == WEATHER_FESTIVAL) {
                impact = "+25%";
                drawText(impact, x + 230, alertY + 10, 14, Red);
            }
		alertY += 42;
            alertCount++;
        }
    }
    
    if (alertCount == 0) {
        drawText("All cities have normal weather", x + 10, alertY, 14, GRAY);
    }
}

void drawWeatherIndicator(int x, int y, string city) {
    Weather* w = getWeatherForCity(city);
    if (w == nullptr) return;
    
    Color weatherColor = getWeatherColor(w->type);
    string icon = getWeatherIcon(w->type);
    string weatherName = getWeatherName(w->type);
    
    drawRoundedBox(x, y, 100, 28, Fade(weatherColor, 0.3f));
    drawText(icon, x + 10, y + 5, 16, weatherColor);
    drawText(weatherName, x + 30, y + 7, 12, DARKGRAY);
}

void saveGame() {
    ofstream file("musafir_save.txt");
    if (!file.is_open()) return;
    
    file << user.name << "\n";
    file << user.totalBookings << "\n";
    file << user.totalSpent << "\n";
    file << user.placesVisited << "\n";
    file << user.maxBudget << "\n";
    file << (user.budgetMode ? 1 : 0) << "\n";
    file << user.level << "\n";
	//Save planner
    file << (planner.enabled ? 1 : 0) << "\n";
    file << planner.totalBudget << "\n";
    file << planner.plannedTrips << "\n";
    file << planner.spentInPlan << "\n";
    file << planner.tripsInPlan << "\n";
    
    // Save notes
    file << noteCount << "\n";
    for (int i = 0; i < noteCount; i++) {
        file << plannerNotes[i] << "\n";
    }


	// Save achievements
    file << (badges.frequentTraveler ? 1 : 0) << "\n";
    file << (badges.budgetMaster ? 1 : 0) << "\n";
    file << (badges.explorer ? 1 : 0) << "\n";
    
    // Save destinations travelled
    file << destinationsTravelled << "\n";

	
    file << visitedHotelCount << "\n";
    for (int i = 0; i < visitedHotelCount; i++) {
        file << visitedHotelNames[i] << "\n";
        file << visitedHotelCities[i] << "\n";
    }
    
    int activeBookings = 0;
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].isActive) activeBookings++;
    }
    file << activeBookings << "\n";

for (int i = 0; i < bookingCount; i++) {
        const Booking& b = bookings[i];
        if (!b.isActive) continue;
        file << b.hotelName << "\n";
        file << b.city << "\n";
        file << b.bookingId << "\n";
        file << b.checkInDate << "\n";
        file << b.checkOutDate << "\n";
        file << b.nights << "\n";
        file << b.guests << "\n";
        file << b.rooms << "\n";
        file << b.totalCost << "\n";
    }
    
    file.close();
}

bool loadGame() {
    ifstream file("musafir_save.txt");
    if (!file.is_open()) return false;
    
    getline(file, user.name);
    file >> user.totalBookings;
    file >> user.totalSpent;
    file >> user.placesVisited;
    file >> user.maxBudget;
    int budgetMode;
    file >> budgetMode;
    user.budgetMode = (budgetMode == 1);
    file >> user.level;
	//Load planner
    int plannerEnabled;
    file >> plannerEnabled;
    planner.enabled = (plannerEnabled == 1);
    file >> planner.totalBudget;
    file >> planner.plannedTrips;
    file >> planner.spentInPlan;
    file >> planner.tripsInPlan;
    //Load notes
    file >> noteCount;
    file.ignore();
    if (noteCount < 0) noteCount = 0;
    if (noteCount > MAX_NOTES) noteCount = MAX_NOTES;
    for (int i = 0; i < noteCount; i++) {
        getline(file, plannerNotes[i]);
    } 

// Load achievements
    int ft, bm, ex;
    file >> ft >> bm >> ex;
    badges.frequentTraveler = (ft == 1);
    badges.budgetMaster = (bm == 1);
    badges.explorer = (ex == 1);
    
    // Load destinations travelled
    file >> destinationsTravelled;

	
    file >> visitedHotelCount;
    file.ignore();
    if (visitedHotelCount < 0) visitedHotelCount = 0;
    if (visitedHotelCount > MAX_VISITED_HOTELS) visitedHotelCount = MAX_VISITED_HOTELS;
    for (int i = 0; i < visitedHotelCount; i++) {
        getline(file, visitedHotelNames[i]);
        getline(file, visitedHotelCities[i]);
    }
      int loadedCount;
    file >> loadedCount;
    file.ignore();
    
    for (int i = 0; i < MAX_BOOKINGS; i++) {
        bookings[i].isActive = false;
    }

    if (loadedCount < 0) loadedCount = 0;
    if (loadedCount > MAX_BOOKINGS) loadedCount = MAX_BOOKINGS;

    bookingCount = 0;
    for (int i = 0; i < loadedCount; i++) {
        Booking b;
        getline(file, b.hotelName);
        getline(file, b.city);
        getline(file, b.bookingId);
        getline(file, b.checkInDate);
        getline(file, b.checkOutDate);
        file >> b.nights;
        file >> b.guests;
        file >> b.rooms;
        file >> b.totalCost;
        file.ignore();
        b.isActive = true;
		
	  if (bookingCount < MAX_BOOKINGS) {
            bookings[bookingCount] = b;
            bookingCount++;
        }
    }
    
    file.close();
    return !user.name.empty();
}

void initializeApp() {
    user.name = "";
    user.totalBookings = 0;
    user.totalSpent = 0;
    user.placesVisited = 0;
    user.travelerScore = 0;
    user.level = 0;
    user.maxBudget = 50000;
    user.budgetMode = true;
    user.preferredCityCount = 0;
    for (int i = 0; i < MAX_PREFERRED_CITIES; i++) user.preferredCities[i] = "";
    //Initialize planner
    planner.enabled = false;
    planner.totalBudget = 80000;
    planner.plannedTrips = 5;
    planner.spentInPlan = 0;
    planner.tripsInPlan = 0;
    
    //Clear notes
    for (int i = 0; i < MAX_NOTES; i++) plannerNotes[i] = "";
    noteCount = 0;
    newNoteInput = "";
	// No achievements yet
    badges.frequentTraveler = false;
    badges.budgetMaster = false;
    badges.explorer = false;
    
    // Reset destinations travelled
    destinationsTravelled = 0;


    visitedHotelCount = 0;
    for (int i = 0; i < MAX_VISITED_HOTELS; i++) {
        visitedHotelNames[i] = "";
        visitedHotelCities[i] = "";
    }
    
    initializeWeather();
    initializeHotels();

    bookingCount = 0;
    for (int i = 0; i < MAX_BOOKINGS; i++) bookings[i].isActive = false;
}

void drawScreenHeader(string title, Screen backScreen) {
    ClearBackground(LIGHT);
    DrawRectangle(0, 0, 1024, 60, WHITE);
    if (drawButton(15, 12, 40, 40, "<", GRAY)) {
        currentScreen = backScreen;
    }
    int titleWidth = measureText(title, 24);
    drawText(title, (1024 - titleWidth) / 2, 18, 24, BLACK);
}


//Functions for screen
void drawSplashScreen() {
    splashTimer += GetFrameTime();
    
    ClearBackground(PakGreen);
    
    float bounce = sinf(splashTimer * 3) * 10;
    DrawCircle(WINDOW_WIDTH / 2, 220 + (int)bounce, 60, WHITE);
    int faceWidth = measureText("(^.^)", 60);
    drawText("(^.^)", (WINDOW_WIDTH - faceWidth) / 2, 190 + (int)bounce, 60, Color{0, 102, 51, 255});
    
    int titleWidth = measureText("MUSAFIR", 64);
    drawText("MUSAFIR", (WINDOW_WIDTH - titleWidth) / 2, 330, 64, WHITE);
    int subTitleWidth = measureText("Pakistan Travel Guide", 24);
    drawText("Pakistan Travel Guide", (WINDOW_WIDTH - subTitleWidth) / 2, 410, 24, Color{200, 255, 200, 255});
    
    float progress = splashTimer / 2.0f;
    if (progress > 1.0f) progress = 1.0f;
    DrawRectangle((WINDOW_WIDTH - 200) / 2, 500, (int)(200 * progress), 10, WHITE);
    
    if (splashTimer >= 2.5f) {
        if (loadGame()) {
            initializeHotels();
            currentScreen = SCREEN_HOME;
        } else {
            initializeApp();
            currentScreen = SCREEN_LOGIN;
        }
    }
}


void drawLoginScreen(){
    ClearBackground(Color{240, 245, 240, 255});
    DrawRectangle(0, 0, 1024, 200, PakGreen); // Top green header
    
    drawText("MUSAFIR", 380, 55, 56, WHITE);
    drawText("Your Gateway to Adventure", 370, 130, 20, Color{200, 255, 200, 255});
    
    // Login "Card" area
    drawRoundedBox(200, 230, 624, 420, WHITE);
    
    // Name Input Field
    drawText("Enter your name:", 250, 260, 20, DARKGRAY);
    DrawRectangle(250, 295, 524, 45, Color{245, 245, 245, 255});
    DrawRectangleLines(250, 295, 524, 45, Color{0, 102, 51, 255});
    drawText(inputText, 265, 307, 20, BLACK);
    handleTextInput(inputText, 60);
    
    // Budget Selector (increments of 5000)
    drawText("Your Budget (Rs.):", 250, 360, 20, DARKGRAY);
    string budgetDisplay = "Rs. " + to_string((int)user.maxBudget);
    DrawRectangle(250, 395, 300, 45, Color{245, 245, 245, 255});
    drawText(budgetDisplay, 265, 407, 20, BLACK);
    
    if (drawSmallButton(560, 400, 40, 35, "-", GRAY) && user.maxBudget > 10000) user.maxBudget -= 5000;
    if (drawSmallButton(610, 400, 40, 35, "+", GRAY) && user.maxBudget < 500000) user.maxBudget += 5000;
    
    //Travel Style Toggle
    drawText("Travel Mode:", 250, 460, 18, DARKGRAY);
    
    Color budgetBtnColor = user.budgetMode ? SuccessGreen : GRAY;
    Color luxuryBtnColor = !user.budgetMode ? Color{168, 85, 247, 255} : GRAY;
    
    if (drawButton(250, 495, 140, 45, "Budget", budgetBtnColor)) {
        user.budgetMode = true;
        updateHotelPrices();
    }
    if (drawButton(410, 495, 140, 45, "Luxury", luxuryBtnColor)) {
        user.budgetMode = false;
        updateHotelPrices();
    }

    //Submit button
    if (drawButton(380, 570, 260, 55, "Start Journey", PakGreen)) {
        if (!inputText.empty()) {
            user.name = inputText;
            saveGame();
            currentScreen = SCREEN_HOME;
        }
    }

    // Submit button - requires a name to proceed
    if (drawButton(380, 570, 260, 55, "Start Journey", PakGreen)) {
        if (!inputText.empty()) {
            user.name = inputText;
            currentScreen = SCREEN_HOME;
        }
    }
}

void drawHomeScreen() {
    ClearBackground(LIGHT);
    DrawRectangle(0, 0, 1024, 80, WHITE); //Navbar
	// Today's date with simulation button
    string dateStr = "Today: " + formatDate(appDay, appMonth, appYear);
    drawText(dateStr, 40, 15, 14, GRAY);
    
    // Plus button to advance day
    if (drawSmallButton(200, 10, 25, 25, "+", PAKISTAN_GREEN)) {
        advanceDay();
    }

    drawText("MUSAFIR", 420, 10, 40, PakGreen); 
	
	// User level badge
    drawRoundedBox(870, 15, 130, 30, getLevelColor(user.level));
    drawText(getLevelName(user.level), 905, 20, 16, WHITE);

    //Personalized greeting and budget status
    drawText("Salam, " + user.name + "!", 40, 95, 22, BLACK);
    
    float remaining = user.maxBudget - user.totalSpent;
    string stats = "Budget: Rs." + to_string((int)user.maxBudget) + " | Spent: Rs." + to_string((int)user.totalSpent) + " | Remaining: Rs." + to_string((int)remaining); 
    // Turn the text red if the user is over budget
    drawText(stats, 40, 130, 14, remaining < 0 ? Red : GRAY);
	// Achievement badges
    int badgeX = 40;
    drawText("Badges:", badgeX, 55, 14, GRAY);
    badgeX += 65;

    if (badges.frequentTraveler) {
        drawRoundedBox(badgeX, 48, 130, 28, Color{147, 51, 234, 255});
        drawText("Frequent Traveler", badgeX + 8, 53, 14, WHITE);
        badgeX += 140;
    }
    if (badges.budgetMaster) {
        drawRoundedBox(badgeX, 48, 115, 28, SUCCESS_GREEN);
        drawText("Budget Master", badgeX + 8, 53, 14, WHITE);
        badgeX += 125;
    }
    if (badges.explorer) {
        drawRoundedBox(badgeX, 48, 85, 28, Color{59, 130, 246, 255});
        drawText("Explorer", badgeX + 10, 53, 14, WHITE);
        badgeX += 95;
    }
    if (!badges.frequentTraveler && !badges.budgetMaster && !badges.explorer) {
        drawText("none yet", badgeX, 55, 14, GRAY);
    }

	// Planner info if enabled
    if (planner.enabled) {
        float planRemaining = getRemainingPlannerBudget();
        string planText;
        if (planRemaining < 0) {
            planText = "Planner: Budget Plan Exceeded!";
        } else {
            planText = "Planner: Rs." + to_string((int)planRemaining) + " left";
        }
        Color planColor = planRemaining < 0 ? Red : GRAY;
        drawText(planText, 40, 155, 14, planColor);
    }
    //Main navigation buttons
    if (drawButton(40, 180, 200, 50, "Explore", Color{59, 130, 246, 255})) {
        scrollPosition = 0;
        currentScreen = SCREEN_EXPLORE;
    }
    if (drawButton(260, 180, 200, 50, "My Bookings", Color{34, 197, 94, 255})) currentScreen = SCREEN_BOOKINGS;

    if (drawButton(480, 180, 200, 50, "Search", Color{168, 85, 247, 255})) 
    {
    scrollPosition = 0;
    currentScreen = SCREEN_SEARCH;
    }
	if (drawButton(700, 180, 140, 50, "Planner", Color{59, 130, 246, 255})){//planner
        currentScreen = SCREEN_PLANNER;
    }
    //Settings button at bottom
    if (drawButton(40, 710, 130, 40, "Settings", GRAY)){
        currentScreen = SCREEN_SETTINGS;
    }

	if (drawButton(850, 710, 150, 40, "Summary", PAKISTAN_GREEN)) {
        currentScreen = SCREEN_SUMMARY;
    }


    // Recommendation List (Shows first 3 active hotels)
    drawText("Recommended For You", 40, 260, 18, BLACK);
    int y = 295;
    int count = 0;
    for (int i = 0; i < hotelCount && count < 3; i++) {
        const Hotel& h = hotels[i];
        if (!h.isActive) continue;
        
        drawRoundedBox(40, y, 940, 80, WHITE);
        drawRoundedBox(40, y, 8, 80, getCityColor(h.city)); //Color strip based on city
        drawText(h.name, 65, y + 12, 18, BLACK);
        
        string info = h.city + " | " + h.category + " | Rs." + to_string((int)h.currentPrice) + "/night";
        drawText(info, 65, y + 42, 14, GRAY);
        
        //Show a red badge if there's a discount
        if (h.hasDeal) {
            drawRoundedBox(840, y + 10, 80, 25, Red);
            drawText(to_string((int)h.dealPercent) + "% OFF!", 855, y + 15, 13, WHITE);
        }
        
        drawText(to_string(h.rating).substr(0, 3), 900, y + 40, 18, Color{234, 179, 8, 255}); // Star rating
        y += 90;
        count++;
    }
}

void drawExploreScreen() {
    ClearBackground(LIGHT);
    DrawRectangle(0, 0, 1024, 60, WHITE);
    
    if (drawButton(15, 12, 40, 40, "<", GRAY)) {
        currentScreen = SCREEN_HOME;
    }
    
    drawText("Explore Pakistan", (1024 - measureText("Explore Pakistan", 24)) / 2, 18, 24, BLACK);
    
    //City filter buttons
    int filterX = 40;
    bool allSelected = searchCity.empty();
    if (drawSmallButton(filterX, 70, 60, 30, "All", allSelected ? PakGreen : GRAY)) {
        searchCity = "";
    }
    filterX += 70;

    for (int i = 0; i < MAX_CITIES; i++) {
        bool isSelected = (searchCity == CITIES[i]);
        Color btnColor = isSelected ? getCityColor(CITIES[i]) : GRAY;
        if (drawSmallButton(filterX, 70, 110, 30, CITIES[i], btnColor)) {
            if (isSelected) {
                searchCity = "";
            } else {
                searchCity = CITIES[i];
            }
        }
        filterX += 118;
    }

    //Get filtered hotels
    int results[MAX_HOTELS];
    int resultCount = SearchHotels(results, MAX_HOTELS, "", searchCity, "", 0, 999999);

    int y = 115;
    int maxVisible = 6;

    for (int idx = scrollPosition; idx < resultCount && idx < scrollPosition + maxVisible; idx++) {
        int i = results[idx];
        const Hotel& h = hotels[i];
        
        drawRoundedBox(40, y, 940, 85, WHITE);
        drawRoundedBox(40, y, 8, 85, getCityColor(h.city));
        
        drawText(h.name, 65, y + 12, 18, BLACK);
		
		// Visited/Booked label
        int nameWidth = measureText(h.name, 18);
        if (isHotelVisited(h.name, h.city)) {
            drawRoundedBox(75 + nameWidth, y + 10, 60, 22, SUCCESS_GREEN);
            drawText("Visited", 82 + nameWidth, y + 13, 12, WHITE);
        } else if (isHotelBooked(h.name, h.city)) {
            drawRoundedBox(75 + nameWidth, y + 10, 60, 22, Color{59, 130, 246, 255});
            drawText("Booked", 82 + nameWidth, y + 13, 12, WHITE);
        

        string info = h.city + " | " + h.category + " | Rs." + to_string((int)h.currentPrice) + "/night";
        drawText(info, 65, y + 42, 14, GRAY);
        
        if (h.hasDeal) {
            string deal = to_string((int)h.dealPercent) + "% OFF!";
            drawRoundedBox(800, y + 10, 80, 25, Red);
            drawText(deal, 815, y + 15, 13, WHITE);
        }
        
        string rating = to_string(h.rating).substr(0, 3);
        drawText(rating, 900, y + 40, 18, Color{234, 179, 8, 255});
        
        Rectangle cardRect = {40, (float)y, 940, 85};
        if (CheckCollisionPointRec(GetMousePosition(), cardRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedHotelIndex = i;
            nights = 1;
            guests = 2;
            bookingDay = appDay;
            bookingMonth = appMonth;
            bookingYear = appYear;
            currentScreen = SCREEN_DETAIL;
        }
        
        y += 95;
    }

    //Scroll controls
    if (scrollPosition > 0 && drawButton(450, 720, 70, 35, "Up", GRAY)) {
        scrollPosition--;
    }
    if (scrollPosition + maxVisible < resultCount && drawButton(530, 720, 70, 35, "Down", GRAY)) {
        scrollPosition++;
    }

    int showing = resultCount - scrollPosition;
    if (showing > maxVisible) showing = maxVisible;
    string countText = "Showing " + to_string(showing) + " of " + to_string(resultCount);
    drawText(countText, 40, 725, 14, GRAY);
}

void drawDetailScreen(){
    // Safety check: if no hotel selected, kick back to home
    if (selectedHotelIndex < 0 || selectedHotelIndex >= hotelCount) {
        currentScreen = SCREEN_HOME; return; }
    Hotel& hotel = hotels[selectedHotelIndex];
    ClearBackground(LIGHT);
    //Large colorful header based on city color
    DrawRectangle(0, 0, 1024, 160, getCityColor(hotel.city));
    if (drawButton(15, 15, 40, 40, "<", Color{50, 50, 50, 255})) 
        currentScreen = SCREEN_EXPLORE;
    //Main Detail Card
    drawRoundedBox(30, 140, 964, 380, WHITE);
    drawText(hotel.name, 60, 165, 26, BLACK);
    drawText(hotel.city + ", Pakistan", 60, 200, 16, GRAY);
    drawText("Rating: " + to_string(hotel.rating).substr(0, 3), 800, 170, 18, Color{234, 179, 8, 255});
    
    //Tags (Category & Deals)
    drawRoundedBox(60, 235, 100, 30, getCategoryColor(hotel.category));
    drawText(hotel.category, 80, 242, 14, WHITE);
    if (hotel.hasDeal){
        drawRoundedBox(175, 235, 100, 30, Red);
        drawText(to_string((int)hotel.dealPercent) + "% OFF!", 195, 242, 14, WHITE);
    }
    //Amenities icons (simple text-based list)
    drawText("Amenities:", 60, 285, 16, BLACK);
    int amenX = 60;
    if (hotel.hasWifi) { drawText("WiFi", amenX, 315, 14, GRAY); amenX += 70; }
    if (hotel.hasPool) { drawText("Pool", amenX, 315, 14, GRAY); amenX += 70; }
    drawText("AC", amenX, 315, 14, GRAY);
    drawText("Price: Rs." + to_string((int)hotel.currentPrice) + "/night", 60, 380, 22, PakGreen);
    //Date selection
    drawText("Check-in Date:", 550, 285, 16, BLACK);
    string dateDisplay = formatDate(bookingDay, bookingMonth, bookingYear);
    drawText(dateDisplay, 700, 285, 16, Color{0, 102, 51, 255});
    
    drawText("Day:", 550, 320, 14, GRAY);
    if (drawSmallButton(600, 315, 30, 25, "-", GRAY) && bookingDay > 1) bookingDay--;
    drawText(to_string(bookingDay), 645, 320, 14, BLACK);
    if (drawSmallButton(680, 315, 30, 25, "+", GRAY) && bookingDay < 28) bookingDay++;
    
    drawText("Month:", 550, 355, 14, GRAY);
    if (drawSmallButton(620, 350, 30, 25, "-", GRAY) && bookingMonth > 1) bookingMonth--;
    drawText(to_string(bookingMonth), 665, 355, 14, BLACK);
    if (drawSmallButton(700, 350, 30, 25, "+", GRAY) && bookingMonth < 12) bookingMonth++;
    //Nights and Guests
    drawText("Nights:", 60, 360, 16, BLACK);
    if (drawSmallButton(140, 355, 35, 30, "-", GRAY) && nights > 1) nights--;
    drawText(to_string(nights), 190, 360, 16, BLACK);
    if (drawSmallButton(230, 355, 35, 30, "+", GRAY) && nights < 30) nights++;
    
    drawText("Guests:", 60, 405, 16, BLACK);
    if (drawSmallButton(140, 400, 35, 30, "-", GRAY) && guests > 1) guests--;
    drawText(to_string(guests), 190, 405, 16, BLACK);
    if (drawSmallButton(230, 400, 35, 30, "+", GRAY) && guests < 20) guests++;
    //Room calculation
    int roomsNeeded = (guests + MAX_GUESTS_PER_ROOM - 1) / MAX_GUESTS_PER_ROOM;
    string roomInfo = "Rooms needed: " + to_string(roomsNeeded) + " (max 4 guests/room)";
    drawText(roomInfo, 60, 450, 14, GRAY);
    //Price calculation
    float total = hotel.currentPrice * nights * roomsNeeded;
    drawRoundedBox(30, 540, 964, 90, PakGreen);
    
    string priceInfo = "Rs." + to_string((int)hotel.currentPrice) + "/night x " + 
                       to_string(nights) + " nights x " + to_string(roomsNeeded) + " rooms";
    drawText(priceInfo, 60, 555, 14, Color{200, 255, 200, 255});
    string totalText = "Total: Rs." + to_string((int)total);
    drawText(totalText, 60, 585, 28, WHITE);
    //Budget warning
    bool exceedsBudget = user.maxBudget > 0 && (user.totalSpent + total) > user.maxBudget;
    if (exceedsBudget){
        drawRoundedBox(550, 500, 280, 30, Red);
        drawText("Exceeds Your Budget!", 600, 507, 14, WHITE);
    }
    //Book button
    if (drawButton(750, 570, 220, 50, "Book Now", Color{34, 197, 94, 255})) {
        if (exceedsBudget){
            messageText = "Booking Failed!\nExceeds your budget: Rs." + to_string((int)user.maxBudget);
            currentScreen = SCREEN_MESSAGE;}
        else if (createBooking(selectedHotelIndex, nights, guests)){
            const Booking& b = bookings[bookingCount - 1];
            messageText = "Booking Confirmed!\nID: " + b.bookingId + 
                          "\n" + to_string(nights) + " nights, " + to_string(guests) + 
                          " guests (" + to_string(roomsNeeded) + " rooms)" +
                          "\nTotal: Rs." + to_string((int)b.totalCost);
            currentScreen = SCREEN_MESSAGE;
        } else {
            messageText = "Booking Failed!\nBudget limits exceeded.";
            currentScreen = SCREEN_MESSAGE;
        }
    }
}
void drawBookingsScreen() {
    ClearBackground(LIGHT);
    DrawRectangle(0, 0, 1024, 60, WHITE);
    
    if (drawButton(15, 12, 40, 40, "<", GRAY)) 
        currentScreen = SCREEN_HOME;
    drawText("My Bookings", 440, 18, 24, BLACK);
    //Header showing the current financial summary
    drawText("Your Budget: Rs." + to_string((int)user.maxBudget) + " | Spent: Rs." + to_string((int)user.totalSpent), 650, 28, 14, Color{0, 102, 51, 255});
    //Count active bookings
    int activeCount = 0;
    for (int i = 0; i < bookingCount; i++){
        if (bookings[i].isActive) activeCount++;
    }
    if (activeCount == 0){
        int noBookingsWidth = measureText("No bookings yet!", 26);
        drawText("No bookings yet!", (WINDOW_WIDTH - noBookingsWidth) / 2, 350, 26, GRAY);
        int exploreWidth = measureText("Start exploring Pakistan!", 20);
        drawText("Start exploring Pakistan!", (WINDOW_WIDTH - exploreWidth) / 2, 400, 20, GRAY);
        return;
    }
    int y = 85;
    int cardHeight = 130;
    
    for (int i = 0; i < bookingCount; i++){
        const Booking& b = bookings[i];
        if (!b.isActive) continue;
        drawRoundedBox(30, y, 960, cardHeight, WHITE);
        drawRoundedBox(30, y, 10, cardHeight, getCityColor(b.city));
        drawText(b.hotelName, 60, y + 15, 22, BLACK);
        //booking id
        string info = "Booking ID: " + b.bookingId + " | City: " + b.city;
        drawText(info, 60, y + 50, 16, GRAY);
        //booking details
        string details = to_string(b.nights) + " nights | " + 
                         to_string(b.guests) + " guests | " + 
                         to_string(b.rooms) + " rooms";
        drawText(details, 60, y + 75, 16, GRAY);
        
        string dates = b.checkInDate + " to " + b.checkOutDate;
        drawText(dates, 60, y + 100, 14, Color{0, 102, 51, 255});
        //total cost
        string total = "Rs." + to_string((int)b.totalCost);
        drawText(total, 800, y + 20, 24, Color{34, 197, 94, 255});

		// Edit button
        if (drawButton(810, y + 55, 80, 32, "Edit", Color{59, 130, 246, 255})) {
            selectedBookingIndex = i;
            editNights = b.nights;
            editGuests = b.guests;
            parseDate(b.checkInDate, editDay, editMonth, editYear);
            currentScreen = SCREEN_EDIT_BOOKING;
        }
        
        //Cancel button
        if (drawButton(810, y + 70, 80, 32, "Cancel", Red)) {
            if (cancelBooking(i)) {
                messageText = "Booking cancelled successfully!";
                currentScreen = SCREEN_MESSAGE;
            }
        }
        
        y += cardHeight + 10;
    }
}

void drawEditBookingScreen() {
    if (selectedBookingIndex < 0 || selectedBookingIndex >= bookingCount) {
        currentScreen = SCREEN_BOOKINGS;
        return;
    }
    
    Booking& booking = bookings[selectedBookingIndex];
    Hotel* hotel = nullptr;
    
    for (int i = 0; i < hotelCount; i++) {
        if (hotels[i].name == booking.hotelName && hotels[i].city == booking.city) {
            hotel = &hotels[i];
            break;
        }
    }
    
    if (hotel == nullptr) {
        currentScreen = SCREEN_BOOKINGS;
        return;
    }
    drawScreenHeader("Edit Booking", SCREEN_BOOKINGS);
    
    drawRoundedBox(30, 75, 964, 650, WHITE);
    
    drawText("Hotel: " + booking.hotelName, 60, 110, 18, BLACK);
    drawText("City: " + booking.city, 60, 140, 14, GRAY);
    
    drawText("Check-in Date:", 60, 180, 16, BLACK);
    string dateDisplay = formatDate(editDay, editMonth, editYear);
    drawText(dateDisplay, 400, 180, 16, PakGreen);
    
    drawText("Day:", 60, 220, 14, GRAY);
    if (drawSmallButton(120, 215, 30, 25, "-", GRAY) && editDay > 1) editDay--;
    drawText(to_string(editDay), 160, 220, 14, BLACK);
    if (drawSmallButton(190, 215, 30, 25, "+", GRAY) && editDay < 28) editDay++;
    
    drawText("Month:", 250, 220, 14, GRAY);
    if (drawSmallButton(320, 215, 30, 25, "-", GRAY) && editMonth > 1) editMonth--;
    drawText(to_string(editMonth), 360, 220, 14, BLACK);
    if (drawSmallButton(390, 215, 30, 25, "+", GRAY) && editMonth < 12) editMonth++;
    
    drawText("Nights:", 60, 270, 16, BLACK);
    if (drawSmallButton(140, 265, 35, 30, "-", GRAY) && editNights > 1) editNights--;
    drawText(to_string(editNights), 190, 270, 16, BLACK);
    if (drawSmallButton(230, 265, 35, 30, "+", GRAY) && editNights < 30) editNights++;

    drawText("Guests:", 400, 270, 16, BLACK);
    if (drawSmallButton(480, 265, 35, 30, "-", GRAY) && editGuests > 1) editGuests--;
    drawText(to_string(editGuests), 530, 270, 16, BLACK);
    if (drawSmallButton(570, 265, 35, 30, "+", GRAY) && editGuests < 20) editGuests++;
    
    int roomsNeeded = (editGuests + MAX_GUESTS_PER_ROOM - 1) / MAX_GUESTS_PER_ROOM;
    drawText("Rooms: " + to_string(roomsNeeded), 60, 320, 14, GRAY);
    
    int checkoutDay, checkoutMonth, checkoutYear;
    calculateCheckoutDate(editDay, editMonth, editYear, editNights, checkoutDay, checkoutMonth, checkoutYear);
    
    drawText("Checkout: " + formatDate(checkoutDay, checkoutMonth, checkoutYear), 60, 350, 14, GRAY);
    
    float newCost = hotel->currentPrice * editNights * roomsNeeded;
    
    drawRoundedBox(60, 400, 400, 80, PakGreen);
    drawText("New Total Cost", 100, 420, 14, WHITE);
    drawText("Rs." + to_string((int)newCost), 100, 450, 24, WHITE);
    
    float difference = newCost - booking.totalCost;
    Color diffColor = difference > 0 ? Red : SuccessGreen;
    drawRoundedBox(500, 400, 400, 80, Fade(diffColor, 0.2f));
    drawText("Difference", 540, 420, 14, DARKGRAY);
    string diffText = (difference > 0 ? "+" : "") + to_string((int)difference);
    drawText("Rs." + diffText, 540, 450, 24, diffColor);

    bool exceedsBudget = user.maxBudget > 0 && 
                         (user.totalSpent - booking.totalCost + newCost) > user.maxBudget;
    
    if (exceedsBudget) {
        drawRoundedBox(60, 510, 840, 30, Red);
        drawText("New total would exceed budget!", 100, 517, 14, WHITE);
    }
    
    if (drawButton(150, 580, 200, 50, "Update", Color{34, 197, 94, 255})) {
        if (!exceedsBudget) {
            user.totalSpent -= booking.totalCost;
            booking.nights = editNights;
            booking.guests = editGuests;
            booking.rooms = roomsNeeded;
            booking.totalCost = newCost;
            booking.checkInDate = formatDate(editDay, editMonth, editYear);
            booking.checkOutDate = formatDate(checkoutDay, checkoutMonth, checkoutYear);
            user.totalSpent += newCost;
            
            saveGame();
            messageText = "Booking updated successfully!";
            currentScreen = SCREEN_MESSAGE;
        } else {
            messageText = "Cannot update: Exceeds budget!";
            currentScreen = SCREEN_MESSAGE;
        }
    }
    if (drawButton(674, 580, 200, 50, "Cancel", Red)) {
        currentScreen = SCREEN_BOOKINGS;
    }
}


void drawMessageScreen(){
    ClearBackground(LIGHT);
    drawRoundedBox(262, 250, 500, 250, WHITE);
    int yPosition = 290;
    string currentLine = "";
    for (int i = 0; i < (int)messageText.length(); i++) {
        if (messageText[i] == '\n') {
            drawText(currentLine, 300, yPosition, 18, BLACK);
            yPosition += 30;
            currentLine = "";
        } else currentLine += messageText[i];
    }
    if (!currentLine.empty()) drawText(currentLine, 300, yPosition, 18, BLACK);
    if (drawButton(412, 420, 200, 50, "OK", PakGreen)) currentScreen = SCREEN_HOME;
}

void drawSearchScreen() {
    ClearBackground(LIGHT);
    DrawRectangle(0, 0, 1024, 60, WHITE);
    
    if (drawButton(15, 12, 40, 40, "<", GRAY)) {
        currentScreen = SCREEN_HOME;
    }
    
    drawText("Search Destinations", 420, 18, 24, BLACK);
    
    drawRoundedBox(40, 75, 420, 280, WHITE);
    drawText("Filters", 70, 95, 18, BLACK);
    
    drawText("Name:", 70, 137, 14, GRAY);
    DrawRectangle(140, 130, 200, 30, Color{245, 245, 245, 255});
    DrawRectangleLines(140, 130, 200, 30, inputFieldActive == 0 ? PakGreen : GRAY);
    drawText(searchName, 150, 137, 14, BLACK);
    Rectangle nameRect = {140, 130, 200, 30};
    if (CheckCollisionPointRec(GetMousePosition(), nameRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputFieldActive = 0;
        string searchName = "";
string searchCity = "";
string searchCategory = "";
float searchMinPrice = 0;
float searchMaxPrice = 50000;

    }
    
    drawText("City:", 70, 177, 14, GRAY);
    DrawRectangle(140, 170, 200, 30, Color{245, 245, 245, 255});
    DrawRectangleLines(140, 170, 200, 30, inputFieldActive == 1 ? PakGreen : GRAY);
    drawText(searchCity, 150, 177, 14, BLACK);
    Rectangle cityRect = {140, 170, 200, 30};
    if (CheckCollisionPointRec(GetMousePosition(), cityRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputFieldActive = 1;
    }
    
    drawText("Category:", 70, 217, 14, GRAY);
    DrawRectangle(140, 210, 200, 30, Color{245, 245, 245, 255});
    DrawRectangleLines(140, 210, 200, 30, inputFieldActive == 2 ? PakGreen : GRAY);
    drawText(searchCategory, 150, 217, 14, BLACK);
    Rectangle catRect = {140, 210, 200, 30};
    if (CheckCollisionPointRec(GetMousePosition(), catRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputFieldActive = 2;
    }
    
    drawText("Price Range:", 70, 260, 14, GRAY);
    string priceRange = "Rs." + to_string((int)searchMinPrice) + " - Rs." + to_string((int)searchMaxPrice);
    drawText(priceRange, 70, 285, 14, GRAY);
    
    if (drawSmallButton(70, 315, 100, 28, "Budget", GRAY)) {
        searchMinPrice = 0;
        searchMaxPrice = 5000;
    }
    if (drawSmallButton(180, 315, 100, 28, "Mid", GRAY)) {
        searchMinPrice = 5000;
        searchMaxPrice = 15000;
    }
    if (drawSmallButton(290, 315, 100, 28, "Luxury", GRAY)) {
        searchMinPrice = 15000;
        searchMaxPrice = 50000;
    }
    
    if (inputFieldActive == 0) handleTextInput(searchName, 60);
    if (inputFieldActive == 1) handleTextInput(searchCity, 30);
    if (inputFieldActive == 2) handleTextInput(searchCategory, 30);
    
    drawRoundedBox(480, 75, 500, 250, WHITE);
    drawText("Quick Select City:", 510, 95, 14, BLACK);
    int qx = 510, qy = 125;
    
    if (drawSmallButton(qx, qy, 50, 28, "All", searchCity.empty() ? PakGreen : GRAY)) {
        searchCity = "";
    }
    qx += 60;
    
    for (int i = 0; i < MAX_CITIES; i++) {
        if (drawSmallButton(qx, qy, 100, 28, CITIES[i], getCityColor(CITIES[i]))) {
            searchCity = CITIES[i];
        }
        qx += 110;
        if (qx > 900) { qx = 510; qy += 35; }
    }
    
    drawText("Quick Select Category:", 510, 220, 14, BLACK);
    qx = 510; qy = 250;
    for (int i = 0; i < MAX_CATEGORIES; i++) {
        if (drawSmallButton(qx, qy, 85, 28, CATEGORIES[i], getCategoryColor(CATEGORIES[i]))) {
            searchCategory = CATEGORIES[i];
        }
        qx += 95;
    }
    
    int results[MAX_HOTELS];
    int resultCount = SearchHotels(results, MAX_HOTELS, searchName, searchCity, searchCategory, searchMinPrice, searchMaxPrice);

    string resultText = "Found: " + to_string(resultCount) + " destinations";
    drawText(resultText, 40, 375, 16, BLACK);
    
    int maxVisible = 4;
    if (scrollPosition > 0 && drawButton(200, 370, 70, 30, "Up", GRAY)) {
        scrollPosition--;
    }
    if (scrollPosition + maxVisible < resultCount && drawButton(280, 370, 70, 30, "Down", GRAY)) {
        scrollPosition++;
    }
    
    if (scrollPosition >= resultCount) scrollPosition = 0;
    
    int y = 410;
    for (int idx = scrollPosition; idx < resultCount && idx < scrollPosition + maxVisible; idx++) {
        int i = results[idx];
        const Hotel& h = hotels[i];
        
        drawRoundedBox(40, y, 940, 75, WHITE);
        drawRoundedBox(40, y, 8, 75, getCityColor(h.city));
        
        drawText(h.name, 65, y + 10, 18, BLACK);
        string info = h.city + " | " + h.category + " | Rs." + to_string((int)h.currentPrice) + "/night";
        drawText(info, 65, y + 38, 14, GRAY);
        
        Rectangle cardRect = {40, (float)y, 940, 75};
        if (CheckCollisionPointRec(GetMousePosition(), cardRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedHotelIndex = i;
            nights = 1;
            guests = 2;
            bookingDay = appDay;
            bookingMonth = appMonth;
            bookingYear = appYear;
            currentScreen = SCREEN_DETAIL;
        }
        
        y += 85;
    }
}
// Planner screen
void drawPlannerScreen() {
    ClearBackground(LIGHT);
    DrawRectangle(0, 0, 1024, 60, WHITE);

    if (drawButton(15, 12, 40, 40, "<", GRAY)) {
        saveGame();
        currentScreen = SCREEN_HOME;
    }

    drawText("Travel Planner", 430, 18, 24, BLACK);

    //Planner settings panel
    drawRoundedBox(40, 80, 460, 400, WHITE);
    drawText("Budget Planner", 70, 110, 22, PakGreen);

    //Toggle button
    string toggleText = planner.enabled ? "Planner ON" : "Planner OFF";
    Color toggleColor = planner.enabled ? SuccessGreen : GRAY;
    if (drawButton(70, 155, 200, 45, toggleText, toggleColor)) {
        if (!planner.enabled) {
            planner.spentInPlan = 0;
            planner.tripsInPlan = 0;
        }
        planner.enabled = !planner.enabled;
    }

    //Reset button
    if (drawButton(300, 155, 150, 45, "Reset Plan", Red)) {
        planner.spentInPlan = 0;
        planner.tripsInPlan = 0;
    }

    //Total budget setting
    string budgetStr = "Total Budget: Rs." + to_string((int)planner.totalBudget);
    drawText(budgetStr, 70, 220, 18, BLACK);
    if (drawSmallButton(330, 215, 40, 35, "-", GRAY) && planner.totalBudget > 10000) {
        planner.totalBudget -= 5000;
    }
    if (drawSmallButton(380, 215, 40, 35, "+", GRAY) && planner.totalBudget < 500000) {
        planner.totalBudget += 5000;
    }

    //Planned trips setting
    string tripsStr = "Planned Trips: " + to_string(planner.plannedTrips);
    drawText(tripsStr, 70, 275, 18, BLACK);
    if (drawSmallButton(330, 270, 40, 35, "-", GRAY) && planner.plannedTrips > 1) {
        planner.plannedTrips--;
    }
    if (drawSmallButton(380, 270, 40, 35, "+", GRAY) && planner.plannedTrips < 20) {
        planner.plannedTrips++;
    }

    //Show stats when planner is ON
    if (planner.enabled) {
        string spentStr = "Spent in Plan: Rs." + to_string((int)planner.spentInPlan);
        drawText(spentStr, 70, 330, 16, GRAY);

        float remaining = getRemainingPlannerBudget();
        string remainStr;
        Color remainColor;
        if (remaining < 0) {
            remainStr = "Budget Plan Exceeded!";
            remainColor = Red;
        } else {
            remainStr = "Remaining: Rs." + to_string((int)remaining);
            remainColor = SuccessGreen;
        }
        drawText(remainStr, 70, 360, 18, remainColor);

        //Progress bar
        string progressStr = "Trips Planned: " + to_string(planner.tripsInPlan) + " / " + to_string(planner.plannedTrips);
        drawText(progressStr, 70, 400, 16, GRAY);

        DrawRectangle(70, 430, 400, 16, Color{230, 230, 230, 255});
        float tripProgress = clamp((float)planner.tripsInPlan / (float)planner.plannedTrips, 0, 1);
        DrawRectangle(70, 430, (int)(400 * tripProgress), 16, PakGreen);
    }

    //Notes section
    drawRoundedBox(520, 80, 470, 650, WHITE);
    drawText("Travel Notes", 550, 110, 22, PakGreen);

    int noteY = 160;
    for (int i = 0; i < noteCount && i < MAX_NOTES; i++) {
        drawRoundedBox(550, noteY, 400, 65, Color{245, 245, 245, 255});
        drawText(plannerNotes[i], 565, noteY + 22, 14, BLACK);

        if (drawSmallButton(920, noteY + 18, 30, 30, "X", Red)) {
            for (int j = i; j < noteCount - 1; j++) {
                plannerNotes[j] = plannerNotes[j + 1];
            }
            plannerNotes[noteCount - 1] = "";
            noteCount--;
        }
        noteY += 80;
    }

    //Add new note
    if (noteCount < MAX_NOTES) {
        drawText("Add Note:", 550, noteY + 30, 16, GRAY);
        DrawRectangle(660, noteY + 25, 250, 35, Color{245, 245, 245, 255});
        DrawRectangleLines(660, noteY + 25, 250, 35, PakGreen);
        drawText(newNoteInput, 670, noteY + 33, 14, BLACK);

        handleTextInput(newNoteInput, 100);

        if (drawSmallButton(660, noteY + 75, 80, 35, "Add", SuccessGreen)) {
            if (!newNoteInput.empty() && noteCount < MAX_NOTES) {
                plannerNotes[noteCount] = newNoteInput;
                noteCount++;
                newNoteInput = "";
            }
        }
    } else {
        drawText("Max 5 notes reached", 550, noteY + 30, 14, GRAY);
    }
}

// Settings screen
void drawSettingsScreen() {
    ClearBackground(LIGHT);
    DrawRectangle(0, 0, 1024, 60, WHITE);

    if (drawButton(15, 12, 40, 40, "<", GRAY)) {
        saveGame();
        currentScreen = SCREEN_HOME;
    }

    drawText("Settings", 450, 18, 24, BLACK);

    // Main settings panel
    drawRoundedBox(200, 85, 624, 400, WHITE);

    // User info
    drawText("Welcome, " + user.name, 240, 120, 20, BLACK);
    drawText("Level: " + getLevelName(user.level), 240, 150, 16, GRAY);

    // Budget setting
    drawText("Trip Budget Cap:", 240, 190, 16, BLACK);
    string budgetStr = "Rs. " + to_string((int)user.maxBudget);
    drawText(budgetStr, 450, 190, 18, PakGreen);

    if (drawButton(240, 220, 150, 40, "Increase", GRAY)){
        user.maxBudget += 5000;
    }
    if (drawButton(410, 220, 150, 40, "Decrease", GRAY) && user.maxBudget > 5000){
        user.maxBudget -= 5000;
    }

    // Travel mode
    drawText("Travel Mode:", 240, 285, 16, BLACK);

    Color budgetColor = user.budgetMode ? SuccessGreen : GRAY;
    Color luxuryColor = !user.budgetMode ? Color{168, 85, 247, 255} : GRAY;

    if (drawButton(240, 315, 150, 40, "Budget", budgetColor)){
        user.budgetMode = true;
        updateHotelPrices();
    }
    if (drawButton(410, 315, 150, 40, "Luxury", luxuryColor)){
        user.budgetMode = false;
        updateHotelPrices();
    }

    //Preferred cities
    drawText("Preferred Cities (max 3):", 240, 375, 16, BLACK);
    int px = 240;
    int py = 405;
    for (int i = 0; i < MAX_CITIES; i++){
        bool selected = isCityPreferred(CITIES[i]);
        Color cityBtnColor = selected ? getCityColor(CITIES[i]) : GRAY;
        if (drawSmallButton(px, py, 100, 30, CITIES[i], cityBtnColor)){
            togglePreferredCity(CITIES[i]);
        }
        px += 110;
        if (px > 700) { px = 240; py += 38; }
    }

    //Reset button
    if (drawButton(240, 520, 150, 45, "Reset All", Red)){
        string savedName = user.name;
        float savedBudget = user.maxBudget;
        initializeApp();
        user.name = savedName;
        user.maxBudget = savedBudget;
        saveGame();
        messageText = "All data has been reset!\n(Name kept)";
        currentScreen = SCREEN_MESSAGE;
    }

    // Save & Quit button
    if (drawButton(410, 520, 150, 45, "Save & Quit", SuccessGreen)){
        saveGame();
        CloseWindow();
      }
}

// ==================== SCREEN: SUMMARY ====================
void drawSummaryScreen() {
    ClearBackground(BG_LIGHT);
    DrawRectangle(0, 0, 1024, 60, BG_WHITE);
    
    if (drawButton(15, 12, 40, 40, "<", GRAY)) {
        currentScreen = SCREEN_HOME;
    }
    drawText("Session Summary", 420, 18, 24, BLACK);

    // Summary card
    drawRoundedBox(300, 120, 424, 520, BG_WHITE);

    // Stats
    drawText("Traveler: " + user.name, 340, 165, 18, BLACK);
    drawText("Destinations Planned: " + to_string(user.placesVisited), 340, 205, 18, BLACK);
    drawText("Destinations Travelled: " + to_string(destinationsTravelled), 340, 245, 18, BLACK);
    drawText("Total Spent: Rs." + to_string((int)user.totalSpent), 340, 285, 18, BLACK);

    // Level with color
    drawText("Level: " + getLevelName(user.level), 340, 325, 18, getLevelColor(user.level));
    drawText("Score: " + to_string((int)user.travelerScore), 340, 365, 18, BLACK);

    // Budget info
    float remaining = user.maxBudget - user.totalSpent;
    Color remColor = remaining < 0 ? DANGER_RED : SUCCESS_GREEN;
    drawText("Budget: Rs." + to_string((int)user.maxBudget), 340, 405, 16, GRAY);
    drawText("Remaining: Rs." + to_string((int)remaining), 340, 430, 16, remColor);

    // Planner info
    if (planner.enabled) {
        float planRemaining = getRemainingPlannerBudget();
        string planStr;
        if (planRemaining < 0) {
            planStr = "Planner: Budget Exceeded!";
        } else {
            planStr = "Planner: Rs." + to_string((int)planRemaining) + " left";
        }
        drawText(planStr, 340, 470, 16, planRemaining < 0 ? DANGER_RED : GRAY);
    }

    // Achievements
    string badgesStr = "Badges: ";
    bool anyBadge = false;
    if (badges.frequentTraveler) { badgesStr += "Frequent Traveler "; anyBadge = true; }
    if (badges.budgetMaster) { badgesStr += "Budget Master "; anyBadge = true; }
    if (badges.explorer) { badgesStr += "Explorer "; anyBadge = true; }
    if (!anyBadge) badgesStr += "None";
    drawText(badgesStr, 340, 510, 14, GRAY);

    drawText("Good job exploring Pakistan!", 340, 560, 18, PAKISTAN_GREEN);
}


int main(){
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Musafir - Pakistan Travel App"); //Initialize window
    SetTargetFPS(60);
    appFont = LoadFontEx("C:/Windows/Fonts/arial.ttf", 48, 0, 250);//for arial font
    if (appFont.texture.id == 0) 
        appFont = GetFontDefault();
    SetTextureFilter(appFont.texture, TEXTURE_FILTER_BILINEAR);
   srand((unsigned int)time(nullptr));
    // Initialize default user
    user.name = "";
    user.totalBookings = 0;
    user.totalSpent = 0;
    user.placesVisited = 0;
    user.travelerScore = 0;
    user.level = 0;
    user.maxBudget = 50000;
    user.budgetMode = true;
    user.preferredCityCount = 0;

	
	
    while (!WindowShouldClose()) {
        // Update app date timer
        dateTimer += GetFrameTime();
        if (dateTimer >= 120.0f) {  // Every 2 minutes
            dateTimer = 0;
            advanceDay();
        }
        
        // Check for completed trips
        for (int i = 0; i < bookingCount; i++) {
            if (!bookings[i].isActive) continue;

            int coDay, coMonth, coYear;
            parseDate(bookings[i].checkOutDate, coDay, coMonth, coYear);

            bool checkoutPassed = (appYear > coYear) ||
                (appYear == coYear && appMonth > coMonth) ||
                (appYear == coYear && appMonth == coMonth && appDay >= coDay);

            if (checkoutPassed) {
                markHotelVisited(bookings[i].hotelName, bookings[i].city);
                destinationsTravelled++;
                if (user.placesVisited > 0) {
                    user.placesVisited--;
                }
                bookings[i].isActive = false;
                updateAchievements();
                saveGame();
            }
        }
        
        BeginDrawing();

        switch (currentScreen) {
            case SCREEN_SPLASH:   drawSplashScreen(); break;
            case SCREEN_LOGIN:    drawLoginScreen(); break;
            case SCREEN_HOME:     drawHomeScreen(); break;
            case SCREEN_EXPLORE:  drawExploreScreen(); break;
            case SCREEN_DETAIL:   drawDetailScreen(); break;
            case SCREEN_BOOKINGS: drawBookingsScreen(); break;
            case SCREEN_MESSAGE: drawMessageScreen(); break;
            case SCREEN_SEARCH:drawSearchScreen();break;
			case SCREEN_EDIT_BOOKING:drawEditBookingScreen();break;
			case SCREEN_SETTINGS: drawSettingsScreen(); break;
			case SCREEN_PLANNER:  drawPlannerScreen(); break;
			case SCREEN_SUMMARY:  drawSummaryScreen(); break;
            default:drawHomeScreen(); break;
        }
    EndDrawing();
    }
    // Cleanup
    CloseWindow();
    return 0;
}

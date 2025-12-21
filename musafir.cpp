#include "raylib.h"
#include <string>
#include <cmath>
#include <ctime>
#include <cstdlib>
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
//app screens
enum Screen {
    SCREEN_SPLASH,
    SCREEN_LOGIN,
    SCREEN_HOME,
    SCREEN_EXPLORE,
    SCREEN_DETAIL,
    SCREEN_BOOKINGS,
    SCREEN_MESSAGE
};
// Global arrays
Hotel hotels[MAX_HOTELS];
int hotelCount = 0;
Booking bookings[MAX_BOOKINGS];
int bookingCount = 0;
Font appFont;
UserProfile user;

Screen currentScreen = SCREEN_SPLASH;
int selectedHotelIndex = -1;
int scrollPosition = 0;
float splashTimer = 0;
string inputText = "";
string messageText = "";
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
    return true;
}
bool cancelBooking(int index) {
    if (index < 0 || index >= bookingCount) return false;
    if (!bookings[index].isActive) return false;
    
    user.totalSpent -= bookings[index].totalCost;
    user.totalBookings--;
    user.placesVisited--;
    bookings[index].isActive = false;
    return true;
}
//Functions for screen
void drawSplashScreen() {
    splashTimer += GetFrameTime();
    ClearBackground(PAKISTAN_GREEN);
    //bouncing effect for the icon
    float bounce = sinf(splashTimer * 3) * 10;
    DrawCircle(WINDOW_WIDTH / 2, 220 + (int)bounce, 60, WHITE);
    //Musafir text and smily face
    int faceWidth = measureText("(^.^)", 60);
    drawText("(^.^)", (WINDOW_WIDTH - faceWidth) / 2, 190 + (int)bounce, 60, Color{0, 102, 51, 255});
    
    int titleWidth = measureText("MUSAFIR", 64);
    drawText("MUSAFIR", (WINDOW_WIDTH - titleWidth) / 2, 330, 64, WHITE);
    
    int subTitleWidth = measureText("Pakistan Travel Guide", 24);
    drawText("Pakistan Travel Guide", (WINDOW_WIDTH - subTitleWidth) / 2, 410, 24, Color{200, 255, 200, 255});
    
    //loading bar
    float progress = splashTimer / 2.0f;
    if (progress > 1.0f) progress = 1.0f;
    DrawRectangle((WINDOW_WIDTH - 200) / 2, 500, (int)(200 * progress), 10, WHITE);
    
    //after 3 sec load data and move to login
    if (splashTimer >= 2.5f){
        initializeHotels();
        currentScreen = SCREEN_LOGIN;
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
    
    // Travel Style Toggle
    drawText("Travel Mode:", 250, 460, 18, DARKGRAY);
    if (drawButton(250, 495, 140, 45, "Budget", user.budgetMode ? Color{34, 197, 94, 255} : GRAY)) user.budgetMode = true;
    if (drawButton(410, 495, 140, 45, "Luxury", !user.budgetMode ? Color{168, 85, 247, 255} : GRAY)) user.budgetMode = false;
    
    // Submit button - requires a name to proceed
    if (drawButton(380, 570, 260, 55, "Start Journey", PAKISTAN_GREEN)) {
        if (!inputText.empty()) {
            user.name = inputText;
            currentScreen = SCREEN_HOME;
        }
    }
}

void drawHomeScreen() {
    ClearBackground(BG_LIGHT);
    DrawRectangle(0, 0, 1024, 80, WHITE); //Navbar
    drawText("MUSAFIR", 420, 10, 40, PakGreen); 
    //Personalized greeting and budget status
    drawText("Salam, " + user.name + "!", 40, 95, 22, BLACK);
    
    float remaining = user.maxBudget - user.totalSpent;
    string stats = "Budget: Rs." + to_string((int)user.maxBudget) + " | Spent: Rs." + to_string((int)user.totalSpent) + " | Remaining: Rs." + to_string((int)remaining); 
    // Turn the text red if the user is over budget
    drawText(stats, 40, 130, 14, remaining < 0 ? DANGER_RED : GRAY);
    //Main navigation buttons
    if (drawButton(40, 180, 200, 50, "Explore", Color{59, 130, 246, 255})) {
        scrollPosition = 0;
        currentScreen = SCREEN_EXPLORE;
    }
    if (drawButton(260, 180, 200, 50, "My Bookings", Color{34, 197, 94, 255})) currentScreen = SCREEN_BOOKINGS;
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
            drawRoundedBox(840, y + 10, 80, 25, Red;
            drawText(to_string((int)h.dealPercent) + "% OFF!", 855, y + 15, 13, WHITE);
        }
        
        drawText(to_string(h.rating).substr(0, 3), 900, y + 40, 18, Color{234, 179, 8, 255}); // Star rating
        y += 90;
        count++;
    }
}

void drawExploreScreen(){
    ClearBackground(LIGHT);
    DrawRectangle(0, 0, 1024, 60, WHITE);
    if (drawButton(15, 12, 40, 40, "<", GRAY)) currentScreen = SCREEN_HOME; //Back button
    drawText("Explore Pakistan", (1024 - measureText("Explore Pakistan", 24)) / 2, 18, 24, BLACK);
    //Loop through hotels with a simple scroll offset (scrollPosition)
    int y = 85;
    int maxVisible = 7;
    int displayedCount = 0;
    for (int i = scrollPosition; i < hotelCount && displayedCount < maxVisible; i++) {
        const Hotel& h = hotels[i];
        if (!h.isActive) continue;
        //Card UI for each hotel
        drawRoundedBox(40, y, 940, 85, WHITE);
        drawRoundedBox(40, y, 8, 85, getCityColor(h.city));
        drawText(h.name, 65, y + 12, 18, BLACK);
        
        string info = h.city + " | " + h.category + " | Rs." + to_string((int)h.currentPrice) + "/night";
        drawText(info, 65, y + 42, 14, GRAY);
        
        if (h.hasDeal) {
            drawRoundedBox(800, y + 10, 80, 25, Red;
            drawText(to_string((int)h.dealPercent) + "% OFF!", 815, y + 15, 13, WHITE);
        }
        
        drawText(to_string(h.rating).substr(0, 3), 900, y + 40, 18, Color{234, 179, 8, 255});
        //If user clicks the card, go to the Detail Screen
        if (CheckCollisionPointRec(GetMousePosition(), {40, (float)y, 940, 85}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            selectedHotelIndex = i;
            currentScreen = SCREEN_DETAIL;
        }
        y += 95;
        displayedCount++;
    }
    //Scroll controls (Up/Down)
    if (scrollPosition > 0 && drawButton(450, 720, 70, 35, "Up", GRAY)) scrollPosition--;
    if (scrollPosition + maxVisible < hotelCount && drawButton(530, 720, 70, 35, "Down", GRAY)) scrollPosition++;
    
    drawText("Showing " + to_string(displayedCount) + " of " + to_string(hotelCount) + " hotels", 40, 725, 14, GRAY);
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
        drawRoundedBox(550, 500, 280, 30, DANGER_RED);
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
    drawText("Your Budget: Rs." + to_string((int)user.maxBudget) + " | Spent: Rs." + to_string((int)user.totalSpent), 650, 28, 14, Color{0, 102, 51, 255})
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
        
        //Cancel button
        if (drawButton(810, y + 70, 80, 32, "Cancel", DANGER_RED)) {
            if (cancelBooking(i)) {
                messageText = "Booking cancelled successfully!";
                currentScreen = SCREEN_MESSAGE;
            }
        }
        
        y += cardHeight + 10;
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
        BeginDrawing();
        switch (currentScreen) {
            case SCREEN_SPLASH:   drawSplashScreen(); break;
            case SCREEN_LOGIN:    drawLoginScreen(); break;
            case SCREEN_HOME:     drawHomeScreen(); break;
            case SCREEN_EXPLORE:  drawExploreScreen(); break;
            case SCREEN_DETAIL:   drawDetailScreen(); break;
            case SCREEN_BOOKINGS: drawBookingsScreen(); break;
            case SCREEN_MESSAGE: drawMessageScreen(); break;
            default:              drawHomeScreen(); break;
        }
    EndDrawing();
    }
    // Cleanup
    CloseWindow();
    return 0;
}

#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <string>
#include "httplib.h" // Include the cpp-httplib header for HTTP requests
#include "include/nlohmann/json.hpp" // Include the nlohmann/json library for JSON parsing


// Thread-safe structure to store real-time prices
class RealTimePrices {
private:
    std::vector<double> prices; // Vector to store stock prices
    std::mutex mtx;             // Mutex to ensure thread-safe access

public:
    // Add price to the vector
    void addPrice(double price) {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex for thread safety
        prices.push_back(price);
    }

    // Print all prices (for debugging)
    void printPrices() {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex for thread safety
        for (const auto& price : prices) {
            std::cout << price << " ";
        }
        std::cout << std::endl;
    }
};

// Worker function to fetch stock prices from Finnhub API
void fetchStockPrices(RealTimePrices& priceStore, const std::string& symbol, const std::string& apiKey) {
    httplib::Client cli("https://finnhub.io");
    std::string endpoint = "/api/v1/quote?symbol=" + symbol + "&token=" + apiKey;

    auto res = cli.Get(endpoint.c_str());
    if (res && res->status == 200) {
        // Parse the response (assuming JSON format)
        auto json = nlohmann::json::parse(res->body);
        double currentPrice = json["c"]; // "c" is the current price in Finnhub's response
        priceStore.addPrice(currentPrice);
        std::cout << "Fetched price for " << symbol << ": " << currentPrice << std::endl;
    } else {
        std::cerr << "Failed to fetch price for " << symbol << std::endl;
    }
}

int main() {
    RealTimePrices priceStore; // Create an instance of RealTimePrices
    std::string apiKey = "cvqogt9r01qp88cms3i0cvqogt9r01qp88cms3ig"; // Your API key

    // Symbols to fetch prices for
    std::vector<std::string> symbols = {"AAPL", "GOOGL", "MSFT"};

    // Create threads to fetch prices for different symbols
    std::vector<std::thread> threads;
    for (const auto& symbol : symbols) {
        threads.emplace_back(fetchStockPrices, std::ref(priceStore), symbol, apiKey);
    }

    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }

    // Print all fetched prices
    std::cout << "All fetched prices: ";
    priceStore.printPrices();

    return 0;
}



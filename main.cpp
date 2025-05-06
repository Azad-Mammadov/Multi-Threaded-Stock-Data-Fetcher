#include <thread>
#include <iostream>
#include <vector>
#include <mutex>

// Thread-safe structure to store real-time prices
class RealTimePrices {

	private:
		std::vector<double> prices;
		std::mutex mtx;

	public:
	// Add price to the vector
	void addPrice(double price) {
		std::lock_guard<std::mutex> lock(mtx); // Lock the mutex for thread safety
		prices.push_back(price);
	}

	// Retrieve all prices (thread-safe copy)
	std::vector<double> getPrices() {
		std::lock_guard<std::mutex> lock(mtx); // Lock the mutex for thread safety
		return prices; // Return a copy of the prices vector
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


void workFunc(int* ptr, size_t times)
{
	while(times--) {
		*ptr += 1;
	}
}


int main(int argc, char const *argv[])
{
	int* numbers = new int[3];

	std::thread t1(workFunc, numbers, 500);
	std::thread t2(workFunc, numbers + 1, 600);
	std::thread t3(workFunc, numbers + 2, 700);

	// Join threads to ensure the main thread waits for their completion
	// .join() blocks the main thread until the respective thread finishes execution.
	t1.join();
	t2.join();
	t3.join();

	for(int i = 0; i < 3; ++i)
		std::cout << "Number Slot " << (int)i << " is " << (int)numbers[i] << std::endl;

	delete[] numbers;
	return 0;
}



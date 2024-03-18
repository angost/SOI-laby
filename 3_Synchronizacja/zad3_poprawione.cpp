#include <iostream>
#include <vector>
#include <string>
#include "monitor.h"
#include<unistd.h>

int const threadsCounts = 4;  //liczba watkow
int const bufferSize = 7;

// Zlicza chetnych konsumentow
class Counter {
private:
    Semaphore sem;
    int counter = 0;
public:
    Counter() : sem(1)
	{
	}

	void increment() {
		sem.p();
        counter++;
		sem.v();
	}

	void decrement() {
		sem.p();
        counter--;
		sem.v();
	}

	int get() {
		sem.p();
        int value = counter;
		sem.v();
		return value;
	}
};

class Buffer
{
private:
	Semaphore mutex;
	Semaphore prodStop;
	Semaphore konsStop;
	int prodCzeka = 0;
	int konsCzeka = 0;
	std::vector<int> values;
    Counter countCons;

	void print(std::string name, int action_nr)
	{
		std::cout << "\n### " << name << " action " << action_nr << " [";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "]###\n";
	}

public:
	Buffer() : mutex(1), prodStop(0), konsStop(0)
	{
	}

	bool prodMoze()
	{
        return ((countCons.get() == 0 || values.size() == 0) && values.size() < bufferSize); // || konsCzeka > 0;
	}

    bool konsMoze()
	{
        return values.size() > 0;
	}


	void put(int value, std::string name)
	{
		mutex.p();
        if (!prodMoze()){
            // wyjdz z obu sekcji i czekaj az bedzie mogl
            std::cout << "Prod " + name + " sprawdzil czy moze" << "\n";
            prodCzeka++;
			mutex.v();
            prodStop.p();
			prodCzeka--;
        } else {
            std::cout << "Prod " + name + " sprawdzil czy moze" << "\n";
        }
        // produkuj
        values.push_back(value);
		print("Prod " + name, value);

        if (konsCzeka > 0) { // bo bufor byl pusty
            konsStop.v(); // przekaz sekcje
        } else if (prodCzeka > 0 && prodMoze()) {
			// wznawia drugiego procucenta, jesli ten moze i czeka
			prodStop.v();
		} else {
            mutex.v();
        }
	}

	int get(int value, std::string name)
	{
        countCons.increment();
        std::cout << "Cons " + name + " sie zglosil" << "\n";

        mutex.p();
        if (!konsMoze()) { // bo vector.size == 0
            // if (prodCzeka > 0) {
            //     prodStop.v(); // ->prod moze, przekazanie sekcji
            // } else {
            //     mutex.v();
            // }
			konsCzeka++;
			mutex.v();
            konsStop.p();
			konsCzeka--;
        }
        // konsumuj
        int v = values.front();
        values.erase(values.begin());
		print("Cons" + name, value);

        // zmniejszamy liczbe czekajacych konsumentow
        countCons.decrement();

        // prod ustapil konsumentim ALBO bufor byl pelny, wiec musial poczekac
        if (prodMoze() && prodCzeka > 0) {
            // -> przekazujemy mu sk
            prodStop.v();
        } else {
            mutex.v();
        }
        return v;
    }

};

Buffer buffer;

void* threadProdA(void* arg)
{
	for (int i = 0; i < 10; ++i)
	{
		buffer.put(i, "A");
        // sleep(1);
	}
	std::cout << "### Koniec producenta A ###\n";
	return NULL;
}

void* threadProdB(void* arg)
{
	for (int i = 0; i < 10; ++i)
	{
		buffer.put(i, "B");
        // sleep(1);
	}
	std::cout << "### Koniec producenta B ###\n";
	return NULL;
}

void* threadConsA(void* arg)
{
	for (int i = 0; i < 10; ++i)
	{
		auto value = buffer.get(i, "A");
	}
	std::cout << "### Koniec konsumenta A ###\n";
	return NULL;
}

void* threadConsB(void* arg)
{
	for (int i = 0; i < 10; ++i)
	{
		auto value = buffer.get(i, "B");
	}
	std::cout << "### Koniec konsumenta B ###\n";
	return NULL;
}

int main()
{
#ifdef _WIN32
	HANDLE tid[threadsCounts];
	DWORD id;

	// utworzenie w�tk�w
	tid[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProdA, 0, 0, &id);
	tid[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProdB, 0, 0, &id);
	tid[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsA, 0, 0, &id);
	tid[3] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsB, 0, 0, &id);

	// czekaj na zako�czenie w�tk�w
	for (int i = 0; i <= threadsCounts; i++)
		WaitForSingleObject(tid[i], INFINITE);
#else
	pthread_t tid[threadsCounts];

	// utworzenie w�tk�w
	pthread_create(&(tid[0]), NULL, threadProdA, NULL);
	pthread_create(&(tid[1]), NULL, threadProdB, NULL);
	pthread_create(&(tid[2]), NULL, threadConsA, NULL);
	pthread_create(&(tid[3]), NULL, threadConsB, NULL);

	//czekaj na zako�czenie w�tk�w
	for (int i = 0; i < threadsCounts; i++)
		pthread_join(tid[i], (void**)NULL);
#endif
	return 0;
}


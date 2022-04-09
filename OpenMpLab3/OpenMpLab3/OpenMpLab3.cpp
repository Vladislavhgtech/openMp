#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <omp.h>


void atomic(int vSize, std::vector<short> vec1, std::vector<short> vec2) {
	unsigned long long newSum = 0;
	int sum;
	int i, j;
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;

	std::chrono::nanoseconds time;

	begin = std::chrono::steady_clock::now();

#pragma omp parallel shared (newSum) private (i, sum)
	{ int sum = 0;
#pragma omp for 
	for (int i = 0; i < vSize; i++) {

		if ((vec1.at(i) != 0) && (vec2.at(i) != 0)) {

			sum = sum + (static_cast <unsigned long long> (vec1[i]) + static_cast <unsigned long long> (vec2[i]));
		}
	}
#pragma omp atomic
	newSum += sum;
	}
	end = std::chrono::steady_clock::now();

	time = std::chrono::duration_cast <std::chrono::nanoseconds> (end - begin);

	std::cout << "Результаты параллельного выполнения c atomic  " << newSum << std::endl;
	std::cout << "Время последовательного выполнения  c atomic  " << time.count() << " nanoseconds" << std::endl;

}


void critical(int vSize, std::vector<short> vec1, std::vector<short> vec2) {
	unsigned long long newSum = 0;
	int sum;
	int i, j;
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;

	std::chrono::nanoseconds time;

	begin = std::chrono::steady_clock::now();

#pragma omp parallel shared (newSum) private (i, sum)
	{ int sum = 0;
#pragma omp for 
			for (int i = 0; i < vSize; i++) {

		if ((vec1.at(i) != 0) && (vec2.at(i) != 0)) {

			sum = sum + (static_cast <unsigned long long> (vec1[i]) + static_cast <unsigned long long> (vec2[i]));
		}
	}
#pragma omp critical
				newSum += sum;
			}
	end = std::chrono::steady_clock::now();

	time = std::chrono::duration_cast <std::chrono::nanoseconds> (end - begin);

	std::cout << "Результаты параллельного выполнения c critical  " << newSum << std::endl;
	std::cout << "Время последовательного выполнения  c critical  " << time.count() << " nanoseconds" << std::endl;

}
	

long work(const std::vector<short>& first_array, const std::vector<short>& second_array, int begin, int end) {
	long result = 0;
	long sum = 0;

	for (int i = begin; i < end; i++) {

		if ((first_array.at(i) != 0) && (second_array.at(i) != 0)) {

			sum = sum + (static_cast <unsigned long long> (first_array[i]) + static_cast <unsigned long long> (second_array[i]));
		}
	}

	return sum;
}

long section_2(const std::vector<short>& first_array, const std::vector<short>& second_array) {
	int size = static_cast<int>(first_array.size());
	int chunk = static_cast<int>(size / 2);
	long result = 0;

	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;

	std::chrono::nanoseconds time;

	begin = std::chrono::steady_clock::now();


#pragma omp parallel default(none) shared(first_array, second_array, result, chunk, size)
#pragma omp sections reduction(+: result)
	{
#pragma omp section
		result += work(first_array, second_array, 0, chunk);

#pragma omp section
		result += work(first_array, second_array, chunk, size);
	}

	

	end = std::chrono::steady_clock::now();

	time = std::chrono::duration_cast <std::chrono::nanoseconds> (end - begin);

	std::cout << "Результаты последовательного section " << result << std::endl;
	std::cout << "Время последовательного выполнения  c sections 2  " << time.count() << " nanoseconds" << std::endl;

	return result;
}


long section_4(const std::vector<short>& first_array, const std::vector<short>& second_array) {
	int size = static_cast<int>(first_array.size());
	int chunk = static_cast<int>(size / 4);
	long result = 0;

	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;

	std::chrono::nanoseconds time;

	begin = std::chrono::steady_clock::now();

#pragma omp parallel default(none) shared(first_array, second_array, result, chunk, size)
#pragma omp sections reduction(+: result)
	{
#pragma omp section
		result += work(first_array, second_array, 0, chunk);

#pragma omp section
		result += work(first_array, second_array, chunk, 2 * chunk);

#pragma omp section
		result += work(first_array, second_array, 2 * chunk, 3 * chunk);

#pragma omp section
		result += work(first_array, second_array, 3 * chunk, size);
	}

	end = std::chrono::steady_clock::now();

	time = std::chrono::duration_cast <std::chrono::nanoseconds> (end - begin);

	std::cout << "Результаты последовательного section " << result << std::endl;
	std::cout << "Время последовательного выполнения  c sections 4  " << time.count() << " nanoseconds" << std::endl;


	return result;
}

long run_semaphore(const std::vector<short>& vec1, const std::vector<short>& vec2, int vSize) {
	omp_lock_t lock;
	omp_init_lock(&lock);
	unsigned long long sum = 0;
	unsigned long long newSum = 0;
	int i, j;
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;

	std::chrono::nanoseconds time;

	begin = std::chrono::steady_clock::now();

#pragma omp parallel shared (newSum) private (i, sum)
	{ int sum = 0;
#pragma omp for 
	for (int i = 0; i < vSize; i++) {

		if ((vec1.at(i) != 0) && (vec2.at(i) != 0)) {

			sum = sum + (static_cast <unsigned long long> (vec1[i]) + static_cast <unsigned long long> (vec2[i]));
		}
	}
	omp_set_lock(&lock);
#pragma omp critical
	newSum += sum;
	omp_unset_lock(&lock);
	}

	omp_destroy_lock(&lock);

	end = std::chrono::steady_clock::now();

	time = std::chrono::duration_cast <std::chrono::nanoseconds> (end - begin);

	std::cout << "Результаты параллельного выполнения c lock  " << newSum << std::endl;
	std::cout << "Время последовательного выполнения  c lock  " << time.count() << " nanoseconds" << std::endl;
}


long barier(const std::vector<short>& vec1, const std::vector<short>& vec2, int vSize) {
	unsigned long long sum = 0;
	unsigned long long newSum = 0;
	int i, j;
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;

	std::chrono::nanoseconds time;

	begin = std::chrono::steady_clock::now();

#pragma omp parallel shared (newSum) private (i, sum)
	{ int sum = 0;
#pragma omp for 
	for (int i = 0; i < vSize; i++) {

		if ((vec1.at(i) != 0) && (vec2.at(i) != 0)) {

			sum = sum + (static_cast <unsigned long long> (vec1[i]) + static_cast <unsigned long long> (vec2[i]));
		}
	}
#pragma omp critical
	newSum += sum;
	}
#pragma omp barrier


	end = std::chrono::steady_clock::now();

	time = std::chrono::duration_cast <std::chrono::nanoseconds> (end - begin);

	std::cout << "Результаты параллельного выполнения c barier  " << newSum << std::endl;
	std::cout << "Время последовательного выполнения  c barier  " << time.count() << " nanoseconds" << std::endl;
}



int main() {

	setlocale(LC_ALL, "Russian");

	const int highLevel = 1;
	const int vSize = 100000000;

	unsigned long long sum = 0;

	std::random_device dev;
	std::mt19937 png(dev());
	std::uniform_int_distribution <std::mt19937::result_type> dist(0, highLevel);

	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;

	std::chrono::nanoseconds time1;
	std::chrono::nanoseconds time2;

	std::vector <short> vec1(vSize);
	std::vector <short> vec2(vSize);

	// Инициализация векторов

	for (std::vector <short> ::iterator i = vec1.begin(); i != vec1.end(); i++) {

		*i = dist(png);
	}

	for (std::vector <short> ::iterator i = vec2.begin(); i != vec2.end(); i++) {

		*i = dist(png);
	}


	// Последовательное выполнение

	begin = std::chrono::steady_clock::now();

	for (int i = 0; i < vSize; i++) {

		if ((vec1.at(i) != 0) && (vec2.at(i) != 0)) {

			sum = sum + (static_cast <unsigned long long> (vec1[i]) + static_cast <unsigned long long> (vec2[i]));
		}
	}

	end = std::chrono::steady_clock::now();

	time1 = std::chrono::duration_cast <std::chrono::nanoseconds> (end - begin);

	std::cout << "Результаты последовательного выполнения " << sum << std::endl;


	sum = 0;


	// Параллельное выполнение

	begin = std::chrono::steady_clock::now();

#pragma omp parallel for reduction(+: sum)
	for (int i = 0; i < vSize; i++) {

		if ((vec1.at(i) != 0) && (vec2.at(i) != 0)) {

			sum = sum + (static_cast <unsigned long long> (vec1[i]) + static_cast <unsigned long long> (vec2[i]));
		}
	}

	end = std::chrono::steady_clock::now();

	time2 = std::chrono::duration_cast <std::chrono::nanoseconds> (end - begin);

	std::cout << "Результаты параллельного выполнения " << sum << std::endl;

	// Параллельное выполнение

	std::cout << "Время последовательного выполнения  " << time1.count() << " nanoseconds" << std::endl;
	std::cout << "Время параллельного выполнения  " << time2.count() << " nanoseconds" << std::endl;

	critical(vSize, vec1,vec2);
	atomic(vSize, vec1, vec2);
	section_2(vec1, vec2);
	section_4(vec1, vec2);
	run_semaphore(vec1, vec2, vSize);
	barier(vec1, vec2, vSize);

	return 0;
}

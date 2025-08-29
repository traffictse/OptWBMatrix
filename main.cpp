#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>
#include <fstream>
#include <algorithm>

#include "OptWBMatrix/random.h"
#include "OptWBMatrix/structure.h"

#include "WBMatrix/WBMatrix.h"
#include "OptWBMatrix/OptWBMatrix.h"

using namespace std;

// Global or static generator
static std::mt19937 gen(std::random_device{}());

uint32_t generateRandomUint32()
{
	std::uniform_int_distribution<uint32_t> dis(0, UINT32_MAX);
	return dis(gen);
}

uint64_t generateRandomUint64()
{
	std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);
	return dis(gen);
}

struct TestResult
{
	string name;
	bool passed;
	string errorMessage;
};

// For simple types with built-in comparison
template <typename T, typename Func1, typename Func2>
TestResult testEquivalence(const string &name, T input, Func1 func1, Func2 func2)
{
	TestResult result = {name, true, ""};

	auto output1 = func1(input);
	auto output2 = func2(input);

	if (output1 != output2)
	{
		result.passed = false;
		stringstream ss;
		ss << name << " failed.";
		result.errorMessage = ss.str();
	}

	return result;
}

// With custom equality check function
template <typename T, typename Func1, typename Func2, typename EqualityFunc>
TestResult testEquivalence(const string &name, T input, Func1 func1, Func2 func2, EqualityFunc equals)
{
	TestResult result = {name, true, ""};

	auto output1 = func1(input);
	auto output2 = func2(input);

	if (!equals(output1, output2))
	{
		result.passed = false;
		stringstream ss;
		ss << name << " failed.";
		result.errorMessage = ss.str();
	}

	return result;
}

template <typename MatrixType, typename EqualityFunc>
TestResult test_matrix_transpose(const string &name,
								 void (*opt_func)(MatrixType, MatrixType *),
								 void (*orig_func)(MatrixType, MatrixType *),
								 void (*rand_func)(MatrixType *),
								 EqualityFunc equal_func)
{
	MatrixType matrix;
	rand_func(&matrix);
	return testEquivalence(name, matrix, [opt_func](MatrixType mat)
						   {
			MatrixType res;
			opt_func(mat, &res);
			return res; }, [orig_func](MatrixType mat)
						   {
			MatrixType res;
			orig_func(mat, &res);
			return res; }, equal_func);
}

template <typename MatrixType, typename EqualityFunc>
TestResult test_matrix_multiplication(const string &name,
									  void (*opt_func)(MatrixType, MatrixType, MatrixType *),
									  void (*orig_func)(MatrixType, MatrixType, MatrixType *),
									  void (*rand_func)(MatrixType *),
									  EqualityFunc equal_func)
{
	MatrixType matrix1, matrix2;
	rand_func(&matrix1);
	rand_func(&matrix2);
	return testEquivalence(name, std::make_pair(matrix1, matrix2), [opt_func](const std::pair<MatrixType, MatrixType> mats)
						   {
			MatrixType res;
			opt_func(mats.first, mats.second, &res);
			return res; }, [orig_func](const std::pair<MatrixType, MatrixType> mats)
						   {
			MatrixType res;
			orig_func(mats.first, mats.second, &res);
			return res; }, equal_func);
}

void test_equivalence(int test_repetition)
{
	vector<TestResult> results;

	for (int i = 0; i < test_repetition; i++)
	{
		// Test parity functions
		{
			uint32_t data = generateRandomUint32();
			results.push_back(testEquivalence("opt_parityU32 vs xorU32", data, opt_parityU32, xorU32));
		}
		{
			uint64_t data = generateRandomUint64();
			results.push_back(testEquivalence("opt_parityU64 vs xorU64", data, opt_parityU64, xorU64));
		}
		{
			uint64_t data[2];
			for (int j = 0; j < 2; j++)
				data[j] = generateRandomUint64();
			results.push_back(testEquivalence("opt_parityU128 vs xorU128", data, opt_parityU128, xorU128));
		}
		{
			uint64_t data[4];
			for (int j = 0; j < 4; j++)
				data[j] = generateRandomUint64();
			results.push_back(testEquivalence("opt_parityU256 vs xorU256", data, opt_parityU256, xorU256));
		}

		// Test matrix transposition
		{
			results.push_back(test_matrix_transpose<M8>("opt_MattransM8 vs MattransM8", opt_MattransM8, MattransM8, opt_randM8, opt_isequalM8));
			results.push_back(test_matrix_transpose<M16>("opt_MattransM16 vs MattransM16", opt_MattransM16, MattransM16, opt_randM16, opt_isequalM16));
			results.push_back(test_matrix_transpose<M32>("opt_MattransM32 vs MattransM32", opt_MattransM32, MattransM32, opt_randM32, opt_isequalM32));
			results.push_back(test_matrix_transpose<M64>("opt_MattransM64 vs MattransM64", opt_MattransM64, MattransM64, opt_randM64, opt_isequalM64));
			results.push_back(test_matrix_transpose<M128>("opt_MattransM128 vs MattransM128", opt_MattransM128, MattransM128, opt_randM128, opt_isequalM128));
			results.push_back(test_matrix_transpose<M256>("opt_MattransM256 vs MattransM256", opt_MattransM256, MattransM256, opt_randM256, opt_isequalM256));
		}

		// Test matrix-vector multiplication
		{
			M128 m128;
			V128 v128;
			opt_randM128(&m128);
			opt_randV128(&v128);
			results.push_back(testEquivalence("opt_MatMulVecM128 vs MatMulVecM128", std::make_pair(m128, v128), [](const std::pair<M128, V128> &p)
											  {
												   V128 res;
												   opt_MatMulVecM128(p.first, p.second, &res);
												   return res; }, [](const std::pair<M128, V128> &p)
											  {
												   V128 res;
												   MatMulVecM128(p.first, p.second, &res);
												   return res; }, opt_isequalV128));
		}

		// Test matrix-matrix multiplication
		{
			results.push_back(test_matrix_multiplication<M64>("opt_MatMulMatM64 vs MatMulMatM64", opt_MatMulMatM64, MatMulMatM64, opt_randM64, opt_isequalM64));
			results.push_back(test_matrix_multiplication<M128>("opt_MatMulMatM128 vs MatMulMatM128", opt_MatMulMatM128, MatMulMatM128, opt_randM128, opt_isequalM128));
			results.push_back(test_matrix_multiplication<M256>("opt_MatMulMatM256 vs MatMulMatM256", opt_MatMulMatM256, MatMulMatM256, opt_randM256, opt_isequalM256));
		}
	}

	// Report results
	bool allPassed = true;
	for (const auto &result : results)
	{
		if (!result.passed)
		{
			cerr << "FAIL: " << result.errorMessage << endl;
			allPassed = false;
		}
	}

	if (allPassed)
	{
		cout << "All equivalence tests PASSED! (" << results.size() << " total tests)" << endl;
		cout << "✓ parityU32/64/128/256 equal to xorU32/64/128/256" << endl;
		cout << "✓ OptMattransM8/16/32/64/128/256 equal to MattransM8/16/32/64/128/256" << endl;
		cout << "✓ OptMatMulVecM128 equals to MatMulVecM128" << endl;
		cout << "✓ OptMatMulMatM64/128/256 equal to MatMulMatM64/128/256" << endl;
	}
	else
	{
		cout << "Some equivalence tests FAILED. See errors above." << endl;
	}
}

struct BenchmarkResult
{
	string name;
	vector<long long> originalTimes;
	vector<long long> optimizedTimes;
	vector<double> speedups;

	double avgOriginal() const
	{
		if (originalTimes.empty())
			return 0;
		long long sum = std::accumulate(originalTimes.begin(), originalTimes.end(), 0LL);
		return static_cast<double>(sum) / originalTimes.size();
	}

	double avgOptimized() const
	{
		if (optimizedTimes.empty())
			return 0;
		long long sum = std::accumulate(optimizedTimes.begin(), optimizedTimes.end(), 0LL);
		return static_cast<double>(sum) / optimizedTimes.size();
	}

	double avgSpeedup() const
	{
		if (speedups.empty())
			return 0;
		double sum = std::accumulate(speedups.begin(), speedups.end(), 0.0);
		return sum / speedups.size();
	}

	double medianSpeedup() const
	{
		if (speedups.empty())
			return 0;
		vector<double> sorted_speedups = speedups;
		sort(sorted_speedups.begin(), sorted_speedups.end());

		if (sorted_speedups.size() % 2 == 0)
			return (sorted_speedups[sorted_speedups.size() / 2 - 1] +
					sorted_speedups[sorted_speedups.size() / 2]) /
				   2.0;
		else
			return sorted_speedups[sorted_speedups.size() / 2];
	}

	double trimmedMeanSpeedup() const
	{
		if (speedups.size() <= 4)
			return avgSpeedup(); // Not enough data to trim

		vector<double> sorted_speedups = speedups;
		sort(sorted_speedups.begin(), sorted_speedups.end());

		// Remove highest and lowest values
		double sum = 0;
		for (size_t i = 1; i < sorted_speedups.size() - 1; i++)
			sum += sorted_speedups[i];

		return sum / (sorted_speedups.size() - 2);
	}

	double bestSpeedup() const
	{
		return *max_element(speedups.begin(), speedups.end());
	}

	double worseSpeedup() const
	{
		return *min_element(speedups.begin(), speedups.end());
	}

	double medianTimesFaster() const
	{
		return 100.0 / (100.0 - medianSpeedup());
	}
};

// Helper function to benchmark paired functions with fresh data in each repetition
template <typename DataGenerator, typename Func1, typename Func2, typename Cleanup>
BenchmarkResult benchmark_fresh(const string &name, int repetitions,
								DataGenerator dataGen,
								Func1 originalFunc, Func2 optimizedFunc, Cleanup cleanupFunc)
{
	BenchmarkResult result;
	result.name = name;
	result.originalTimes.reserve(repetitions);
	result.optimizedTimes.reserve(repetitions);
	result.speedups.reserve(repetitions);

	for (int rep = 0; rep < repetitions; rep++)
	{
		// Generate fresh data for this repetition
		auto data = dataGen();

		// Original implementation on fresh data
		auto start = chrono::high_resolution_clock::now();
		originalFunc(data);
		auto end = chrono::high_resolution_clock::now();
		auto origTime = chrono::duration_cast<chrono::milliseconds>(end - start).count();
		result.originalTimes.push_back(origTime);

		// Optimized implementation on same fresh data
		start = chrono::high_resolution_clock::now();
		optimizedFunc(data);
		end = chrono::high_resolution_clock::now();
		auto optTime = chrono::duration_cast<chrono::milliseconds>(end - start).count();
		result.optimizedTimes.push_back(optTime);

		// Calculate speedup
		if (origTime > 0)
		{
			result.speedups.push_back(100.0 * (origTime - optTime) / origTime);
		}
		else
		{
			result.speedups.push_back(0);
		}

		// Free memory
		cleanupFunc(data);
	}

	return result;
}

// Generic function for matrix transpose benchmarks
template <typename MatrixType>
BenchmarkResult benchmark_matrix_transpose(const string &name, int test_repetition, int datasize,
										   void (*original_func)(MatrixType, MatrixType *),
										   void (*optimized_func)(MatrixType, MatrixType *),
										   void (*random_generator)(MatrixType *))
{
	return benchmark_fresh(name, test_repetition, [datasize, random_generator]()
						   {
            MatrixType* arr = new MatrixType[datasize];
            for (int i = 0; i < datasize; i++)
                random_generator(arr + i);
            return arr; }, [datasize, original_func](const MatrixType *arr)
						   {
            MatrixType result;
            for (int i = 0; i < datasize; i++)
                original_func(arr[i], &result);
            return result; }, [datasize, optimized_func](const MatrixType *arr)
						   {
            MatrixType result;
            for (int i = 0; i < datasize; i++)
                optimized_func(arr[i], &result);
            return result; }, [](MatrixType *arr)
						   { delete[] arr; });
}

// Generic function for matrix multiplication benchmarks
template <typename MatrixType>
BenchmarkResult benchmark_matrix_multiply(const string &name, int test_repetition, int datasize,
										  void (*original_func)(MatrixType, MatrixType, MatrixType *),
										  void (*optimized_func)(MatrixType, MatrixType, MatrixType *), void (*random_generator)(MatrixType *))
{
	return benchmark_fresh(name, test_repetition, [datasize, random_generator]()
						   {
            MatrixType* left = new MatrixType[datasize];
            MatrixType* right = new MatrixType[datasize];
            for (int i = 0; i < datasize; i++) {
                random_generator(left + i);
                random_generator(right + i);
            }
            return std::make_pair(left, right); }, [datasize, original_func](const std::pair<MatrixType *, MatrixType *> &data)
						   {
            MatrixType result;
            for (int i = 0; i < datasize; i++)
                original_func(data.first[i], data.second[i], &result);
            return result; }, [datasize, optimized_func](const std::pair<MatrixType *, MatrixType *> &data)
						   {
            MatrixType result;
            for (int i = 0; i < datasize; i++)
                optimized_func(data.first[i], data.second[i], &result);
            return result; }, [](const std::pair<MatrixType *, MatrixType *> &data)
						   {
            delete[] data.first;
            delete[] data.second; });
}

void test_performance(int test_repetition)
{
	vector<BenchmarkResult> results;

	// Benchmark parity functions
	// Benchmark parityU32 vs xorU32
	{
		constexpr int datasize = 50000000;
		auto result = benchmark_fresh("xorU32 vs parityU32", test_repetition, []()
									  {
			uint32_t *data = new uint32_t[datasize];
			for (int i = 0; i < datasize; i++)
				data[i] = generateRandomUint32();
			return data; }, [](const uint32_t *data)
									  {
										  for (int i = 0; i < datasize; i++)
											  xorU32(data[i]); }, [](const uint32_t *data)
									  {
										  for (int i = 0; i < datasize; i++)
											  opt_parityU32(data[i]); }, [](uint32_t *data)
									  { delete[] data; });
		results.push_back(result);
	}
	// Benchmark parityU64/128/256 vs xorU64/128/256
	{
		constexpr int datasize = 10000000;
		auto result = benchmark_fresh("xorU64 vs parityU64", test_repetition, []()
									  {
			uint64_t *data = new uint64_t[datasize];
			for (int i = 0; i < datasize; i++)
				data[i] = generateRandomUint64();
			return data; }, [](const uint64_t *data)
									  {
										   for (int i = 0; i < datasize; i++)
											   xorU64(data[i]); }, [](const uint64_t *data)
									  {
										   for (int i = 0; i < datasize; i++)
											   opt_parityU64(data[i]); }, [](uint64_t *data)
									  { delete[] data; });
		results.push_back(result);

		auto resultU128 = benchmark_fresh("xorU128 vs parityU128", test_repetition, []()
										  {
			uint64_t *data = new uint64_t[datasize];
			for (int i = 0; i < datasize; i++)
				data[i] = generateRandomUint64();
			return data; }, [](const uint64_t *data)
										  {
										   for (int i = 0; i < datasize / 2; i++)
										   {
											   uint64_t temp[2] = {data[2 * i], data[2 * i + 1]};
											   xorU128(temp);
										   } }, [](const uint64_t *data)
										  {
										   for (int i = 0; i < datasize / 2; i++)
										   {
											   uint64_t temp[2] = {data[2 * i], data[2 * i + 1]};
											   opt_parityU128(temp);
										   } }, [](uint64_t *data)
										  { delete[] data; });
		results.push_back(resultU128);

		auto resultU256 = benchmark_fresh("xorU256 vs parityU256", test_repetition, []()
										  {
			uint64_t *data = new uint64_t[datasize];
			for (int i = 0; i < datasize; i++)
				data[i] = generateRandomUint64();
			return data; }, [](const uint64_t *data)
										  {
											  for (int i = 0; i < datasize / 4; i++)
											  {
												  uint64_t temp[4] = {data[4 * i], data[4 * i + 1], data[4 * i + 2], data[4 * i + 3]};
												  xorU256(temp);
											  } }, [](const uint64_t *data)
										  {
											  for (int i = 0; i < datasize / 4; i++)
											  {
												  uint64_t temp[4] = {data[4 * i], data[4 * i + 1], data[4 * i + 2], data[4 * i + 3]};
												  opt_parityU256(temp);
											  } }, [](uint64_t *data)
										  { delete[] data; });
		results.push_back(resultU256);
	}

	// Benchmark Matrix Transpose
	{
		results.push_back(benchmark_matrix_transpose<M8>("MattransM8 vs Opt MattransM8", test_repetition, 1000000,
														 MattransM8, opt_MattransM8, opt_randM8));
		results.push_back(benchmark_matrix_transpose<M16>("MattransM16 vs Opt MattransM16", test_repetition, 1000000,
														  MattransM16, opt_MattransM16, opt_randM16));
		results.push_back(benchmark_matrix_transpose<M32>("MattransM32 vs Opt MattransM32", test_repetition, 100000,
														  MattransM32, opt_MattransM32, opt_randM32));
		results.push_back(benchmark_matrix_transpose<M64>("MattransM64 vs Opt MattransM64", test_repetition, 100000,
														  MattransM64, opt_MattransM64, opt_randM64));
		results.push_back(benchmark_matrix_transpose<M128>("MattransM128 vs Opt MattransM128", test_repetition, 10000,
														   MattransM128, opt_MattransM128, opt_randM128));
		results.push_back(benchmark_matrix_transpose<M256>("MattransM256 vs Opt MattransM256", test_repetition, 10000,
														   MattransM256, opt_MattransM256, opt_randM256));
	}

	// Benchmark Matrix-Vector Multiplication
	{
		constexpr int datasize = 100000;
		auto result = benchmark_fresh("MatMulVecM128 vs Opt MatMulVecM128", test_repetition, [&]()
									  {
										  M128 *arrMMV128L = new M128[datasize];
										  V128 *vecMMV128R = new V128[datasize];
										  for (int i = 0; i < datasize; i++)
										  {
											  opt_randM128(arrMMV128L + i);
											  opt_randV128(vecMMV128R + i);
										  }
										  return std::make_pair(arrMMV128L, vecMMV128R); }, [](const std::pair<M128 *, V128 *> &data)
									  {
										  M128 *arrMMV128L = data.first;
										  V128 *vecMMV128R = data.second;
										  V128 ansMMV128;
										  for (int i = 0; i < datasize; i++)
											  MatMulVecM128(arrMMV128L[i], vecMMV128R[i], &ansMMV128);
										  return ansMMV128; }, [](const std::pair<M128 *, V128 *> &data)
									  {
										  M128 *arrMMV128L = data.first;
										  V128 *vecMMV128R = data.second;
										  V128 ansMMV128;
										  for (int i = 0; i < datasize; i++)
											  opt_MatMulVecM128(arrMMV128L[i], vecMMV128R[i], &ansMMV128);
										  return ansMMV128; }, [](const std::pair<M128 *, V128 *> &data)
									  {
										  delete[] data.first;
										  delete[] data.second; });
		results.push_back(result);
	}

	// Benchmark Matrix-Matrix Multiplication
	{
		results.push_back(benchmark_matrix_multiply<M64>("MatMulMatM64 vs Opt MatMulMatM64", test_repetition, 10000,
														 MatMulMatM64, opt_MatMulMatM64, opt_randM64));
		results.push_back(benchmark_matrix_multiply<M128>("MatMulMatM128 vs Opt MatMulMatM128", test_repetition, 1000,
														  MatMulMatM128, opt_MatMulMatM128, opt_randM128));
		results.push_back(benchmark_matrix_multiply<M256>("MatMulMatM256 vs Opt MatMulMatM256", test_repetition, 500,
														  MatMulMatM256, opt_MatMulMatM256, opt_randM256));
	}

	// Print results table with statistics
	cout << "\n============== PERFORMANCE RESULTS ==============\n";
	cout << left << setw(40) << "Function"
		 << setw(15) << "Original (ms)" << setw(15) << "Optimized (ms)"
		 << setw(20) << "Average Speedup (%)" << setw(20) << "Median Speedup (%)" << setw(20) << "Median Times Faster" << setw(25) << "Trimmed Mean Speedup (%)" << setw(15) << "Best Run (%)" << setw(15) << "Worst Run (%)" << endl;
	cout << string(60, '-') << endl;

	for (const auto &result : results)
	{
		cout << left << setw(40) << result.name
			 << setw(15) << fixed << setprecision(2) << result.avgOriginal()
			 << setw(15) << fixed << setprecision(2) << result.avgOptimized()
			 << setw(20) << fixed << setprecision(2) << result.avgSpeedup()
			 << setw(20) << fixed << setprecision(2) << result.medianSpeedup()
			 << setw(20) << (std::to_string(result.medianTimesFaster()).substr(0, 4) + "x")
			 << setw(25) << fixed << setprecision(2) << result.trimmedMeanSpeedup()
			 << setw(15) << fixed << setprecision(2) << result.bestSpeedup()
			 << setw(15) << fixed << setprecision(2) << result.worseSpeedup()
			 << endl;
	}

	// Write detailed results to CSV
	ofstream resultFile("benchmark_results.csv");
	if (resultFile.is_open())
	{
		resultFile << "Function,Avg Speedup (%),Median Speedup (%),Median Times Faster,Trimmed Mean Speedup (%),Best Run (%),Worst Run (%)\n";
		for (const auto &result : results)
		{
			resultFile << result.name << ","
					   << result.avgSpeedup() << ","
					   << result.medianSpeedup() << ","
					   << result.medianTimesFaster() << "x" << ","
					   << result.trimmedMeanSpeedup() << ","
					   << result.bestSpeedup() << ","
					   << result.worseSpeedup() << "\n";

			// Add individual run details
			resultFile << "Run #,Original,Optimized,Run Speedup (%)\n";
			for (size_t i = 0; i < result.originalTimes.size(); i++)
			{
				resultFile << i + 1 << ","
						   << result.originalTimes[i] << ","
						   << result.optimizedTimes[i] << ","
						   << result.speedups[i] << "\n";
			}
			resultFile << "\n"; // Add blank line between functions
		}
		resultFile.close();
		cout << "\nDetailed results saved to benchmark_results.csv\n";
	}
}

int main()
{
	test_equivalence(1000);
	test_performance(100);
	return 0;
}
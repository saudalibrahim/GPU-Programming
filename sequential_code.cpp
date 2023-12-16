#include <iostream>
#include <vector>
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

void setup(vector<vector<char>>& array, int numObstacles) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, array.size() - 1);

    for (int i = 0; i < numObstacles; ++i) {
        int row = dis(gen);
        int col = dis(gen);
        array[row][col] = 'X';
    }
}

pair<int, vector<pair<int, int>>> findObstacles(const vector<vector<char>>& array) {
    vector<pair<int, int>> obstacleLocations;

    for (int i = 0; i < array.size(); ++i) {
        for (int j = 0; j < array[i].size(); ++j) {
            if (array[i][j] == 'X') {
                obstacleLocations.push_back({i, j});
            }
        }
    }

    return {obstacleLocations.size(), obstacleLocations};
}

int main() {
    int arraySize = 50000;
    int numObstacles = 100;

    vector<vector<char>> array(arraySize, vector<char>(arraySize, '.'));

    auto start = high_resolution_clock::now();
    setup(array, numObstacles);
    auto result = findObstacles(array);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Sequential Execution Time: " << duration.count() << " ms\n";

    return 0;
}


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

pair<int, vector<pair<int, int>>> findObstaclesOpenACC(const vector<vector<char>>& array) {
    vector<pair<int, int>> obstacleLocations;
    int obstacleCount = 0;
      //#pragma acc kernels
    //array[i].size()
   // #pragma acc parallel loop collapse(2) reduction(+:obstacleCount)
    #pragma acc kernels
{
    #pragma acc loop independent
    for (int i = 0; i < array.size(); ++i) {
        for (int j = 0; j < array[i].size(); ++j) {
            if (array[i][j] == 'X') {
                obstacleCount = obstacleCount +1;
                //#pragma acc atomic
                obstacleLocations.push_back({i, j});
            }
        }
    }
}
    cout << "# of obstacles: " << obstacleLocations.size() << "\n";
    return {obstacleLocations.size(), obstacleLocations};
}

int main() {
    int arraySize = 50000;
    int numObstacles = 100;

    vector<vector<char>> array(arraySize, vector<char>(arraySize, '.'));

    auto startOpenACC = high_resolution_clock::now();
    
    #pragma acc data copyin(array)
    {
        setup(array, numObstacles);
        auto resultOpenACC = findObstaclesOpenACC(array);
    }

    auto stopOpenACC = high_resolution_clock::now();
    auto durationOpenACC = duration_cast<milliseconds>(stopOpenACC - startOpenACC);

    cout << "C++ OpenACC Execution Time: " << durationOpenACC.count() << " ms\n";
   

    return 0;
}

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

const int population_size = 100;
const int generation_number = 100;
const double mutationRate = 0.1;
const double crossoverRate = 0.7;

//initialize population
vector<int> initialize_population(int capacity, int num_of_items, vector<pair<int, int>> items)
{
    vector<int> chromosome(num_of_items);
    //handle infeasible solution
    bool feasible = false;
    while (!feasible)
    {
        for (int i = 0; i < num_of_items; i++)
        {
            chromosome[i] = rand() % 2; //generates random binary values 0 or 1
        }

        int totalWeight = 0;
        for (int j = 0; j < num_of_items; j++)
        {
            if (chromosome[j] == 1)
            {
                totalWeight += items[j].first;
            }
        }

        if (totalWeight <= capacity)
        {
            feasible = true;
        }
    }

    return chromosome;
}

//evaluate fitness funtion
int calculate_fitness(vector<int> population, vector<pair<int, int>> items, int capacity)
{
    int total_weight = 0, total_value = 0;
    for (int i = 0; i < items.size(); i++)
    {
        if (population[i] == 1)
        {
            total_weight += items[i].first;
            total_value += items[i].second;
        }
    }

    // Check if the solution is infeasible, and if so, return a fitness of 0
    if (total_weight > capacity)
    {
        return 0;
    }

    return total_value;
}

//rank selection function
vector<vector<int>> rankSelection(vector<vector<int>> population, vector<pair<int, int>> items, int capacity)
{
    vector<vector<int>> new_population(population_size);
    vector<pair<int, vector<int>> > data(population_size); //data.first=fitness , data.second=chromosome

    for (int i = 0; i < population_size; i++)
    {
        data[i].first = calculate_fitness(population[i], items, capacity);
        data[i].second = population[i];
    }

    sort(data.begin(), data.end());

    vector<double> selection_probabilities(population_size);

    for (int i = 0; i < population_size; i++)
    {
        //rank
        selection_probabilities[i] = (i + 1) / static_cast<double>(population_size);
    }

    for (int i = 0; i < population_size; i++)
    {
        double random_number = (double)(rand()) / RAND_MAX;//between 0 and 1

        for (int j = 0; j < population_size; j++)
        {
            if (random_number <= selection_probabilities[j])
            {
                new_population[i] = data[j].second;
                break;
            }
        }
    }

    return new_population;
}

//one point crossover function
vector<int> crossover(vector<int> parent1, vector<int> parent2)
{
    if ((double)rand() / RAND_MAX < crossoverRate)
    {
        int crossover_point = rand() % parent1.size(); // Randomly select a crossover point
        vector<int> offspring(parent1.size());

        for (int i = 0; i < crossover_point; i++)
        {
            offspring[i] = parent1[i];
        }

        for (int i = crossover_point; i < parent1.size(); i++)
        {
            offspring[i] = parent2[i];
        }

        return offspring;
    }
    else
    {
        return parent1; //No crossover return parent1 as it is
    }
}

//mutation function
void mutation(vector<int>& individual)
{
    for (int j = 0; j < individual.size(); j++)
    {
        if ((double)rand() / RAND_MAX < mutationRate)
        {
            individual[j] = 1 - individual[j]; //Flip bit
        }
    }
}

int main()
{
    srand(time(0)); //random number generator
    ifstream inputFile("knapsack_input.txt");
    if (!inputFile)
    {
        cout << "Failed to open the input file." << endl;
        return 1;
    }

    int num_of_testcases;
    inputFile >> num_of_testcases;
    for (int testcase = 0; testcase < num_of_testcases; testcase++)
    {
        int capacity, num_of_items;
        inputFile >> capacity >> num_of_items;

        vector<pair<int, int>> items(num_of_items);

        for (int i = 0; i < num_of_items; i++)
        {
            inputFile >> items[i].first >> items[i].second;
        }

        vector<vector<int>> population(population_size, vector<int>(num_of_items));

        //call Initialization
        for (int i = 0; i < population_size; i++)
        {
            population[i] = initialize_population(capacity, num_of_items, items);
        }

        vector<int> bestSolution;
        int bestFitness = 0;

        for (int generation = 0; generation < generation_number; generation++)
        {
            // Fitness & chromosome
            vector<vector<int>> new_population(population_size);

            // call rank selection here
            population = rankSelection(population, items, capacity);

            vector<vector<int>> next_generation(population_size);

            for (int i = 0; i < population_size; i += 2)
            {
                vector<int> parent1 = population[i];
                vector<int> parent2 = population[i + 1];
                vector<int> child1 = crossover(parent1, parent2);
                vector<int> child2 = crossover(parent2, parent1);
                mutation(child1);
                mutation(child2);
                next_generation[i] = child1;
                next_generation[i + 1] = child2;
            }
            //make Replacement
            population = next_generation;

            // Find best solution in the current population
            for (int i = 0; i < population_size; i++)
            {
                //call fitness function
                int currentFitness = calculate_fitness(population[i], items, capacity);
                if (currentFitness > bestFitness)
                {
                    bestSolution = population[i];
                    bestFitness = currentFitness;
                }
            }
        }

        cout << "Test Case " << testcase + 1 << ":" << endl;
        int totalWeight = 0;
        int numSelectedItems = 0;
        for (int i = 0; i < bestSolution.size(); i++)
        {
            if (bestSolution[i] == 1)
            {
                numSelectedItems++;
                cout << "Item " << i + 1 << ": Weight = " << items[i].first << ", Value = " << items[i].second << endl;
                totalWeight += items[i].first;
            }
        }

        cout << "Number of Selected Items: " << numSelectedItems << endl;
        cout << "Total Weight: " << totalWeight << endl;
        cout << "Total Value: " << bestFitness << endl;
        cout << "-----------------" << endl;
    }

    inputFile.close();
    return 0;
}


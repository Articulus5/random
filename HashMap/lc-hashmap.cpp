#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

// LeetCode problem #1 - Two Sum using hashmap
vector<int> twoSum(vector<int> &nums, int target)
{
    vector<int> result;
    unordered_map<int, int> umap;

    for (unsigned int i = 0; i < nums.size(); i++)
    {
        //unordered_map<int, int>::iterator it
        auto it
            = umap.find(nums[i]);

        if (it != umap.end())
        {
            result.push_back(it->second);
            result.push_back(i);
            break;
        }

        umap[target-nums[i]] = i;
    }

    return result;
}

int main()
{
    vector<int> test1 = { 2, 7, 11, 15 };
    int target1 = 9;
    vector<int> test2 = { 3, 2, 4 };
    int target2 = 6;
    vector<int> test3 = { 3, 3 };
    int target3 = 6;
    vector<int> result;

    result = twoSum(test1, target1);
    for (int i : test1)
        cout << i << ' ';
    cout << endl << "Found " << test1[result[0]] << " + " << test1[result[1]] << " = " << target1 << endl;

    result = twoSum(test2, target2);
    for (int i : test2)
        cout << i << ' ';
    cout << endl << "Found " << test2[result[0]] << " + " << test2[result[1]] << " = " << target2 << endl;

    result = twoSum(test3, target3);
    for (int i : test3)
        cout << i << ' ';
    cout << endl << "Found " << test3[result[0]] << " + " << test3[result[1]] << " = " << target3 << endl;

    return 0;
}








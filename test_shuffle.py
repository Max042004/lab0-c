import subprocess
import re
import random
from itertools import permutations
import random
import subprocess
import re
import random
from scipy.stats import chi2
import matplotlib.pyplot as plt
import numpy as np

def run_one():
    # 測試 shuffle 次數
    test_count = 100000
    input = "new\nit 1\nit 2\nit 3\nit 4\n"
    input += "shuffle " + str(test_count) + "\n"
    #input += "shuffle\n" * test_count
    input += "free\nquit\n"

    # 取得 stdout 的 shuffle 結果
    command='./qtest -v 3'
    clist = command.split()
    completedProcess = subprocess.run(clist, capture_output=True, text=True, input=input)
    s = completedProcess.stdout
    startIdx = s.find("l = [1 2 3 4]") 
    endIdx = s.find("l = NULL")
    s = s[startIdx + 14 : endIdx]
    Regex = re.compile(r'\d \d \d \d')
    result = Regex.findall(s)

    def permute(nums):
        nums=list(permutations(nums,len(nums)))
        return nums

    def chiSquared(observation, expectation):
        return ((observation - expectation) ** 2) / expectation 

    # shuffle 的所有結果   
    nums = []
    for i in result:
        nums.append(i.split())

    # 找出全部的排序可能
    counterSet = {}
    shuffle_array = ['1', '2', '3', '4']
    s = permute(shuffle_array)

    # 初始化 counterSet
    for i in range(len(s)):
        w = ''.join(s[i])
        counterSet[w] = 0

    # 計算每一種 shuffle 結果的數量
    for num in nums:
        permutation = ''.join(num)
        counterSet[permutation] += 1
            
    # 計算 chiSquare sum
    expectation = test_count // len(s)
    c = counterSet.values()
    chiSquaredSum = 0
    for i in c:
        chiSquaredSum += chiSquared(i, expectation)
    p_value = 1 - chi2.cdf(chiSquaredSum, 23)
    print("Expectation: ", expectation)
    print("Observation: ", counterSet)
    print("chi square sum: ", chiSquaredSum)
    print("p: ", p_value)

    return p_value

def main():
    times = 100
    p_values = []
    for i in range(times):
        p = run_one()
        p_values.append(p)
        print(i)
    
    # 計算 p_value 的平均數
    average_p = sum(p_values) / len(p_values)
    print("Average p-value:", average_p)
    
    intervals = {
        ">0.9": lambda p: p > 0.9,
        "0.9~0.1": lambda p: 0.1 < p <= 0.9,
        "0.1~0.05": lambda p: 0.05 < p <= 0.1,
        "0.05~0.025": lambda p: 0.025 < p <= 0.05,
        "0.025~0.001": lambda p: 0.001 < p <= 0.025
    }
    labels = list(intervals.keys())
    counts = [sum(1 for p in p_values if condition(p)) for condition in intervals.values()]
    
    # 繪製直條圖
    plt.figure(figsize=(10,6))
    plt.bar(labels, counts, width=0.5)
    plt.xlabel("p-value interval")
    plt.ylabel("times")
    plt.title("p-value plot")
    plt.show()

if __name__ == "__main__":
    main()
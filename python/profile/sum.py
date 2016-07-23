import time

def func1():
    sum = 0
    for i in range(1000000):
        sum += i

def func2():
    time.sleep(1)

func1()
func2()

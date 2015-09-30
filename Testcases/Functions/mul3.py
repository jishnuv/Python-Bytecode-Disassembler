def mult3(n):
    if n == 1:
        return 3
    else:
        return mult3(n-1) + 3

print mult3(6)

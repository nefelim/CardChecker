# About CardChecker
Some dishonest sellers deceive their customers. They sell fake USB or SD cards with small (fake) capacity,
at the price of good flash cards. When writing to these drives, the customer usually loses his money and
data that was written there. This program will help to find the true capacity of the purchased drive.

# How to build

## Requirements
You must install 'gcc' and 'cmake' packages (optionally Qt Creator for editing).

## Build
To use the program, you must compile and run it. To do this, you can run the *./build.sh* script (for Linux).
You can also use [Qt Creator](https://www.qt.io/product/development-tools) to open, build, and edit a project.
To do this, you must open the *./src/CMakeList.txt* file in it.

# How to use
After a successful build, you can find a new file *./bin/Release/CardCheckerCli* and run it with parameter. 
+ You must find path of your USB **DEVICE**. Usualy it is path similar to /dev/sd[a|b|c|...]
+ Run program *with root privilages*.
```
sudo CardCheckerCli /dev/sdb
```

(!) **All datas on this drive will be corrupted!!! Don't use this program, if you are unsure of your knowledge**
+ After long time, if all ok, you can see text:
```
Checking of blocks completed
Real size is 104857600b.
This is 100% of the declared size
```

# Development tips
For testing and development, you can use file (instead of real device). 
For example you can make new file */tmp/test_file* with 100M size and try to use it.
```
dd if=/dev/zero of=/tmp/test_file bs=100M count=1
```
and run 'check' it
```
CardCheckerCli /tmp/test_file
```

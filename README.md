# scurl
scurl is a simple cli tool that works like curl. 
It is no nonsense and does not have any fancy features.

"like curl but sh***y" - @torbenconto

"don't ever use this" - @literally everyone

## Motivation
I wanted to learn a little bit of c++ and I thought this would be a fun project to do so.

## Requirements
Unix:
- g++
- make
- cmake
Windows
- g++ (mingw or visual studio)

## Installation

```bash
Unix:
git clone github.com/torbenconto/scurl.git
cd scurl
./build.sh
```
Windows:
```bash
./build.bat
```


## Usage
```bash
./scurl <url> [port] 
```
default port is 80
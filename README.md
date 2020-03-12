# Puck

Puck is a simple keylogger for Windows written in C++.

## Icon

You need to create the icon first. Then you need to create a RC file with the
below content. Here we will name it as `puck.rc`.
```
id ICON "chrome.ico"
```

The id mentioned in the above command can be pretty much anything. It does not
matter unless you want to refer to it in your code. Run windres as follows:
```
windres puck.rc -O coff -o puck.res
```

While building the executable, along with other object files and resource files,
include `puck.res` generated in the previous step.

```
g++ -o main main.cpp puck.res
```
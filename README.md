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

## License

Copyright 2020 Samuel Rowe

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

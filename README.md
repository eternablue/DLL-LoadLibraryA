# LoadLibraryA - DLL injection

## The API

```LoadLibraryA``` is a function from the Window API. It enables a process to load a dynamic linked library (aka DLL). It only takes one argument which is the path to the dll that you want to load. If the dll is located in the ```Known Dll's``` registry key you can simply give the name without giving the whole path, an example would be user32.dll.

## Steps for injection

We will first open a handle to the target process using ```OpenProcess``` with full access. We will need that later to call other api's which require that as a parameter, for writing to memory and creating threads.

```cpp
OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
``` 
\
Then we allocate memory in the remote process using ```VirtualAllocEx```. The 'Ex' version of this functions enables us to supply a remote process to allocate the memory too instead of our own. We allocate memory that will contain the path to our dll later on. This is why we allocate as much memory as needed to contain the longest path in Windows which is 260 chars. This is defined in the MAX_PATHmacro.
  
```cpp
VirtualAllocEx(hProcess, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
```  
\
We then use ```WriteProcessMemory``` to write the path of our dll in the allocated memory of the remote process. So that it can be used by functions which are called in the context of the remote process.

```cpp
WriteProcessMemory(hProcess, allocated_memory, &(*argv[2]), MAX_PATH, 0);
```
\
Now for the most important part, we use ```CreateRemoteThread``` to start a new thread of execution in the remote process. We set the 'entrypoint' of our thread so that it starts at the address of the ```LoadLibraryA``` function. We also supply an argument to our thread which is the address of the dll path that was allocated earlier. When the thread starts it will execute the api call and pass as first and single parameter the dll that needs to be loaded.

```cpp
CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, allocated_memory, 0, 0);
```

## Showcase

![image](https://cdn.discordapp.com/attachments/780153367305256981/1018562091667443762/demo.gif)

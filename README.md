# MLV_Fixes
This repository is only about fixing issues with MLV files created by Magic Lantern software. Those fixes are at file level and should be used along MLVApp.

# MLV 00 Removal

A simple toolset to inspect and repair MLV files with trailing `00` bytes at the end of the file, which can cause invalid blockSize errors in MLV processing applications.
Link to MLVApp: https://github.com/ilia3101/MLV-App
---

## Features

- **Error Finder** (`MLV_Error_Finder.exe`):  
  - Scans MLV files for invalid `blockSize == 0` errors.  
  - **Does not modify files**. Helps locate corrupted areas.  

- **MLV Truck** (`MLV_Truck.exe`):  
  - Safely truncates trailing zero bytes at the end of MLV files.  
  - Fully portable and safe: writes changes to a temporary file before replacing the original.  

- **Menu.bat**:  
  - Provides a simple menu to choose between the Error Finder and MLV Truck.  
  - Returns to the menu after each operation.  

---

## Usage

1. In order to run, start Menu.bat. Confirmed working on Win 10 x64.

MLV_Fix/
└─ MLV 00 Removal/
├─ Menu.bat
├─ MLV_RepairC++/bin/Release/MLV_Error_Finder.exe
└─ MLV_Truck/bin/Release/MLV_Truck.exe


2. Double-click `Menu.bat` to open the menu.  
3. Choose an option:  
   - `1` → Run **Error Finder** to check MLV file for blockSize errors.  
   - `2` → Run **MLV Truck** to fix trailing zeros.  
   - `3` → Exit.  

4. Follow the on-screen instructions to provide the path of your MLV file.  

---

## Notes

- The Error Finder only detects errors; it will not modify your files.  
- MLV Truck safely removes trailing zeros at EOF and will never delete more than necessary.  
- Compatible with large MLV files (multi-GB).  

---

## License

[MIT License](LICENSE) 
GNU GENERAL PUBLIC LICENSE
Version 3, 29 June 2007
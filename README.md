# MLV 00 Removal

A simple toolset to inspect and repair MLV files with **trailing `00` bytes** at the end of the file, which can cause invalid `blockSize == 0` errors in MLV processing applications.

This issue is described in the original MLV-App repository: [MLV-App Issue #305](https://github.com/ilia3101/MLV-App/issues/305).

---

## How the Error Works

Some MLV files may end with extra zero bytes. When MLV processing tools (like MLV-App) read such files, they may encounter a block with `blockSize == 0`, which causes a **corrupted file error**:

![BlockSize 0 Example](https://raw.githubusercontent.com/GhervasaCristian/MLV_Fixes/main/MLV%2000%20Removal/src/blocksize%20%270%27.png)

This happens because the reader interprets the trailing zeros as a malformed block, crashing or refusing to process the file.

---

## Features

### Error Finder (`MLV_Error_Finder.exe`)
- Scans MLV files for invalid `blockSize == 0`.  
- **Does not modify files**.  
- Helps locate corrupted areas visually and in hex.  

Example menu screenshot:  
![Menu Error Finder](MLV 00 Removal/src/Menu_00_0.png)

### MLV Truck (`MLV_Truck.exe`)
- Safely truncates trailing zero bytes at the end of MLV files.  
- Fully portable and safe: writes changes to a temporary file before replacing the original.  

Example menu screenshot:  
![Menu MLV Truck](src/Menu_00_1.png)

---

## Usage

1. Start with Menu.bat

MLV_Fix/
└─ MLV 00 Removal/
├─ Menu.bat
├─ MLV_RepairC++/bin/Release/MLV_Error_Finder.exe
└─ MLV_Truck/bin/Release/MLV_Truck.exe
└─ src/
├─ blocksize '0'.png
├─ Menu_00_0.png
└─ Menu_00_1.png


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

[MIT License](LICENSE) (or your preferred license)

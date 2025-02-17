# NG2ArchiveTool
A tool to extract and repack .ng2 archives used in Ninja Gaiden 2 for Xbox 360.

# Disclaimer
The author of this repository has no affiliation with Team Ninja or Koei Tecmo and this software is intended purely for educational or entertainment purposes. The author does not claim ownership of the file formats that may be modifiable through use of the software, and no copyright infringement is intended. No game files will be distributed. All rights go to their respective owners. If you are the rights holder and would like us to remove this content, please contact us and we will comply with your request immediately.

# Usage
To use this tool, first you need to meet a few conditions.
1. Extract your Ninja Gaiden 2 ISO file. You should see several `.ng2` files, `default.xex`, `pack_img.bin` and more.
2. Download [Noesis](https://richwhitehouse.com/index.php?content=inc_projects.php&showproject=91) and open it. Navigate to `default.xex` and double-click (or right-click -> Export) in order to decrypt this game executable. You should get `xex_decrypted.bin`, which you put in the same directory as the other game files.
3. Download the latest [release](https://github.com/Mystixor/NG2ArchiveTool/releases) of the NG2ArchiveTool and put `NG2ArchiveTool.exe` in the same directory as all the game files you just prepared.
4. Drag-n-drop a `.ng2` file onto the `NG2ArchiveTool.exe` to extract its contents. You can now edit these files, but do not rename the files or the folders.
5. When you are done modding, drag-n-drop the whole folder of the archive (for `s_chap_01.ng2` that will be `s_chap_01`, make sure the name didn't change!) onto the executable again. You will receive a new file with the ending `-out.ng2` and another file `pack_img-out.bin`, remove the "-out" in both of these file names. If the files are still in the game directory your mod is now active!
6. To disable a mod again, simply replace the modded `.ng2` archive as well as the `pack_img.bin` with the original files you extracted from your ISO file.

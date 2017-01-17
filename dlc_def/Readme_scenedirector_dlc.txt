Additional game content to be used for Scene director mod. 

Install as a GTA V dlc pack by performing the following:

First part - Copy the necessary files
1. Open OpenIV
2. Expand mods\update\x64\dlcpacks
3. Create new folder scenedirector
4. Copy dlc.rpf from zip file to this folder

Second part - Add to dlclist.xml
Then add it to the dlclist.xml so that it's loaded by GTA. 
1. In OpenIV, open mods\update\update.rpf
2. Navigate to folder common\data
3. Extract file dlclist.xml to file system
4. Edit dlclist.xml on file system
5. Add line <Item>dlcpacks:\scenedirector\</Item> after other dlcpacks (and before </Paths>)
6. Put OpenIV in Edit mode
6. Save file and drag into OpenIV to replace dlclist.xml with your changes


If this works, you should be able to spawn the spotlights using the prop names prop_sd_spot_01 to prop_sd_spot_15.

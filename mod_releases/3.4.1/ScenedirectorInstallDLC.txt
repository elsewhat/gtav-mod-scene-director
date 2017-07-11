Additional game content to be used for Scene director mod. 

Install as a GTA V dlc pack by performing the following:

First part - Copy the necessary files
1. Open OpenIV
2. Expand or create folder mods\update\x64\dlcpacks
3. Create new folder scenedirector
4. Copy dlc.rpf from distribution zip file to scenedirector folder

Second part - Add to dlclist.xml
In order to get loaded by GTA, it must be part of the dlclist.xml file. 
1. In OpenIV, open mods\update\update.rpf
(if this does not exist in the mods folder, you need to move it to here via standard OpenIV feature)
2. Navigate to folder common\data
3. Edit dlclist.xml in OpenIV
4. Add line <Item>dlcpacks:\scenedirector\</Item> after other dlcpacks (and before </Paths>)
5. Save

The above steps are necessary to spawn the special stage lights using the prop names prop_sd_spot_01 to prop_sd_spot_15.
If this is not done correctly, adding a new light will fail with the message "Failed to load objects".

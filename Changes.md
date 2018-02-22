MQ2Nav Changelog
================

1.2.2
-----

Updated latest EQ LIVE (2/21/2018)

**Bug Fixes**
* Don't try to initialize the plugin until we enter the game
* Use actual position and distance when calculating the camera angle when following a path.
* Fixed an issue with the automatic door opener that was causing it to ignore most RoS doors. The restrictions have been relaxed while still trying to avoid switches such as teleporters.

1.2.1
-----

**Improvements - MQ2Nav**
* Updated source to latest version of MQ2 core
* Now built using the latest Windows SDK and Visual Studio 2017.
* Destination zone name on switches (for example, pok stones) is now available to debug tools and will eventually be introduced into other features
* D3DX9_43.dll is now included in the build to avoid requiring DX9 runtime installer.

**Bug Fixes**
* Zones.ini: Add new zones for Ring of Scale expansion
* Fixed a crash when trying to load a mesh that had been created with an incorrect zone name.
* Fixed a typo in log message complaining about not finding a starting point.

1.2.0
-----

**Improvements - MQ2Nav**
* New feature: active navigation path will now be displayed on the map if MQ2Map is loaded.
  * Color, layer and toggle added to MQ2Nav settings.
* New feature: automatic door opening while following path
  * Door opening is only enabled while a path is actively being followed. Check out the "Door Handler Debug" UI to tweak behavior.
  * Current behavior is: click door every 2 seconds for any door with distance < 20. Check the "Door Handler Debug" to find tune the distance.

**Improvements - MeshGenerator**
* New feature: Maximum Zone Extents
  * Some zones have geometry that exists far outside the normal zone area, leading to issues generating the mesh for that zone. Added a new feature that will filter out geometry that exists outside these maximum boundaries on certain zones. See NavMeshData for the list of existing zones that support this feature. 
  * This feature has a checkbox to disable it in settings if you want to be a rebel and see the filtered out geometry.

**Bug Fixes**
* Zones.ini: fix shortname for Chelsith Reborn.
* Waypoints now use FloorHeight when calculating player position (if enabled in settings).


1.1.1
-----

**Bug Fixes**
* Fix crash when loading MQ2Nav while also running InnerSpace
* Fix problem selecting zones that have duplicate names in the zone picker

**Improvements**
* Use FloorHeight instead of Z for calculating spawn positions, should avoid some cases of "unable to find destination" problems.
  * This behavior can be disabled in the settings window of MQ2Nav if it causes problems


1.1.0
-----
*NOTE* This version is not compatible with navmeshes made with previous versions. You will need to rebuild your meshes!

**New Areas Tool**
* MeshGenerator can now define custom area types with cost modifiers. Access it from the Edit menu.
  * Define custom areas types, give them names, colors, and costs.
* Convex Volume tool has been given an overhaul, lets you save the volumes and edit existing volumes.
  * Convex Volumes let you mark parts of the navmesh as different area types.
  * Areas define cost modifiers or can completely exclude areas of the mesh from navigation
  * Navigation will always use the lowest cost path. Mark areas with high cost to avoid them and low cost to prefer them. Default area cost is 1.0

**Navmesh Format Changes**
* Due to many changes to the internal structure of the navmesh, old navmeshes will not load correctly. As a result, the navmesh compat version has been bumped up, so loading older meshes will give you an error.
* Navmesh files now have their own extension: .navmesh
* Navmesh files are now compressed for extra space savings
* All modifications to the navmesh are now stored in the .navmesh file, so reopening a mesh will have all the changes that were made when the navmesh was created.
* Tile size limit has been removed.

**UI Changes**
* New theme applied to the UI in both the plugin and MeshGenerator
* Completely redesigned MeshGenerator UI.
* Adjusted the defaults of a couple navmesh parameters for better path finding behavior:
  * Reduced MaxClimb to 4.0 to avoid getting stuck on the edge of tall objects
  * Increased MaxSlope to 75 to allow traversal over steeper terrain.
* Navmesh ingame path rendering has received a visual upgrade.
* Revamp the ingame Waypoints UI, fixing several bugs
  * Added option to delete and rename waypoints
  * Fixed the order of the x,y,z coordinates in waypoints UI to match /loc
* Improvements to how the in-game active navigation path line is rendered.

**Misc Changes**
* Added feature to export/import settings as json files. This can be accessed through the "mesh" menu. By default these settings files well be put into <MQ2Dir>/MQ2Nav/Settings
* MeshGenerator is now a 64-bit application
* Obligatory mention: fixed lots of bugs.
  * EOK Zones should now load all objects. Please note that performance is going to be pretty terrible, please have patience - i'm still working on improving handling large zones.
  * Fixed issues that would cause your character to get stuck running back and forth when running near edges of the navmesh

**Command Changes**
* Fixed various /nav commands and made many improvements:
  * Removed /nav x y z, replaced with:
    * eq /loc coordinates: /nav loc y x z, /nav locyxz y x z
    * eq xyz coordinates: /nav locxyz x y z
  * Fixed issues with clicking items and doors when using /nav item or /nav door
* Remove spam click behavior of /nav item/door as it didn't work right anyways
  * Added /nav spawn <text> - takes same parameters of Spawn TLO to do a spawn search
* Added new forms of /nav commands. [See the wiki](https://github.com/brainiac/MQ2Nav/wiki/Command-Reference) for more info.



1.0.0
-----

Initial Version of change log, started recording changes after this point!
* Originally numbered 2.0, but retroactively changed to 1.0 to start a new versioning sequence!

Local Map Overhaul - Furniture Support

Independent furniture extension for Local Map Overhaul 3.0 Full.

Requires Local Map Overhaul, yUI/ySI, and KEYWORDS.

Registers its own map-menu-open handler and draws immediately, checking
LMO's distance trait and the existing local-map image rather than waiting
for LMO's dynamic icon container. Scans furniture (form type 39) and builds a
separate LMOFurniture_Icons container using LMO's existing UI templates.
Only furniture passing the LocalMapIcon keyword/aux-var check is drawn.
Furniture must have a ySI icon assignment. The Auto-Save Points Beds and
Chairs integrations provide those assignments and keywords.

Enable alongside Local Map Overhaul in MO2. No LMO files are overridden.
Disable this extension to remove furniture icons.
Restart the game after enabling or disabling.

Depends on LMO's Map_Icons and Icon_Template templates and UI traits.
Future LMO changes to that UI may require compatibility adjustments.
In-game verification is still required, including the first map opening
after loading a save, reopening the map,
switching map tabs, hovering icons, and loading another save.

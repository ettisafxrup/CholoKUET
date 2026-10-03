# How the campus map was built

The campus data in `data/locations.txt` and `data/paths.txt` comes from two sources that we checked against each other.

## 1. OpenStreetMap

OpenStreetMap already has KUET's campus outline and 40-odd named buildings (departments, halls, library, Durbar Bangla, the auditorium, and so on). These were surveyed on the ground, so we use their coordinates directly for 38 of our 44 places.

Map data © OpenStreetMap contributors, available under the Open Database Licence (ODbL).

## 2. The KUET master plan

The _Proposed Revised Master Plan of KUET Khulna Campus_ shows the road layout and a few buildings that OSM doesn't have yet. We used it for two things:

- **Paths.** The 56 walkable paths follow the roads drawn on the plan.
- **Six missing buildings.** Fazlul Haque Hall, the Central Mosque, the Canteen, the Garage, the Dormitory and the Kodom Tola are marked `(plan)` in the data file.

To place those six, we picked 18 buildings that appear on both the plan and OSM. For each one we noted its pixel position on the plan and its real latitude and longitude. A least-squares affine fit then turns any plan pixel into a coordinate:

```
latitude  = a0 + a1·x + a2·y
longitude = b0 + b1·x + b2·y
```

Across the 18 shared buildings the fit is off by **27 m on average**. That's about one building's width, and it also shows that the plan's layout matches the real campus well. The fit comes out at roughly 1 m per plan pixel, with the plan turned about 2° from true north.

The plan also uses an old hall name: _B.B. Hall_ was renamed **Shaheed Smrity Hall** in November 2024, and the data uses the new name.

## What we left out

Proposed buildings (the pink "PRO." blocks), vacant land, and blocks whose labels we couldn't identify for sure (_H.E._, _T.C._) are not included. They can be added from the admin menu later.

OSM's outline leaves out the west area where the plan shows the IT park, even though the IT Building is there. The map drawing follows the plan for that part.

## Redrawing the map

`assets/campus-map.svg` is generated from the data files, so it always matches what the app knows:

```
node tools/render-map.js
```

The numbers on the map are the location IDs used in the app.

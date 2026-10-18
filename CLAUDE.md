# FSRA Hawk EFM

External Flight Model (EFM) DLL for the BAE Hawk T.1/A in DCS World. C++17, Visual Studio 2022
(toolset v143), output `Hawk.dll`. Source comments are partly Italian.

## Build

- Solution `Hawk.sln`, project `ExternalFM/ExternalFM.vcxproj`.
- The working configuration is **Release|x64**. It outputs `ExternalFM/bin_debug/Hawk.dll`
  (named `bin_debug`, but this is the release build). The Debug configs use precompiled headers and
  an `OutDir` outside the repo, and are not maintained.
- Command line:
  `MSBuild ExternalFM\ExternalFM.vcxproj /p:Configuration=Release /p:Platform=x64`
- The linker runs with `/FORCE:MULTIPLE`. That hides real duplicate definitions:
  - `linearInterpolation` and the data vectors are defined in headers that several `.cpp` files include,
  - `Hawk::Adour RRAdour` is defined in both `Engine/APU.cpp` and `ExternalFM.cpp`.

  Expect `LNK4006` warnings. Fixing this properly (`inline` / `extern`) is a worthwhile cleanup.
- Existing warnings are mostly harmless. One real one is in `ed_fm_release`: `ed_fm_simulate;` has
  no brackets, so it does nothing.

## Install / test in DCS

- The mod lives in `Saved Games\DCS\Mods\aircraft\FSRA Hawk\`. `entry.lua` loads `bin\Hawk.dll`
  (`binaries = { 'Hawk' }`). Always back up the installed DLL before replacing it.
- Mass properties are in the mod's `FM/config.lua`, which is not in this repo:
  - CG `{-0.0745, 0.009825, 0}`,
  - inertia `{8037.24, 13714.73, 6481.98, -768.88}` (Ixx, Iyy, Izz, Ixy).
- ED's EFM API headers are in `include/FM/` (the same as the DCS install's `API/include`).

## Code map

| File | Purpose |
|---|---|
| `ExternalFM/ExternalFM.cpp` | All `ed_fm_*` exports. Collects state, runs subsystems, sums forces and moments |
| `Aerodynamics/AerodyneFM.*`, `AeroData.h` | Aero model: CL/CD vs AoA table, linear derivatives, AoA/Mach/q-bar scalar tables, ground effect |
| `Airframe/Airframe.*` | Control-surface, flap, airbrake and wheel-brake actuators; lights |
| `Actuators/Actuators.*` | Gear and airbrake actuators |
| `Engine/Adour.*` | **Active** engine model: spool-up/down and thrust |
| `Engine/Engine.*`, `EngineData.h` | Table-based engine (thrust vs Mach, altitude scalar). Compiled in but **not used** |
| `Engine/APU.*`, `Engine/FuelSystem.*` | Gas-turbine starter, fuel |
| `Suspension/` | Gear and wheels |
| `Hydraulics/` | Present but commented out of the simulation |
| `Commands.h` | DCS command IDs |

**Axis conventions.** In `ed_fm_set_current_state_body_axis` the DCS body axes (x forward, y up,
z right) are remapped to FSRA's convention: `u = vx`, `v = vz`, `w = -vy`, `p = omega_x`,
`q = omega_z`, `r = -omega_y`. Aero forces and moments are mapped back to DCS axes in
`ed_fm_simulate`, with yaw applied as `-N` and pitch on z.

The stick axes are stored negated: `stick_pitch = -value`.

## Known bugs (not yet fixed)

1. **Mach is wrong.** `Hawk::Atmosphere::update` (`AerodyneFM.cpp`) divides airspeed in **m/s** by
   the speed of sound in **ft/s** (`sqrt(1.4 * 1716.3 * T_R)`), so Mach is about 3.3× too low.
   That affects the Mach scalar tables and anything else that reads `mach`. Use `V / vSuono`
   (`vSuono` comes from `ed_fm_set_atmosphere`).
2. **Pressure is overwritten.** In the same call, q-bar is written into `pressione_Pa`, overwriting
   the ambient pressure.
3. **Memory leak.** `ed_fm_simulate` calls `malloc` for `temp` every frame and never frees it.
4. **Thrust has no altitude or Mach effect.** `Adour::setThrust` uses `throttle × NG_RPM × 240`,
   capped at 24 kN. The unused `Engine` class has Mach and altitude tables that could be adapted.
5. **Fuel burn is disabled.** `simulate_fuel_consumption` is marked TODO and switched off;
   `FuelSystem::update` handles fuel instead.
6. The installed `Hawk.dll` in Saved Games is not identical to `bin/Hawk.dll` in this repo.

## Optional local variant (`FSRA_VRA_DATA`) — leave it off

`ExternalFM.cpp` contains `#ifdef FSRA_VRA_DATA` blocks, and the `.vcxproj` adds that define when
built with `/p:VraData=true`. This is a **private, local-only** variant: it swaps in aero data
derived from another team's private Hawk mod.

- Its source (`ExternalFM/VRA/`, `build_vra.bat`) is deliberately **not in this repo** (see
  `.gitignore`), and the `.vcxproj` only compiles it if the files exist.
- With the define off (the default), those blocks compile out and the build is pure FSRA.
- Do not recreate or reverse-engineer that data, and do not commit those paths. Work on the FSRA
  model itself.

## Suggested next steps

- Fix bugs 1–3, then retune anything that was tuned around the wrong Mach number.
- Build a proper Adour Mk 151 thrust model with altitude and Mach lapse from public data, replacing
  the flat `setThrust`.
- Remove the `/FORCE:MULTIPLE` duplicate definitions.
- Test-fly after each change: check takeoff speed, climb rate, roll rate, and trim at a few speeds
  and altitudes.

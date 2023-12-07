# Twin City Platform 🏡

**Version:** 1.0  
**Tech lead:** Erik Junholm  
**Product Owner** Erik Junholm
**Developers:** David Beuger, Anders Kettisen  
**Hall of Fame:** Mikael Pettersson

This is the home of the Digital Twin project.

# Specification
- Coordinate system : SWEREF99TM 16 30 00
- Extent Nkpg
  - Min X: 129411,400, Y: 6495015,262 
  - Max X: 134211.400, Y: 6498915,262
  
- Scale : 1:1500

## Build & Run

This project has been built and tested on:
- Windows
- Unreal Engine 4.27
- Visual Studio 2019

## Plugins

The GeoReferencing plugin from UE 4.27 is currently used in the Digital Twin.

## Dependencies

- SimStad traffic server: [SimStad Traffic Server](https://gitlab.liu.se/Exploranation/City/simstad-repos/utils/trafficserver/digitaltwin-norrkoeping-utils)

## Roadmap

```mermaid
gantt
    title A Gantt Diagram
    dateFormat  YYYY-MM-DD
    section Section
    Datahandling             :a1, 2023-10-23, 30d
    Installation script      :after a1  , 20d
    section Low prio
    Improve Dynamic visuals  :a2, 2023-11-15, 30d
    Handle mediadataset      :after a2, 15d
```
# Third Party

- Zmq-plugin: [ZmqPluginforUnreal](https://gitlab.liu.se/Exploranation/City/)

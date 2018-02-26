- bottletype + bottletype array
  - alias
  - weight
  - write to eeprom (EEPROM.update)
  - store object with save method

- menu libraries
  - https://github.com/jonblack/arduino-menusystem
  - https://github.com/VaSe7u/LiquidMenu
  - https://github.com/neu-rah/ArduinoMenu

- button libraries
  - https://github.com/r89m/Button
  - https://github.com/pololu/pushbutton-arduino
  - https://github.com/poelstra/arduino-multi-button
  - https://github.com/julioterra/Switches
  - https://github.com/madleech/Button

- state machine
  - menu
  - waiting
  - filling
  - calibrating new bottle
  - https://www.norwegiancreations.com/2017/03/state-machines-and-arduino-implementation/
  - https://github.com/jonblack/arduino-fsm

- when to stop filling?
  - simplest: just adjust bottletype liquidweight based on line lenght and speed
    of scale + code
  - measure what the fillrate is
  - make response time a constant
  - if current weight + (respones time * fillrate) >= fillweight: stop valve

- choose (proportional) solenoid valve
  - http://www.burkert.com/en/sitesearch?search_term=proportional+valve
  - Elektroventiel / Servogestuurd ventiel
  - https://www.sossolutions.nl/996-brass-liquid-solenoid-valve-12v-12-nps
  - https://www.ebay.nl/sch/i.html?_from=R40&_sacat=0&_sop=15&_nkw=stainless+solenoid+valve&LH_PrefLoc=3&_pgn=24&_skc=1150&rt=nc

- projectbox
  - lcd display = 100x60
  - arduino = 53x80

- connectors
  - circular din
  - gx16
  - jack
  - how many terminals?

- BOM
  - 2 pin screw terminal 12v in
  - 2 pin screw terminal 12v out to valve
  - female jack mountable
  - male jack for valve cable
  - 4 wire cable for weight sensor
  - 2 wire cable for ??
  - 4 pin female connector mountable (project box)
  - 4 pin male connector cable (project box)
  - 4 pin female connector mountable (sensor side)
  - 4 pin male connector cable (sensor side)

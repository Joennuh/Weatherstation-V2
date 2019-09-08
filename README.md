# Weatherstation V2
**Open-source Weatherstation to build by yourself. Expandable with remote sensors.**

[![GitHub license](https://img.shields.io/github/license/Joennuh/Weatherstation-V2)](https://github.com/Joennuh/Weatherstation-V2/blob/master/LICENSE)
![GitHub repo size](https://img.shields.io/github/repo-size/Joennuh/Weatherstation-V2)
![GitHub last commit (branch)](https://img.shields.io/github/last-commit/Joennuh/Weatherstation-V2/master)
[![Gitter](https://badges.gitter.im/Weatherstation-V2/community.svg)](https://gitter.im/Weatherstation-V2/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

## Work in progress
This readme.md need to be completed but here is a small start.

## Short introduction
This software is part of the Weatherstation V2 project of The Big Site / Jeroen Maathuis (aka Joennuh). This project aims to provide a software solution for a weatherstation which acts as a central display unit to view all kind of weather related measurements on. Also it aims to provide a software solution for expansion with remote sensors.

## Current hardware setup
The hardware used is subject to changes but at the moment of writing this readme I use the following hardware in the lists below.
There might be parts missing. Please inform me if you think there should be a specific part on this list which is missing.
Hyperlinks after the products refer to the place where I bought those products. They are not affiliate links.

### Weatherstation
* LiliGO TTGO T-Display <https://www.aliexpress.com/item/33048962331.html>
* RGB led (Common Cathode) <https://www.aliexpress.com/item/32826209045.html>
* Warm white led <https://www.aliexpress.com/item/32892047864.html>
* DS18B20 temperature sensor (300 cm) <https://www.aliexpress.com/item/32949416515.html>

### Inside extra meter
* Wemos D1 mini <https://www.aliexpress.com/item/32651747570.html>
* ST7735S display <https://www.aliexpress.com/item/32888860613.html>
* DS18B20 temperature sensor (100 cm) <https://www.aliexpress.com/item/32265871189.html>
* 2 tactile push buttons

## Sources
This software is based on example sketches from several sources. Mostly the examples come from library's being used in this sketch.

Sources:
 * **TTGO T-Display**: <https://github.com/Xinyuan-LilyGO/TTGO-T-Display>
 * **ArduinoMenu**: <https://github.com/neu-rah/ArduinoMenu>
 * **TFT_eSPI**: <https://github.com/Bodmer/TFT_eSPI>
 * **Button2**: <https://github.com/LennartHennigs/Button2>

And probably I've forgotten to mention sources. In that case: I'm sorry for that.

## Dependencies
The following libraries are being used in the sketches of this project:
* **ArduinoMenu**: <https://github.com/neu-rah/ArduinoMenu>
* **TFT_eSPI**: <https://github.com/Bodmer/TFT_eSPI>
* **Button2**: <https://github.com/LennartHennigs/Button2>
* **OneWire**: <https://www.pjrc.com/teensy/td_libs_OneWire.html>
* **Arduino Library for Maxim Temperature Integrated Circuits**: <https://github.com/milesburton/Arduino-Temperature-Control-Library>

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.

The license is also enclosed with this repository in the `LICENSE` file: <https://github.com/Joennuh/Weatherstation-V2/blob/master/LICENSE>

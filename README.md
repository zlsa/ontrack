
# Overview

OnTrack is an advanced train simulator currently in
development.

# Usage

1. Clone this repository.
2. `make`.
3. `make install`.
4. `./bin/ontrack`
5. View the glorious... revolving triangle. Shut up, it's plain C.

To use the GE ES44C4 model, you must convert all SVG files in `data/`
to PNG (with the same relative path;
`data/cars/ge-es44c4/operators/bnsf/diffuse.svg` would be converted to
`data/cars/ge-es44c4/operators/bnsf/diffuse.png`).

# Components

* OnTrack simulator - the simulation/rendering main component.
* Route Creator - the web-based route creator.
* Exporters - used to export from Blender to OnTrack formats.

# Terminology

## Cars

A _car_ (also known as _rolling stock_) is a single movable unit,
designed to travel along a track. It can be either a train car or a
road vehicle; the simulator does not make any distinction between the
two.

## Train

One or more connected cars.

## Route

A single map, containing one or more tracks, plus information about
the route, times, scenery, weather, and track signage.

## Segment

A segment is something that a car can travel along; multiple rail
segments can be connected to form a railway track.

## Track

One or more connected segments. A track can be either a railway track
(used exclusively for trains) or a road (used for road vehicles, such as
sedans or tractor-trailers).

# License

All code is licensed under the MIT license.

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

All art, including 3D models and audio files, are licensed under the CC0 license.

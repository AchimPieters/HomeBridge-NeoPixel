# HomeBridge-NeoPixel

Supports NeoPixel Strips http(s) devices on the HomeBridge Platform and provides a readable
callback for getting and setting the following characteristics to Homekit:

* Characteristic.On
* Characteristic.Brightness
* Characteristic.Hue
* Characteristic.Saturation


# Installation

1. Install homebridge using: `npm install -g homebridge`
2. Install HomeBridge-NeoPixel using: `npm install -g HomeBridge-NeoPixel`
3. Update your configuration file.  See below for examples.

npm install git://github.com/<repo-owner>/<repo>.git

# Configuration

## Structure

The following is an overview of the structure of your HomeBridge-NeoPixel accessory.

Both `powerOn` and `powerOff` can either be a `string` or an `object`.  If a
`string` is provided it is filled in as the `url` and the `body` will be blank.
Most devices will be ok with the `string` option.

The purpose of `powerOff`/`powerOn` for an NeoPixel light is not to physically power
or de-power the device (as then how would it respond to further commands?), but
to set the LED color to black (for `powerOff`), and restore the color (for
`powerOn`).  Your backend device should already be doing this.  This is just
a convenience function so that your HomeBridge knows this device can turn off
or on.

Additionally, both `brightness` and `color` share the same structure (with the
exception that the `color` structure allows for a `.brightness` variable), they
can either be a `string` or an `object`.  If it is a `string`, it is filled in
as the `status` and the other fields are left blank. When this is the case, you
can only read the settings, you may not change them.


    {
        "accessory": "HTTP-NEO",
        "name": string,

        "http_method": string-optional,
        "username": string-optional,
        "password": string-optional,
        "sendImmediately": string-optional,

        "switch": {
            "status": url-optional,
            "powerOn": string-or-object,
            "powerOff": {
                url: string,
                body: string
            }
        },

        "lock": {
            "status": url-optional,
            "secure": string-or-object,
            "unsecure": {
                url: string,
                body: string
            }
        },

        "brightness": string-or-object,
        "color": {
            "status": url-status,
            "url": url-optional,
            "brightness": boolean,
            "http_method": string-optional
        }
    }


## Examples

### Full RGB Device

    "accessories": [
        {
            "accessory": "HTTP-NEO",
            "name": "HomeBridge-NeoPixel",

            "switch": {
                "status": "http://localhost/api/v1/status",
                "powerOn": "http://localhost/api/v1/on",
                "powerOff": "http://localhost/api/v1/off"
            },

            "brightness": {
                "status": "http://localhost/api/v1/brightness",
                "url": "http://localhost/api/v1/brightness/%s"
            },

            "color": {
                "status": "http://localhost/api/v1/set",
                "url": "http://localhost/api/v1/set/%s",
                "brightness": true
            }
        }
    ]

### Single Color Light that only turns "off" and "on"

    "accessories": [
        {
            "accessory": "HTTP-NEO",
            "name": "Single Color Light",

            "switch": {
                "status": "http://localhost/api/v1/status",
                "powerOn": "http://localhost/api/v1/on",
                "powerOff": "http://localhost/api/v1/off"
            }
        }
    ]

### Single Color Light with Brightness

    "accessories": [
        {
            "accessory": "HTTP-NEO",
            "name": "Single Color Light",

            "switch": {
                "status": "http://localhost/api/v1/status",
                "powerOn": "http://localhost/api/v1/on",
                "powerOff": "http://localhost/api/v1/off"
            },

            "brightness": {
                "status": "http://localhost/api/v1/brightness",
                "url": "http://localhost/api/v1/brightness/%s"
            }
        }
    ]

### RGB Light without Brightness

    "accessories": [
        {
            "accessory": "HTTP-NEO",
            "name": "Single Color Light",

            "switch": {
                "status": "http://localhost/api/v1/status",
                "powerOn": "http://localhost/api/v1/on",
                "powerOff": "http://localhost/api/v1/off"
            },

            "color": {
                "status": "http://localhost/api/v1/set",
                "url": "http://localhost/api/v1/set/%s"
            }
        }
    ]

This normally will not occur, however, you may not want your application to
display a "brightness" slider to the user.  In this case, you will want to
remove the brightness component from the config.


# Interfacing

All the `.status` urls expect a 200 HTTP status code and a body of a single
string with no HTML markup.

* `switch.status` expects `0` for Off, and `1` for On.
* `brightness.status` expects a number from 0 to 100.
* `color.status` expects a 6-digit hexidemial number.

## http-led-controller

This package was developed in tandem with
[jnovack/http-led-controller](https://github.com/jnovack/http-led-controller).

I needed a way to control my [blinksticks](https://www.blinkstick.com/) from
HomeKit/HomeBridge.  After developing this package, I had to have a backend in
the event someone's HomeKit/HomeBridge is on a different server, or in my case,
there are 4-5 different Raspberry Pi's running blinksticks around my house.



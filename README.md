# homebridge-neopixel

Supports NeoPixel http(s) devices on the HomeBridge Platform and provides a readable
callback for getting and setting the following characteristics to Homekit:

* Characteristic.On
* Characteristic.Brightness
* Characteristic.Hue
* Characteristic.Saturation

For more information visit: http://www.studiopieters.nl/apple-homebridge-neopixel-light/

# Installation

1. Install homebridge using: `npm install -g homebridge`
2. Install homebridge-http using: `npm install -g homebridge-neopixel`
3. Update your configuration file.  


# Configuration file

## Structure

The following is an overview of the structure of your HTTP-NEO accessory.

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




# Interfacing

All the `.status` urls expect a 200 HTTP status code and a body of a single
string with no HTML markup.

* `switch.status` expects `0` for Off, and `1` for On.
* `brightness.status` expects a number from 0 to 100.
* `color.status` expects a 6-digit hexidemial number.

### Update: if you use Homebridge version 2, It is possible that you will get a ECONNRESET-bug. But the good nes is we have a possible fix:

The error can be resolved by inserting `while (client.read() >= 0);`right before`client.stop();`

As the previous code just cuts the communication (thats where the ECONNRESET comes from) without reading the remaining buffer, homebridge sees unsent packages and a broken channel. This is solvable by simply emptying the buffer and close it afterwards (as there is no other useful information in the communication).
 

Homebridge-NeoPixel is based on homebridge-better-http-rgb.

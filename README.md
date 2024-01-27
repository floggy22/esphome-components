# esphome_components

own or customized components for esphome

## Dew Point

Calculate the dew point for a given temperature and humidity

Import:  
How to import external components: <https://esphome.io/components/external_components.html>

```yaml
  external_components:
    - source:
        type: git
        url: https://github.com/floggy22/esphome_components
        ref: main    
      components: [ dew_point ]
```

Use:

```yaml
    sensor:  
        - platform: dew_point
          name: Dew Point
          temperature: sensor_id_temperature
          humidity: sensor_id_humidity
```

* name (Required, string): The name for the dew point sensor.
* temperature (Required, ID): The sensor that is used to measure the current temperature, in °C.
* humidity (Required, ID): The sensor that is used to measure the current relative humidity, in %.

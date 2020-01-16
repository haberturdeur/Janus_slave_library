# Janus_slave_library

- = none
+ = optional(depends on slave type)

# received command table
| CMD | Data 0 | Data 1 | Data 2 | Data 3 | Data 4 | Comment|
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|a| new addr | - | - | - | - | setting new address|
|n|byte 0|byte 1|byte 2|byte 3|-| Current session number|
|r|what to read|-|-|-|-| Read from slave(0 means current output_data)|
|w|what to write|value0|value1|value2|value3| write data to output(relay, ....)|
|s|what to set|value0|value1|value2|value3| set parametr of slave|


#sending command table
| CMD | Data 0 | Data 1 | Data 2 | Data 3 | Data 4 | Comment|
|r| Data 0 | Data 1 | Data 2 | Data 3 | Data 4 | Respond to master/send data|
|w| slave_type | + | + | + | + | sending welcome message |

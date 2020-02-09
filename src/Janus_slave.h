#pragma once

#include "Janus_decoder.h"
#include "Janus_slave_types_list.h"
#include "Janus_command_handler.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <vector>
#include <memory>

#define debug_slave true

#define DEBUG_MESSAGE_V(X,...) if(debug_decoder) printf(X, __VA_ARGS__)
#define DEBUG_MESSAGE(X) if(debug_decoder) printf(X)

#define ERROR           0b1<<1
#define STARTED         0b1<<1
#define INITIALIZED     0b1<<2
#define SYNCHRONIZED    0b1<<3
#define RENEWED_ADDRESS 0b1<<4
#define CONNECTED       0b1<<5
#define DATA_READY      0b1<<6

#define MAX_REPETITION 10

const uint8_t broadcast_addr = 254;
const char *nvs_name_space = "Janus";
const char *session_number_nvs_key = "session";
const char *last_address_nvs_key = "address";

enum sending_mode_t : uint8_t{
    ON_REQUEST,
    SINGLE,
    PERIODICAL,
    CONSTANT
};


const uint8_t default_master_address = 0x00;


class Base_Janus_slave_t
{
private:
    Decoder decoder;
    Command_handler cmd_handler;

    nvs_handle nvs_handler;
    bool nvs_dirty = false;

    uint8_t my_slave_type;

    uint64_t current_session_number = 0;
    uint64_t last_session_number;
    
    uint8_t master_address = default_master_address;

    uint8_t my_current_address;
    uint8_t my_last_address = 0;

    // sending_mode_t sending_mode = ON_REQUEST;

    uint8_t state = 0b00000000;
    
    std::vector<uint8_t>* output_data;
    std::vector<uint8_t> input_data;

    esp_err_t start_nvs();
    void commit_to_nvs();
    uint64_t get_session_from_nvs();
    uint8_t get_address_from_nvs();
    void get_everything_from_nvs();
    void save_session_to_nvs(uint64_t session);
    void save_address_to_nvs(uint8_t address);
    void save_everything_to_nvs();

    virtual void receive();

    void synchronize(int repetition = 0);
    void connect(int repetition = 0);

public:
    void init(Janus_decoder_settings_t& decoder_settings = Janus_decoder_default_settings);

    uint8_t begin();

    virtual void run();
    
    void set_output_data(std::vector<uint8_t>* output){
            output_data = output;
            state |= DATA_READY;     
        }

    Command_handler& getCommandHandler(){
            return cmd_handler;
        }

    Decoder& getDecoder(){
        return decoder;
    }

    uint64_t getSessionNumber(){
        return current_session_number;
    }

    uint8_t getAddress(){
        return my_current_address;
    }

    uint8_t getMasterAddress(){
        return master_address;
    }

    uint8_t setMasterAddress(uint8_t new_address = default_master_address){
        master_address = new_address;
        return getMasterAddress();
    }
};


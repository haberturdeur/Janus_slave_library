#pragma once

#include "Janus_decoder.h"
#include "Janus_slave_types_list.h"
#include "command_list.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <vector>
#include <memory>


const uint8_t broadcast_addr = 254;
const char *nvs_name_space = "Janus";
const char *session_number_nvs_key = "session";
const char *last_address_nvs_key = "address";

enum Janus_slave_type_t:uint8_t {
    OMNI_ULTRA,



    NON_STANDARDIZED = 255
};



enum sending_mode_t : uint8_t{
    ON_REQUEST,
    SINGLE,
    PERIODICAL,
    CONSTANT
};



class Base_Janus_slave_t
{
    private:
        Decoder* decoder;
        
        
        nvs_handle nvs_handler;
        bool nvs_dirty = false;


        uint8_t my_slave_type;

        uint64_t current_session_number;
        uint64_t last_session_number;
        
        uint8_t master_address = 0;

        uint8_t my_current_address;
        uint8_t my_last_address;
        sending_mode_t sending_mode = ON_REQUEST;
        
        std::vector<uint8_t>* output_data;
        std::vector<uint8_t> input_data;
        std::vector<uint8_t> decoded_data;

        

        esp_err_t start_nvs();
        void commit_to_nvs();
        uint64_t get_session_from_nvs();
        uint8_t get_address_from_nvs();
        void get_everything_from_nvs();
        void save_session_to_nvs(uint64_t session);
        void save_address_to_nvs(uint8_t address);
        void save_everything_to_nvs();

        uint64_t receive_session_number(int repetetion = 0);
        uint8_t receive_address(int repetetion = 0);
        void send_welcome_message();

        message_t receive();
        void act(Janus_commands::id_t cmd, bool broadcast = false);
        void act_broadcast(Janus_commands::id_t cmd);
        void respond();

    public:
        Base_Janus_slave_t(Decoder* Janus_decoder, 
            Janus_slave_type_t type, 
            std::map<Janus_commands::id_t, Janus_commands::command_t>& comms = Janus_commands::Janus_default_commands,
            std::map<std::string, Janus_commands::id_t>& ids = Janus_commands::Janus_default_command_ids);

        void connect();
        void run();

        void set_output_data(std::vector<uint8_t>* output){
            output_data = output;
        }
};
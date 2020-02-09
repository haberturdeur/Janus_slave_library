#include "Janus_slave.h"
#include <string>


void Base_Janus_slave_t::init(Janus_decoder_settings_t& decoder_settings){
    decoder.init(decoder_settings);
    start_nvs();
    get_everything_from_nvs();
    DEBUG_MESSAGE("Slave initialized.\n");
}

esp_err_t Base_Janus_slave_t::start_nvs(){
    DEBUG_MESSAGE("Starting NVS\n");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs_open(nvs_name_space, NVS_READWRITE, &nvs_handler));
    DEBUG_MESSAGE_V("NVS started with: %li \n", err);
    return err;
}

void Base_Janus_slave_t::commit_to_nvs(){
     esp_err_t err = nvs_commit(nvs_handler);
    nvs_dirty = false;
    DEBUG_MESSAGE_V("Commited to NVS with: %li \n", err);
}

uint64_t Base_Janus_slave_t::get_session_from_nvs(){
    uint64_t value;
    esp_err_t err =nvs_get_u64(nvs_handler, session_number_nvs_key, &value);
    ESP_ERROR_CHECK(err);
    DEBUG_MESSAGE_V("Read session from NVS with: %li, current sessin number is: %llu \n", err, value);
    return value;
}

uint8_t Base_Janus_slave_t::get_address_from_nvs(){
    uint8_t value;
    esp_err_t err =nvs_get_u8(nvs_handler, last_address_nvs_key, &value);
    ESP_ERROR_CHECK(err);
    DEBUG_MESSAGE_V("Read session from NVS with: %li, current sessin number is: %u \n", err, value);
    return value;
}

void Base_Janus_slave_t::get_everything_from_nvs(){
    last_session_number = get_session_from_nvs();
    my_last_address = get_address_from_nvs();
}

void Base_Janus_slave_t::save_session_to_nvs(uint64_t session){
    esp_err_t err = nvs_set_u64(nvs_handler, session_number_nvs_key, session);
    ESP_ERROR_CHECK(err);
    nvs_dirty = true;
    DEBUG_MESSAGE_V("Wrote session to NVS with: %li \n", err);
}

void Base_Janus_slave_t::save_address_to_nvs(uint8_t address){
    esp_err_t err =nvs_set_u8(nvs_handler, last_address_nvs_key, address);
    ESP_ERROR_CHECK(err);
    nvs_dirty = true;
    DEBUG_MESSAGE_V("Wrote address to NVS with: %li \n", err);
}

void Base_Janus_slave_t::save_everything_to_nvs(){
    save_session_to_nvs(current_session_number);
    save_address_to_nvs(my_current_address);
    commit_to_nvs();
}

void Base_Janus_slave_t::receive(){
    message_t input = decoder.receive();
    if(input.send_addr == master_address){
        BaseResult* base;
        base = cmd_handler.run(input.cmd, &input.data);
        bool broadcast = (input.rec_addr == broadcast_addr)? 1 : 0;
        switch (base->getType())
        {
        case addr_r:
                AddressResult* result = static_cast<AddressResult*>(base);
                my_current_address = result->getValue();
                state |= RENEWED_ADDRESS;
                state &= ~CONNECTED;
            break;
        case session_r:
                SessionResult* result = static_cast<SessionResult*>(base);
                current_session_number = result->getValue();
            break;
        
        default:

            break;
        }
    }
}

void Base_Janus_slave_t::synchronize(int repetition){
    receive();
    if(current_session_number == last_session_number){
        save_session_to_nvs(current_session_number);
        state |= SYNCHRONIZED;
        connect();
    }else if(repetition<=MAX_REPETITION){
        state &= ~SYNCHRONIZED;
        vTaskDelay((1000 / portTICK_PERIOD_MS));
        synchronize(repetition + 1);
    }else{
        state &= ~SYNCHRONIZED;
        state |= ERROR;
    }
}

void Base_Janus_slave_t::connect(int repetition){
    receive();
    if(!(state & RENEWED_ADDRESS)){
        connect(repetition + 1);
    }else{
        state |= CONNECTED;
    }
}



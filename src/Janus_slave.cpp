
#include "Janus_slave.h"
#include "command_list.h"
#include <string>

Base_Janus_slave_t::Base_Janus_slave_t(Decoder *Janus_decoder, 
    Janus_slave_type_t type, 
    std::map<Janus_commands::id_t, Janus_commands::command_t>& comms, 
    std::map<std::string, Janus_commands::id_t>& ids):
        cmd_handler{&decoded_data, Janus_decoder, comms, ids}
{
    decoder = Janus_decoder;
    my_slave_type = type;
    start_nvs();
    get_everything_from_nvs();    
}


esp_err_t Base_Janus_slave_t::start_nvs(){
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs_open(nvs_name_space, NVS_READWRITE, &nvs_handler));
    return err;
}

void Base_Janus_slave_t::commit_to_nvs(){
    nvs_commit(nvs_handler);
    nvs_dirty = false;
}

uint64_t Base_Janus_slave_t::get_session_from_nvs(){
    uint64_t value;
    ESP_ERROR_CHECK(nvs_get_u64(nvs_handler, session_number_nvs_key, &value));
    return value;
}

uint8_t Base_Janus_slave_t::get_address_from_nvs(){
    uint8_t value;
    ESP_ERROR_CHECK(nvs_get_u8(nvs_handler, last_address_nvs_key, &value));
    return value;
}

void Base_Janus_slave_t::get_everything_from_nvs(){
    last_session_number = get_session_from_nvs();
    my_last_address = get_address_from_nvs();
}

void Base_Janus_slave_t::save_session_to_nvs(uint64_t session){
    ESP_ERROR_CHECK(nvs_set_u64(nvs_handler, session_number_nvs_key, session));
    nvs_dirty = true;
}

void Base_Janus_slave_t::save_address_to_nvs(uint8_t address){
    ESP_ERROR_CHECK(nvs_set_u8(nvs_handler, last_address_nvs_key, address));
    nvs_dirty = true;
}

void Base_Janus_slave_t::save_everything_to_nvs(){
    save_session_to_nvs(current_session_number);
    save_address_to_nvs(my_current_address);
    commit_to_nvs();
}

uint64_t Base_Janus_slave_t::receive_session_number(int repetetion)
{   
    printf("Receiving session number...\n");
    uint64_t output;
    Janus_commands::SessionResult output_class;
    message_t received = decoder->receive();
    if ((received.cmd == cmd_handler.command_ids["session"]) && (received.send_addr == master_address))
    {
        decoded_data = received.data;
        cmd_handler.act(received.cmd, &output_class);
        output = output_class.session_id;
    }
    else if (repetetion < 10)
    {
        output = receive_session_number(repetetion);
    }else output = 0;
    printf("Received session number: %llu \n", output);
    return output;
   
}

uint8_t Base_Janus_slave_t::receive_address(int repetetion)
{   
    printf("Receiving new address...\n");
    message_t received = decoder->receive();
    uint8_t output;
    Janus_commands::AddressResult output_class;
    if ((received.cmd == cmd_handler.command_ids["address_change"]) && (received.send_addr == master_address))
    {
        decoded_data = received.data;
        cmd_handler.act(received.cmd, &output_class);
        output = output_class.address;
    }
    else if (repetetion < 10)
    {
        output = receive_session_number(repetetion);
    }else output = 0;
    printf("Received session number: %llu \n", output);
    return output;
}

void Base_Janus_slave_t::connect(){
    printf("Connecting to master...\n");
    current_session_number = receive_session_number();
    if(current_session_number == last_session_number){
        my_current_address = my_last_address;
        printf("Session number hasn't changed.\n Session number: %llu \n. My address: %u \n",current_session_number, my_current_address );
    }else{
        my_current_address = receive_address();
        printf("Session number has changed.\n Session number: %llu .\n My address: %u. \n",current_session_number, my_current_address );
    }
    save_everything_to_nvs();
    printf("Succesfully connected.\n");
}

void Base_Janus_slave_t::send_welcome_message(){
    std::vector<uint8_t> output;
    output.push_back(my_slave_type);                                                                
    decoder->send(master_address, my_current_address, cmd_handler.command_ids["welcome"], &output);
}

message_t Base_Janus_slave_t::receive(){
    printf("Receiving...");
    message_t input = decoder->receive();
    // if(input.send_addr == master_address){
    //     if(input.rec_addr == my_current_address){
    //         uint8_t output = input.cmd;
    //         printf("Received message with command: %u", output);
    //         if(output == 'r'){
    //             printf("Responding to direct request...\n");
    //             decoder->send(master_address, my_current_address, 'w', output_data);
    //         }else{
    //             printf("Message wasn't direct request...\n");
    //             act(input);
    //         }
    //     }else if(input.rec_addr == broadcast_addr){
    //         printf("Message is a broadcast...\n");
    //         act_broadcast(input);
    //     }
    // }
    return input;
}

void Base_Janus_slave_t::act(Janus_commands::id_t cmd, bool broadcast){
    if(broadcast) act_broadcast(cmd);
    Janus_commands::BaseResult base;
    cmd_handler.act(cmd, &base);
    switch (base.getType())
    {
    case 0x4:
            Janus_commands::AddressResult* result = dynamic_cast<Janus_commands::AddressResult*> (&base);
        break;
    
    default:
        break;
    }
}


#pragma once
#include <functional>
#include <map>
#include <vector>

namespace Janus_commands{
    
    std::map<std::string, id_t> Janus_default_command_ids = {
        {"read", 0x1},
        {"write", 0x2},
        {"welcome", 0x3},
        {"address_change", 0x4},
        {"session", 0x5}
    };

    

    

    class SessionResult : public BaseResult{
        public:
            int getType() { return Janus_default_command_ids["session"];};
            uint64_t session_id;
    };

    class AddressResult : public BaseResult{
        public:
            int getType() { return Janus_default_command_ids["address_change"];};
            uint8_t address;
    };
    
    typedef std::function<void(input_t*, BaseResult*)> command_t;

    command_t read = [](input_t *in, BaseResult *out) {};
    command_t write = [](input_t *in, BaseResult *out) {};
    command_t welcome = [](input_t *in, BaseResult *out) {};
    command_t address_change = [](input_t *in, BaseResult *out) {};
    command_t session = [](input_t *in, BaseResult *out) {
        SessionResult output;
        for (int i = 0; i < 4; i++)
        {
            uint64_t a = (*in)[i];
            a = a << (8 * (4 - i));
            output.session_id += a;
        }
        *out = output; 
    };

    std::map<id_t, command_t> Janus_default_commands = {
        {Janus_default_command_ids["read"], read},
        {Janus_default_command_ids["write"], write},
        {Janus_default_command_ids["welcome"], welcome},
        {Janus_default_command_ids["address_change"], address_change},
        {Janus_default_command_ids["session"], session}
    };
};



class Command_handler
{
    private:
        Janus_commands::input_t *in_data;
        Decoder *decoder;
        
        

    public:
        Command_handler(Janus_commands::input_t *in_d,
            Decoder *dec,
            std::map<Janus_commands::id_t, Janus_commands::command_t> &comms = Janus_commands::Janus_default_commands,
            std::map<std::string, Janus_commands::id_t> &names = Janus_commands::Janus_default_command_ids):               
                in_data{in_d},
                decoder {dec},
                commands{Janus_commands::Janus_default_commands},
                command_ids{Janus_commands::Janus_default_command_ids}
                {
                    commands.insert(comms.begin(), comms.end());
                    command_ids.insert(names.begin(), names.end());
                }
        void act(Janus_commands::id_t in_cmd, Janus_commands::BaseResult* out_data);
        std::map<Janus_commands::id_t, Janus_commands::command_t> &commands;
        std::map<std::string, Janus_commands::id_t> &command_ids;

        
};

void Command_handler::act(Janus_commands::id_t in_cmd, Janus_commands::BaseResult* out_data)
{
    Janus_commands::command_t &run = commands[in_cmd];
    run(in_data, out_data);
}


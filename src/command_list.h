#pragma once
#include <functional>
#include <map>
#include <vector>

typedef uint8_t id_t;
typedef std::function<void(std::vector<uint8_t> *)> command_t;
typedef std::vector<uint8_t> input_t;

namespace Janus_commands
{
    command_t read = [](input_t *in) {};
    command_t write = [](input_t *in) {};
    command_t welcome = [](input_t *in) {};
    command_t address_change = [](input_t *in) {};
    command_t session = [](input_t *in) {};

    std::map<std::string, id_t> Janus_default_command_ids = {

    };

    std::map<id_t, command_t> Janus_default_commands = {
        {0x1, read},
        {0x2, write},
        {0x3, welcome},
        {0x4, address_change},
        {0x5, session}
        };
};

class Command_handler
{
    private:
        input_t *in_data; 
        Decoder *decoder;
        std::map<id_t, command_t> *commands;
        std::map<std::string, id_t> *command_ids;


    public:
        Command_handler(input_t *in_d,
            Decoder *dec,
            std::map<id_t, command_t> *comms = &Janus_commands::Janus_default_commands,
            std::map<std::string, id_t> *names = &Janus_commands::Janus_default_command_ids):               
                in_data{in_d},
                decoder {dec},
                commands{&Janus_commands::Janus_default_commands},
                command_ids{&Janus_commands::Janus_default_command_ids}
                {
                    commands->insert(comms->begin(), comms->end());
                    command_ids->insert(names->begin(), names->end());
                }
        void act(id_t in_cmd);
        // void init();
        
};

void Command_handler::act(id_t in_cmd)
{
    command_t &run = (*commands)[in_cmd];
    run(in_data);
}


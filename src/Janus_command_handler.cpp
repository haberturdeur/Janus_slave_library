#include "Janus_command_handler.h"

void Command_handler::store(Janus_command_base* cmd, id_t cmd_id, std::string cmd_name){
    if(cmd){
        commands_.insert({cmd_id, cmd});
        names_.insert({cmd_name, cmd_id}); 
    }
}

void Command_handler::store(std::map<id_t, Janus_command_base*>* ids, std::map<std::string, id_t>* names){
    commands_.insert(ids->begin(), ids->end());
    names_.insert(names->begin(), names->end());
}

void Command_handler::setInput(input_t* in){
    input = in;
}

BaseResult* Command_handler::run(id_t id, input_t* in){
    return commands_[id]->Execute((in==nullptr) ? input : in);
}

BaseResult* Command_handler::run(std::string name, input_t* in){
    return run(names_[name], in);
}

id_t Command_handler::getId(std::string name){
    return names_[name];
}

std::string Command_handler::getName(id_t id){
    for (std::map<std::string, id_t>::iterator it = names_.begin(); it != names_.end(); ++it)
            if(it->second == id)
                return it->first;
    return "error";
}

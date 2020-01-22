#pragma once

#include <vector>
#include <memory>
#include <map>
#include <string>
#include <vector>

typedef uint8_t id_t;
typedef std::vector<uint8_t> input_t;

enum{
    u8_r = 1,
    u16_r,
    u32_r,
    u64_r,
    i8_r,
    i16_r,
    i32_r,
    i64_r,
    float_r,
    char_r,
    bool_r,
    templated_r,
    max_r
};

class BaseResult {
    public:
        virtual uint8_t getType() = 0;
};

class Janus_command_base {
 public:
  virtual ~Janus_command_base() = default;
  virtual BaseResult* Execute(input_t* in) = 0;
};

template<class T>
class templated_result : public BaseResult{
    public:
        uint8_t getType(){ return templated_r;};
        T getValue(){return value;};
    private:
        T value;
};


class Command_handler{
    public:
        void store(Janus_command_base* cmd, id_t cmd_id, std::string cmd_name);
        void store(std::map<id_t, Janus_command_base*>* ids, std::map<std::string, id_t>* names);
        void setInput(input_t* in);
        BaseResult* run(id_t id, input_t* in = NULL);
        BaseResult* run(std::string name, input_t* in = NULL);
        id_t getId(std::string name);
        std::string getName(id_t id);
    private:
        std::map<id_t, Janus_command_base*> commands_ = {};
        std::map<std::string, id_t> names_ = {
            {"read", 0x1},
            {"write", 0x2},
            {"welcome", 0x3},
            {"address_change", 0x4},
            {"session", 0x5}
        };
        input_t* input;
};
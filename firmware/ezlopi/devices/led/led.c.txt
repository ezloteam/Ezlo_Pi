#include "led.h"
#include <string>
#include <vector>
#include "frozen.h"

led *led::led_ = nullptr;

static std::vector<string (*)(string &payload)> command_list = {
    led::set_config,
    led::get_config,
    led::clear_config,
    led::set_state,
};

string led::process_json(string &payload)
{
    string ret = "{\"resp_id\": 2}";
    int cmd_id = 0;
    if (json_scanf(payload.c_str(), payload.length(), "{cmd_id: %d}", &cmd_id) && (cmd_id > 1))
    {
        ret = command_list[cmd_id - 1](payload);
    }

    return ret;
}

string led::set_config(string &paylaod)
{
    string ret = "{\"resp_id\": 2}";
    return ret;
}

string led::get_config(string &payload)
{
    string ret = "{\"resp_id\": 3}";
    return ret;
}

string led::set_state(string &payload)
{
    string ret = "";
    return ret;
}

string led::clear_config(string &payload)
{
    string ret = "";
    return ret;
}

int led::store_config(string &config)
{
    int ret = 0;

    return ret;
}

int led::clear_selected(string &config)
{
    int ret = 0;

    return ret;
}

int led::clear_all(string &config)
{
    int ret = 0;

    return ret;
}

int led::init_pin(s_led_t &pin)
{
    int ret = 0;

    return ret;
}

int led::deinit_pin(s_led_t &pin)
{
    int ret = 0;

    return ret;
}

int led::init_all_pin()
{
    int ret = 0;

    return ret;
}

int led::deinit_all_pin()
{
    int ret = 0;

    return ret;
}

led *led::get_instance(void)
{
    if (led_ == nullptr)
    {
        led_ = new led();
    }

    return led_;
}
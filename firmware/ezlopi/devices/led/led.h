#ifndef __LED_H__
#define __LED_H__

#include <string>
// #include "devices.h"
#include "driver/gpio.h"

using namespace std;

typedef struct s_led
{
    char led_name[16];
    gpio_num_t gpio_num;
    gpio_config_t gpio_config;
} s_led_t;

class led // : public devices_abc
{
private:
    int init_pin(s_led_t &pin);
    int deinit_pin(s_led_t &pin);
    int init_all_pin();
    int deinit_all_pin();
    int store_config(string &config);
    int clear_selected(string &config);
    int clear_all(string &config);

protected:
    led() {}

    static led *led_;

public:
    /**
     * led should not be cloneable.
     */
    led(led &other) = delete;

    /**
     * led should not be assignable.
     */
    void operator=(const led &) = delete;

    static led *get_instance(void);

    void init();
    void deinit();

    static string process_json(string &payload);

    static string set_config(string &paylaod);
    static string get_config(string &payload);
    static string set_state(string &payload);
    static string clear_config(string &payload);
};

#endif // __LED_H__
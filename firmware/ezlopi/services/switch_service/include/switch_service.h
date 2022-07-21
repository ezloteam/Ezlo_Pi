#ifndef __GPIO_INTERRUPT_SERVICE_H__
#define __GPIO_INTERRUPT_SERVICE_H__

class switch_service
{
private:
    void init(void);
    static void gpio_intr_proces(void *pv);
    static uint32_t debounce(uint32_t pin_n);

public:
    static switch_service *get_instance(void);

    switch_service(switch_service &other) = delete;
    void operator=(const switch_service &) = delete;

protected:
    switch_service() {}
    static switch_service *switch_service_;
};

#endif // __GPIO_INTERRUPT_SERVICE_H__
/*
 * LED Pattern Controller with Responsive Button Toggle
 * 
 * This program controls 8 LEDs in two different animation patterns:
 *   - Pattern 0: Alternating odd/even LEDs
 *   - Pattern 1: Larson scanner (Knight Rider effect)
 * 
 * A button press toggles between patterns.
 * Button is checked frequently for immediate response.
 */

 #include <stdio.h>
 #include "mxc_device.h"
 #include "gpio.h"
 #include "pb.h"
 #include "mxc_delay.h"
 
 #define BUTTON_PORT MXC_GPIO2
 #define BUTTON_PIN  MXC_GPIO_PIN_3
 #define BUTTON_MASK (MXC_GPIO_PIN_3)
 
 
 // ---------------- LED DEFINITIONS ----------------
 
 const mxc_gpio_cfg_t led_cfgs[] = {
     {MXC_GPIO1, MXC_GPIO_PIN_6,  MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH}, // LED1
     {MXC_GPIO0, MXC_GPIO_PIN_9,  MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH}, // LED2
     {MXC_GPIO0, MXC_GPIO_PIN_8,  MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH}, // LED3
     {MXC_GPIO0, MXC_GPIO_PIN_11, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH}, // LED4
     {MXC_GPIO0, MXC_GPIO_PIN_19, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH}, // LED5
     {MXC_GPIO3, MXC_GPIO_PIN_1,  MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH}, // LED6
     {MXC_GPIO0, MXC_GPIO_PIN_16, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH}, // LED7
     {MXC_GPIO0, MXC_GPIO_PIN_17, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH}  // LED8
 };
 
 #define NUM_LEDS 8
 
 // Global mode variable accessible to all functions
 int mode = 0;
 
 
 // ---------------- LED HELPERS ----------------
 
 void leds_off(void)
 {
     for (int i = 0; i < NUM_LEDS; i++) {
         MXC_GPIO_OutClr(led_cfgs[i].port, led_cfgs[i].mask);
     }
 }
 
 void led_on(int index)
 {
     MXC_GPIO_OutSet(led_cfgs[index].port, led_cfgs[index].mask);
 }
 
 void led_off(int index)
 {
     MXC_GPIO_OutClr(led_cfgs[index].port, led_cfgs[index].mask);
 }
 
 
 // ---------------- BUTTON FUNCTIONS ----------------
 
 void init_button(void)
 {
     mxc_gpio_cfg_t btn;
     btn.port  = BUTTON_PORT;
     btn.mask  = BUTTON_MASK;
     btn.func  = MXC_GPIO_FUNC_IN;
     btn.pad   = MXC_GPIO_PAD_PULL_UP;
     btn.vssel = MXC_GPIO_VSSEL_VDDIO;
     MXC_GPIO_Config(&btn);
 }
 
 int button_pressed(void)
 {
     if (!(BUTTON_PORT->in & BUTTON_MASK)) {
         MXC_Delay(MXC_DELAY_MSEC(30));
         if (!(BUTTON_PORT->in & BUTTON_MASK)) {
             while (!(BUTTON_PORT->in & BUTTON_MASK));
             MXC_Delay(MXC_DELAY_MSEC(30));
             return 1;
         }
     }
     return 0;
 }
 
 
 // ---------------- RESPONSIVE DELAY WITH BUTTON CHECKING ----------------
 
 /*
  * delay_with_button_check(int total_ms)
  * 
  * Delays for specified milliseconds while checking button periodically.
  * If button is pressed during delay, toggles mode and provides visual feedback.
  * 
  * This makes button response feel immediate even during long pattern delays.
  * 
  * Parameters:
  *   total_ms - Total delay time in milliseconds
  * 
  * Returns:
  *   1 if button was pressed during delay (mode changed)
  *   0 if no button press occurred
  */
 int delay_with_button_check(int total_ms)
 {
     int chunks = total_ms / 50;  // Check button every 50ms
     int remainder = total_ms % 50;
     
     for (int i = 0; i < chunks; i++) {
         MXC_Delay(MXC_DELAY_MSEC(50));
         
         if (button_pressed()) {
             mode ^= 1;  // Toggle mode
             leds_off();
             MXC_Delay(MXC_DELAY_MSEC(200));
             return 1;  // Indicate mode changed
         }
     }
     
     // Handle remainder
     if (remainder > 0) {
         MXC_Delay(MXC_DELAY_MSEC(remainder));
     }
     
     return 0;  // No mode change
 }
 
 
 // ---------------- LED PATTERN: ODD / EVEN ----------------
 
 /*
  * pattern_odd_even()
  * 
  * Alternates between even and odd LEDs with responsive button checking.
  * If button is pressed during pattern, immediately returns to switch patterns.
  */
 void pattern_odd_even(void)
 {
     // Phase 1: Even indices
     for (int i = 0; i < NUM_LEDS; i++) {
         if (i % 2 == 0) led_on(i);
         else led_off(i);
     }
     
     if (delay_with_button_check(400)) return;  // Exit if mode changed
 
     // Phase 2: Odd indices
     for (int i = 0; i < NUM_LEDS; i++) {
         if (i % 2 == 1) led_on(i);
         else led_off(i);
     }
     
     delay_with_button_check(400);  // Check during second phase too
 }
 
 
 // ---------------- LED PATTERN: LARSON SCANNER ----------------
 
 /*
  * pattern_larson()
  * 
  * Bouncing LED scanner with responsive button checking.
  * Button checked during each frame delay for immediate response.
  */
 void pattern_larson(void)
 {
     static int pos = 0;
     static int dir = 1;
 
     leds_off();
     led_on(pos);
     
     if (delay_with_button_check(120)) return;  // Exit if mode changed
 
     pos += dir;
 
     if (pos == NUM_LEDS - 1) dir = -1;
     if (pos == 0) dir = 1;
 }
 
 
 // ---------------- SETUP ----------------
 
 void setup_leds(void)
 {
     for (int i = 0; i < NUM_LEDS; i++) {
         MXC_GPIO_Config(&led_cfgs[i]);
         MXC_GPIO_OutClr(led_cfgs[i].port, led_cfgs[i].mask);
     }
 }
 
 
 // ---------------- MAIN ----------------
 
 int main(void)
 {
     setup_leds();
     init_button();
 
     while (1) {
         if (mode == 0) {
             pattern_odd_even();
         } else {
             pattern_larson();
         }
     }
 
     return 0;
 }
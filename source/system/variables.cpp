#include "headers.hpp"

bool var_connect_test_succes = false;
bool var_need_reflesh = true;
bool var_allow_send_app_info = false;
bool var_night_mode = false;
bool var_eco_mode = true;
bool var_debug_mode = false;
bool var_flash_mode = false;
bool var_wifi_enabled = false;
bool var_high_resolution_mode = true;
bool var_3d_mode = false;
bool var_turn_on_top_lcd = true;
bool var_turn_on_bottom_lcd = true;
bool var_core_2_available = false;
bool var_core_3_available = false;
u8 var_wifi_state = 0;
u8 var_wifi_signal = 0;
u8 var_battery_charge = 0;
int var_hours = 0;
int var_minutes = 0;
int var_seconds = 0;
int var_days = 0;
int var_months = 0;
int var_years = 0;
int var_battery_level_raw = 0;
int var_afk_time = 0;
int var_free_ram = 0;
int var_free_linear_ram = 0;
int var_lcd_brightness = 100;
int var_top_lcd_brightness = 100;
int var_bottom_lcd_brightness = 100;
int var_time_to_turn_off_lcd = 150;
int var_num_of_app_start = 0;
int var_system_region = 0;
double var_scroll_speed = 0.5;
double var_battery_voltage = 0;
char var_status[128];
std::string var_clipboard = "";
std::string var_connected_ssid = "";
std::string var_lang = "en";
std::string var_model = "Unknown";
C2D_Image var_square_image[1];

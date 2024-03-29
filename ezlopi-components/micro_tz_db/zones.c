#include "zones.h"
#include <stdio.h>

static const micro_tz_db_pair micro_tz_db_tzs[] = {
    {"Africa/Abidjan", "GMT0"},
    {"Africa/Accra", "GMT0"},
    {"Africa/Addis_Ababa", "EAT-3"},
    {"Africa/Algiers", "CET-1"},
    {"Africa/Asmara", "EAT-3"},
    {"Africa/Bamako", "GMT0"},
    {"Africa/Bangui", "WAT-1"},
    {"Africa/Banjul", "GMT0"},
    {"Africa/Bissau", "GMT0"},
    {"Africa/Blantyre", "CAT-2"},
    {"Africa/Brazzaville", "WAT-1"},
    {"Africa/Bujumbura", "CAT-2"},
    {"Africa/Cairo", "EET-2EEST,M4.5.5/0,M10.5.4/24"},
    {"Africa/Casablanca", "GMT-1"},
    {"Africa/Ceuta", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Africa/Conakry", "GMT0"},
    {"Africa/Dakar", "GMT0"},
    {"Africa/Dar_es_Salaam", "EAT-3"},
    {"Africa/Djibouti", "EAT-3"},
    {"Africa/Douala", "WAT-1"},
    {"Africa/El_Aaiun", "GMT-1"},
    {"Africa/Freetown", "GMT0"},
    {"Africa/Gaborone", "CAT-2"},
    {"Africa/Harare", "CAT-2"},
    {"Africa/Johannesburg", "SAST-2"},
    {"Africa/Juba", "CAT-2"},
    {"Africa/Kampala", "EAT-3"},
    {"Africa/Khartoum", "CAT-2"},
    {"Africa/Kigali", "CAT-2"},
    {"Africa/Kinshasa", "WAT-1"},
    {"Africa/Lagos", "WAT-1"},
    {"Africa/Libreville", "WAT-1"},
    {"Africa/Lome", "GMT0"},
    {"Africa/Luanda", "WAT-1"},
    {"Africa/Lubumbashi", "CAT-2"},
    {"Africa/Lusaka", "CAT-2"},
    {"Africa/Malabo", "WAT-1"},
    {"Africa/Maputo", "CAT-2"},
    {"Africa/Maseru", "SAST-2"},
    {"Africa/Mbabane", "SAST-2"},
    {"Africa/Mogadishu", "EAT-3"},
    {"Africa/Monrovia", "GMT0"},
    {"Africa/Nairobi", "EAT-3"},
    {"Africa/Ndjamena", "WAT-1"},
    {"Africa/Niamey", "WAT-1"},
    {"Africa/Nouakchott", "GMT0"},
    {"Africa/Ouagadougou", "GMT0"},
    {"Africa/Porto-Novo", "WAT-1"},
    {"Africa/Sao_Tome", "GMT0"},
    {"Africa/Tripoli", "EET-2"},
    {"Africa/Tunis", "CET-1"},
    {"Africa/Windhoek", "CAT-2"},
    {"America/Adak", "HST10HDT,M3.2.0,M11.1.0"},
    {"America/Anchorage", "AKST9AKDT,M3.2.0,M11.1.0"},
    {"America/Anguilla", "AST4"},
    {"America/Antigua", "AST4"},
    {"America/Araguaina", "GMT+3"},
    {"America/Argentina/Buenos_Aires", "GMT+3"},
    {"America/Argentina/Catamarca", "GMT+3"},
    {"America/Argentina/Cordoba", "GMT+3"},
    {"America/Argentina/Jujuy", "GMT+3"},
    {"America/Argentina/La_Rioja", "GMT+3"},
    {"America/Argentina/Mendoza", "GMT+3"},
    {"America/Argentina/Rio_Gallegos", "GMT+3"},
    {"America/Argentina/Salta", "GMT+3"},
    {"America/Argentina/San_Juan", "GMT+3"},
    {"America/Argentina/San_Luis", "GMT+3"},
    {"America/Argentina/Tucuman", "GMT+3"},
    {"America/Argentina/Ushuaia", "GMT+3"},
    {"America/Aruba", "AST4"},
    {"America/Asuncion", "GMT+4,M10.1.0/0,M3.4.0/0"},
    {"America/Atikokan", "EST5"},
    {"America/Bahia", "GMT+3"},
    {"America/Bahia_Banderas", "CST6"},
    {"America/Barbados", "AST4"},
    {"America/Belem", "GMT+3"},
    {"America/Belize", "CST6"},
    {"America/Blanc-Sablon", "AST4"},
    {"America/Boa_Vista", "GMT+4"},
    {"America/Bogota", "GMT+5"},
    {"America/Boise", "MST7MDT,M3.2.0,M11.1.0"},
    {"America/Cambridge_Bay", "MST7MDT,M3.2.0,M11.1.0"},
    {"America/Campo_Grande", "GMT+4"},
    {"America/Cancun", "EST5"},
    {"America/Caracas", "GMT+4"},
    {"America/Cayenne", "GMT+3"},
    {"America/Cayman", "EST5"},
    {"America/Chicago", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/Chihuahua", "CST6"},
    {"America/Costa_Rica", "CST6"},
    {"America/Creston", "MST7"},
    {"America/Cuiaba", "GMT+4"},
    {"America/Curacao", "AST4"},
    {"America/Danmarkshavn", "GMT0"},
    {"America/Dawson", "MST7"},
    {"America/Dawson_Creek", "MST7"},
    {"America/Denver", "MST7MDT,M3.2.0,M11.1.0"},
    {"America/Detroit", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Dominica", "AST4"},
    {"America/Edmonton", "MST7MDT,M3.2.0,M11.1.0"},
    {"America/Eirunepe", "GMT+5"},
    {"America/El_Salvador", "CST6"},
    {"America/Fortaleza", "GMT+3"},
    {"America/Fort_Nelson", "MST7"},
    {"America/Glace_Bay", "AST4ADT,M3.2.0,M11.1.0"},
    {"America/Godthab", "GMT+2,M3.5.0/-1,M10.5.0/0"},
    {"America/Goose_Bay", "AST4ADT,M3.2.0,M11.1.0"},
    {"America/Grand_Turk", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Grenada", "AST4"},
    {"America/Guadeloupe", "AST4"},
    {"America/Guatemala", "CST6"},
    {"America/Guayaquil", "GMT+5"},
    {"America/Guyana", "GMT+4"},
    {"America/Halifax", "AST4ADT,M3.2.0,M11.1.0"},
    {"America/Havana", "CST5CDT,M3.2.0/0,M11.1.0/1"},
    {"America/Hermosillo", "MST7"},
    {"America/Indiana/Indianapolis", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Indiana/Knox", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/Indiana/Marengo", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Indiana/Petersburg", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Indiana/Tell_City", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/Indiana/Vevay", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Indiana/Vincennes", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Indiana/Winamac", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Inuvik", "MST7MDT,M3.2.0,M11.1.0"},
    {"America/Iqaluit", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Jamaica", "EST5"},
    {"America/Juneau", "AKST9AKDT,M3.2.0,M11.1.0"},
    {"America/Kentucky/Louisville", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Kentucky/Monticello", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Kralendijk", "AST4"},
    {"America/La_Paz", "GMT+4"},
    {"America/Lima", "GMT+5"},
    {"America/Los_Angeles", "PST8PDT,M3.2.0,M11.1.0"},
    {"America/Lower_Princes", "AST4"},
    {"America/Maceio", "GMT+3"},
    {"America/Managua", "CST6"},
    {"America/Manaus", "GMT+4"},
    {"America/Marigot", "AST4"},
    {"America/Martinique", "AST4"},
    {"America/Matamoros", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/Mazatlan", "MST7"},
    {"America/Menominee", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/Merida", "CST6"},
    {"America/Metlakatla", "AKST9AKDT,M3.2.0,M11.1.0"},
    {"America/Mexico_City", "CST6"},
    {"America/Miquelon", "GMT+3,M3.2.0,M11.1.0"},
    {"America/Moncton", "AST4ADT,M3.2.0,M11.1.0"},
    {"America/Monterrey", "CST6"},
    {"America/Montevideo", "GMT+3"},
    {"America/Montreal", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Montserrat", "AST4"},
    {"America/Nassau", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/New_York", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Nipigon", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Nome", "AKST9AKDT,M3.2.0,M11.1.0"},
    {"America/Noronha", "GMT+2"},
    {"America/North_Dakota/Beulah", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/North_Dakota/Center", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/North_Dakota/New_Salem", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/Ojinaga", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/Panama", "EST5"},
    {"America/Pangnirtung", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Paramaribo", "GMT+3"},
    {"America/Phoenix", "MST7"},
    {"America/Port-au-Prince", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Port_of_Spain", "AST4"},
    {"America/Porto_Velho", "GMT+4"},
    {"America/Puerto_Rico", "AST4"},
    {"America/Punta_Arenas", "GMT+3"},
    {"America/Rainy_River", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/Rankin_Inlet", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/Recife", "GMT+3"},
    {"America/Regina", "CST6"},
    {"America/Resolute", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/Rio_Branco", "GMT+5"},
    {"America/Santarem", "GMT+3"},
    {"America/Santiago", "GMT+4,M9.1.6/24,M4.1.6/24"},
    {"America/Santo_Domingo", "AST4"},
    {"America/Sao_Paulo", "GMT+3"},
    {"America/Scoresbysund", "GMT+1,M3.5.0/0,M10.5.0/1"},
    {"America/Sitka", "AKST9AKDT,M3.2.0,M11.1.0"},
    {"America/St_Barthelemy", "AST4"},
    {"America/St_Johns", "NST3:30NDT,M3.2.0,M11.1.0"},
    {"America/St_Kitts", "AST4"},
    {"America/St_Lucia", "AST4"},
    {"America/St_Thomas", "AST4"},
    {"America/St_Vincent", "AST4"},
    {"America/Swift_Current", "CST6"},
    {"America/Tegucigalpa", "CST6"},
    {"America/Thule", "AST4ADT,M3.2.0,M11.1.0"},
    {"America/Thunder_Bay", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Tijuana", "PST8PDT,M3.2.0,M11.1.0"},
    {"America/Toronto", "EST5EDT,M3.2.0,M11.1.0"},
    {"America/Tortola", "AST4"},
    {"America/Vancouver", "PST8PDT,M3.2.0,M11.1.0"},
    {"America/Whitehorse", "MST7"},
    {"America/Winnipeg", "CST6CDT,M3.2.0,M11.1.0"},
    {"America/Yakutat", "AKST9AKDT,M3.2.0,M11.1.0"},
    {"America/Yellowknife", "MST7MDT,M3.2.0,M11.1.0"},
    {"Antarctica/Casey", "GMT-11"},
    {"Antarctica/Davis", "GMT-7"},
    {"Antarctica/DumontDUrville", "GMT-10"},
    {"Antarctica/Macquarie", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
    {"Antarctica/Mawson", "GMT-5"},
    {"Antarctica/McMurdo", "NZST-12NZDT,M9.5.0,M4.1.0/3"},
    {"Antarctica/Palmer", "GMT+3"},
    {"Antarctica/Rothera", "GMT+3"},
    {"Antarctica/Syowa", "GMT-3"},
    {"Antarctica/Troll", "GMT0,M3.5.0/1,M10.5.0/3"},
    {"Antarctica/Vostok", "GMT-6"},
    {"Arctic/Longyearbyen", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Asia/Aden", "GMT-3"},
    {"Asia/Almaty", "GMT-6"},
    {"Asia/Amman", "GMT-3"},
    {"Asia/Anadyr", "GMT-12"},
    {"Asia/Aqtau", "GMT-5"},
    {"Asia/Aqtobe", "GMT-5"},
    {"Asia/Ashgabat", "GMT-5"},
    {"Asia/Atyrau", "GMT-5"},
    {"Asia/Baghdad", "GMT-3"},
    {"Asia/Bahrain", "GMT-3"},
    {"Asia/Baku", "GMT-4"},
    {"Asia/Bangkok", "GMT-7"},
    {"Asia/Barnaul", "GMT-7"},
    {"Asia/Beirut", "EET-2EEST,M3.5.0/0,M10.5.0/0"},
    {"Asia/Bishkek", "GMT-6"},
    {"Asia/Brunei", "GMT-8"},
    {"Asia/Chita", "GMT-9"},
    {"Asia/Choibalsan", "GMT-8"},
    {"Asia/Colombo", "GMT-5:30"},
    {"Asia/Damascus", "GMT-3"},
    {"Asia/Dhaka", "GMT-6"},
    {"Asia/Dili", "GMT-9"},
    {"Asia/Dubai", "GMT-4"},
    {"Asia/Dushanbe", "GMT-5"},
    {"Asia/Famagusta", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Asia/Gaza", "EET-2EEST,M3.4.4/50,M10.4.4/50"},
    {"Asia/Hebron", "EET-2EEST,M3.4.4/50,M10.4.4/50"},
    {"Asia/Ho_Chi_Minh", "GMT-7"},
    {"Asia/Hong_Kong", "HKT-8"},
    {"Asia/Hovd", "GMT-7"},
    {"Asia/Irkutsk", "GMT-8"},
    {"Asia/Jakarta", "WIB-7"},
    {"Asia/Jayapura", "WIT-9"},
    {"Asia/Jerusalem", "IST-2IDT,M3.4.4/26,M10.5.0"},
    {"Asia/Kabul", "GMT-4:30"},
    {"Asia/Kamchatka", "GMT-12"},
    {"Asia/Karachi", "PKT-5"},
    {"Asia/Kathmandu", "GMT-5:45"},
    {"Asia/Khandyga", "GMT-9"},
    {"Asia/Kolkata", "IST-5:30"},
    {"Asia/Krasnoyarsk", "GMT-7"},
    {"Asia/Kuala_Lumpur", "GMT-8"},
    {"Asia/Kuching", "GMT-8"},
    {"Asia/Kuwait", "GMT-3"},
    {"Asia/Macau", "CST-8"},
    {"Asia/Magadan", "GMT-11"},
    {"Asia/Makassar", "WITA-8"},
    {"Asia/Manila", "PST-8"},
    {"Asia/Muscat", "GMT-4"},
    {"Asia/Nicosia", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Asia/Novokuznetsk", "GMT-7"},
    {"Asia/Novosibirsk", "GMT-7"},
    {"Asia/Omsk", "GMT-6"},
    {"Asia/Oral", "GMT-5"},
    {"Asia/Phnom_Penh", "GMT-7"},
    {"Asia/Pontianak", "WIB-7"},
    {"Asia/Pyongyang", "KST-9"},
    {"Asia/Qatar", "GMT-3"},
    {"Asia/Qyzylorda", "GMT-5"},
    {"Asia/Riyadh", "GMT-3"},
    {"Asia/Sakhalin", "GMT-11"},
    {"Asia/Samarkand", "GMT-5"},
    {"Asia/Seoul", "KST-9"},
    {"Asia/Shanghai", "CST-8"},
    {"Asia/Singapore", "GMT-8"},
    {"Asia/Srednekolymsk", "GMT-11"},
    {"Asia/Taipei", "CST-8"},
    {"Asia/Tashkent", "GMT-5"},
    {"Asia/Tbilisi", "GMT-4"},
    {"Asia/Tehran", "GMT-3:30"},
    {"Asia/Thimphu", "GMT-6"},
    {"Asia/Tokyo", "JST-9"},
    {"Asia/Tomsk", "GMT-7"},
    {"Asia/Ulaanbaatar", "GMT-8"},
    {"Asia/Urumqi", "GMT-6"},
    {"Asia/Ust-Nera", "GMT-10"},
    {"Asia/Vientiane", "GMT-7"},
    {"Asia/Vladivostok", "GMT-10"},
    {"Asia/Yakutsk", "GMT-9"},
    {"Asia/Yangon", "GMT-6:30"},
    {"Asia/Yekaterinburg", "GMT-5"},
    {"Asia/Yerevan", "GMT-4"},
    {"Atlantic/Azores", "GMT+1,M3.5.0/0,M10.5.0/1"},
    {"Atlantic/Bermuda", "AST4ADT,M3.2.0,M11.1.0"},
    {"Atlantic/Canary", "WET0WEST,M3.5.0/1,M10.5.0"},
    {"Atlantic/Cape_Verde", "GMT+1"},
    {"Atlantic/Faroe", "WET0WEST,M3.5.0/1,M10.5.0"},
    {"Atlantic/Madeira", "WET0WEST,M3.5.0/1,M10.5.0"},
    {"Atlantic/Reykjavik", "GMT0"},
    {"Atlantic/South_Georgia", "GMT+2"},
    {"Atlantic/Stanley", "GMT+3"},
    {"Atlantic/St_Helena", "GMT0"},
    {"Australia/Adelaide", "ACST-9:30ACDT,M10.1.0,M4.1.0/3"},
    {"Australia/Brisbane", "AEST-10"},
    {"Australia/Broken_Hill", "ACST-9:30ACDT,M10.1.0,M4.1.0/3"},
    {"Australia/Currie", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
    {"Australia/Darwin", "ACST-9:30"},
    {"Australia/Eucla", "GMT-8:45"},
    {"Australia/Hobart", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
    {"Australia/Lindeman", "AEST-10"},
    {"Australia/Lord_Howe", "GMT-10:30,M10.1.0,M4.1.0"},
    {"Australia/Melbourne", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
    {"Australia/Perth", "AWST-8"},
    {"Australia/Sydney", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
    {"Europe/Amsterdam", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Andorra", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Astrakhan", "GMT-4"},
    {"Europe/Athens", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Europe/Belgrade", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Berlin", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Bratislava", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Brussels", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Bucharest", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Europe/Budapest", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Busingen", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Chisinau", "EET-2EEST,M3.5.0,M10.5.0/3"},
    {"Europe/Copenhagen", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Dublin", "IST-1GMT0,M10.5.0,M3.5.0/1"},
    {"Europe/Gibraltar", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Guernsey", "GMT0BST,M3.5.0/1,M10.5.0"},
    {"Europe/Helsinki", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Europe/Isle_of_Man", "GMT0BST,M3.5.0/1,M10.5.0"},
    {"Europe/Istanbul", "GMT-3"},
    {"Europe/Jersey", "GMT0BST,M3.5.0/1,M10.5.0"},
    {"Europe/Kaliningrad", "EET-2"},
    {"Europe/Kiev", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Europe/Kirov", "MSK-3"},
    {"Europe/Lisbon", "WET0WEST,M3.5.0/1,M10.5.0"},
    {"Europe/Ljubljana", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/London", "GMT0BST,M3.5.0/1,M10.5.0"},
    {"Europe/Luxembourg", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Madrid", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Malta", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Mariehamn", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Europe/Minsk", "GMT-3"},
    {"Europe/Monaco", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Moscow", "MSK-3"},
    {"Europe/Oslo", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Paris", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Podgorica", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Prague", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Riga", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Europe/Rome", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Samara", "GMT-4"},
    {"Europe/San_Marino", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Sarajevo", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Saratov", "GMT-4"},
    {"Europe/Simferopol", "MSK-3"},
    {"Europe/Skopje", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Sofia", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Europe/Stockholm", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Tallinn", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Europe/Tirane", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Ulyanovsk", "GMT-4"},
    {"Europe/Uzhgorod", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Europe/Vaduz", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Vatican", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Vienna", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Vilnius", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Europe/Volgograd", "MSK-3"},
    {"Europe/Warsaw", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Zagreb", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe/Zaporozhye", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"Europe/Zurich", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Indian/Antananarivo", "EAT-3"},
    {"Indian/Chagos", "GMT-6"},
    {"Indian/Christmas", "GMT-7"},
    {"Indian/Cocos", "GMT-6:30"},
    {"Indian/Comoro", "EAT-3"},
    {"Indian/Kerguelen", "GMT-5"},
    {"Indian/Mahe", "GMT-4"},
    {"Indian/Maldives", "GMT-5"},
    {"Indian/Mauritius", "GMT-4"},
    {"Indian/Mayotte", "EAT-3"},
    {"Indian/Reunion", "GMT-4"},
    {"Pacific/Apia", "GMT-13"},
    {"Pacific/Auckland", "NZST-12NZDT,M9.5.0,M4.1.0/3"},
    {"Pacific/Bougainville", "GMT-11"},
    {"Pacific/Chatham", "GMT-12:45,M9.5.0/2:45,M4.1.0/3:45"},
    {"Pacific/Chuuk", "GMT-10"},
    {"Pacific/Easter", "GMT+6,M9.1.6/22,M4.1.6/22"},
    {"Pacific/Efate", "GMT-11"},
    {"Pacific/Enderbury", "GMT-13"},
    {"Pacific/Fakaofo", "GMT-13"},
    {"Pacific/Fiji", "GMT-12"},
    {"Pacific/Funafuti", "GMT-12"},
    {"Pacific/Galapagos", "GMT+6"},
    {"Pacific/Gambier", "GMT+9"},
    {"Pacific/Guadalcanal", "GMT-11"},
    {"Pacific/Guam", "ChST-10"},
    {"Pacific/Honolulu", "HST10"},
    {"Pacific/Kiritimati", "GMT-14"},
    {"Pacific/Kosrae", "GMT-11"},
    {"Pacific/Kwajalein", "GMT-12"},
    {"Pacific/Majuro", "GMT-12"},
    {"Pacific/Marquesas", "GMT+9:30"},
    {"Pacific/Midway", "SST11"},
    {"Pacific/Nauru", "GMT-12"},
    {"Pacific/Niue", "GMT+11"},
    {"Pacific/Norfolk", "GMT-11,M10.1.0,M4.1.0/3"},
    {"Pacific/Noumea", "GMT-11"},
    {"Pacific/Pago_Pago", "SST11"},
    {"Pacific/Palau", "GMT-9"},
    {"Pacific/Pitcairn", "GMT+8"},
    {"Pacific/Pohnpei", "GMT-11"},
    {"Pacific/Port_Moresby", "GMT-10"},
    {"Pacific/Rarotonga", "GMT+10"},
    {"Pacific/Saipan", "ChST-10"},
    {"Pacific/Tahiti", "GMT+10"},
    {"Pacific/Tarawa", "GMT-12"},
    {"Pacific/Tongatapu", "GMT-13"},
    {"Pacific/Wake", "GMT-12"},
    {"Pacific/Wallis", "GMT-12"},
    {NULL, NULL}};

static char lower(char start)
{
  if ('A' <= start && start <= 'Z')
  {
    return start - 'A' + 'a';
  }
  return start;
}

/**
 * Basically strcmp, but accounting for spaces that have become underscores
 * @param[in] target - the 0-terminated string on the left hand side of the comparison
 * @param[in] other - the 0-terminated string on the right hand side of the comparison
 * @return > 0 if target comes before other alphabetically,
 *         ==0 if they're the same,
 *         < 0 if other comes before target alphabetically
 *         (we don't expect NULL arguments, but, -1 if either is NULL)
 **/
static int tz_name_cmp(const char *target, const char *other)
{
  if (!target || !other)
  {
    return -1;
  }

  while (*target)
  {
    if (lower(*target) != lower(*other))
    {
      break;
    }
    do
    {
      target++;
    } while (*target == '_');
    do
    {
      other++;
    } while (*other == '_');
  }

  return lower(*target) - lower(*other);
}

const char *micro_tz_db_get_posix_str(const char *name)
{
  int lo = 0, hi = sizeof(micro_tz_db_tzs) / sizeof(micro_tz_db_pair);
  while (lo < hi)
  {
    int mid = (lo + hi) / 2;
    micro_tz_db_pair mid_pair = micro_tz_db_tzs[mid];
    int comparison = tz_name_cmp(name, mid_pair.name);
    if (comparison == 0)
    {
      return mid_pair.posix_str;
    }
    else if (comparison < 0)
    {
      hi = mid;
    }
    else
    {
      lo = mid + 1;
    }
  }
  return NULL;
}

const micro_tz_db_pair *get_tz_db(void)
{
  return micro_tz_db_tzs;
}
#ifndef FAKE_LANGINFO_H
#define FAKE_LANGINFO_H

typedef char * nl_item;

#define nl_langinfo(item) item

#define RADIXCHAR "."
#define THOUSEP   ","

#define D_FMT   "%m.%d.%Y"

#define DAY_1   "Sunday"
#define DAY_2   "Monday"
#define DAY_3   "Tuesday"
#define DAY_4   "Wednesday"
#define DAY_5   "Thursday"
#define DAY_6   "Friday"
#define DAY_7   "Saturday"

#define MON_1   "January"
#define MON_2   "February"
#define MON_3   "March"
#define MON_4   "April"
#define MON_5   "May"
#define MON_6   "June"
#define MON_7   "July"
#define MON_8   "August"
#define MON_9   "September"
#define MON_10  "October"
#define MON_11  "November"
#define MON_12  "December"

#define ABMON_1     "Jan"
#define ABMON_2     "Feb"
#define ABMON_3     "Mar"
#define ABMON_4     "Apr"
#define ABMON_5     "May"
#define ABMON_6     "Jun"
#define ABMON_7     "Jul"
#define ABMON_8     "Aug"
#define ABMON_9     "Sep"
#define ABMON_10    "Oct"
#define ABMON_11    "Nov"
#define ABMON_12    "Dec"

#endif

